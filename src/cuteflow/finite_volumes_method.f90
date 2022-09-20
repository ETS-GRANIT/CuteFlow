module finite_volumes_method
  implicit none
contains
  subroutine solve_finite_volumes
    use mpi
    use precision_kind
    use global_data
    use file_id
    use pre_post_traitement
    use flux_riemann
    use cfl_condition
    use source_terms
    use memory_exchange_functions
    use maillage

    implicit none

    !*** Variables locales ***
    integer :: stat(MPI_STATUS_SIZE)
    integer :: iter_nodes, iter_entree
    integer :: i, j, k

    real(fp_kind)   :: time_start, time_stop
    integer         :: ierr

    tc          = 0.d0
    t_reg_perm  = 0.d0

    nt             = 0 
    reg_perm_reached  = .FALSE.

    allocate(resc(ndln,nelt), res(ndln,nelt))

    resc = 0.d0
    res = 0.d0

    allocate(sourcfric(ndln,nelt))
    allocate(sourcbath(ndln,nelt))
    allocate(io_identifier(nelt))

    allocate(afm1(nelt, 9))

    if (ndi>0) allocate(debit_entree_arr(ndi-1,2))
    if (ndo>0) allocate(debit_sortie_arr(ndo-1))

    call get_io_index

    if(num_mpi_process > 1 .and. is_cgns==1) then
      nelt_send_tot = 0
      nelt_nonsend = nelt
      do i=1,nelt_fant_envoi_bloc
        nelt_send_tot = nelt_send_tot + elt_fant_envoi_list(i)%length
        do j=1,elt_fant_envoi_list(i)%length
          nelt_nonsend = min(nelt_nonsend, elt_fant_envoi_list(i)%elem_id(j))
        end do
      end do

      allocate(elt_fant_envoi_list_glob(nelt_send_tot))

      k=1
      do i=1,nelt_fant_envoi_bloc
        do j=1,elt_fant_envoi_list(i)%length
          elt_fant_envoi_list_glob(k) = elt_fant_envoi_list(i)%elem_id(j)
          k=k+1
        end do
      end do
    end if

    !!Initial dt exchange
    call cflcond(vdlg0)
    if(num_mpi_process>1) call MPI_ALLREDUCE(MPI_IN_PLACE,dt,1,fp_kind_mpi,MPI_MIN,MPI_COMM_WORLD, mpi_ierr)

    !!CPU STOP HERE

    write(*,*) 'Loop over time starts' 
    do while( (tc - ts + 2*tol ) < tol .and. .not.(reg_perm_reached))
      is_solnode = .FALSE.

      nt = nt + 1

      debit_entree  = 0.d0
      debit_sortie  = 0.d0

      if(iupwind==3) call update_sol_nodes 

      if(inlet=="phys_inflow" .and. nombre_entrees>0) then
          surf_entree(:) = 0.d0
          call calcul_surf_entree()
          call mpi_allreduce(mpi_in_place,surf_entree,nombre_entrees,fp_kind_mpi,mpi_sum,mpi_comm_world, mpi_ierr)
      end if

      call compute_flux
      call compute_source_terms
      call update_sol_with_flux_and_source

      !!Memory exchange is started
      if(num_mpi_process>1 .and. is_cgns==1) then
        call update_send_cells
      end if

      !!  Echange sur GPU directement bloc
      if(num_mpi_process>1) then
        if(is_cgns==1) then
          call gpu_echange_cgns(vdlg, ndln)
        else
          call gpu_echange_fant_bloc_async(vdlg, ndln)
        end if
      end if

      !Compute outflow
      if(ndo>1) then
        debit_sortie  =  debit_sortie + sum(debit_sortie_arr)
      end if

      !Compute inflow
      if(ndi>1) then
        !En fait on parcours les arretes numérote dans get_io_index
        do iter_nodes=1,ndi-1
          !!iter_entree = numero_ndinput(iter_nodes)
          iter_entree = debit_entree_arr(iter_nodes,2)
          debit_entree(iter_entree) = debit_entree(iter_entree) + debit_entree_arr(iter_nodes,1)
        end do
      end if

      !!Reduce sum of all debits
      if(num_mpi_process>1) then
        call MPI_ALLREDUCE(MPI_IN_PLACE,debit_entree,nombre_entrees,fp_kind_mpi,MPI_SUM,MPI_COMM_WORLD, mpi_ierr)
        call MPI_ALLREDUCE(MPI_IN_PLACE,debit_sortie,1,fp_kind_mpi,MPI_SUM,MPI_COMM_WORLD, mpi_ierr)
      end if

      call compute_time_step

      !!Attendre fin de l'échange mémoire de vdlg
      if(num_mpi_process>1) then
        call MPI_WAITALL(size(reqsend),reqsend,sendstat,mpi_ierr)
        call MPI_WAITALL(size(reqrecv),reqrecv,recvstat,mpi_ierr)
      end if

      vdlg0 = vdlg

      tc = tc + dt

      !!Verifie la condition de régime stationnaire
      call check_steady_state

      !!Affiche l'avancement à l'écran
      call print_status_in_console

      !!Sauvegarde la solution de différentes manières si requis
      call global_save_sol_visu
    enddo 

    write(*,*) 'Loop on time ends at iteration : ', nt

    if(reg_perm_reached) tc = t_reg_perm 

    if(solrestart==1) call save_sol_for_restart

    if(sortie_finale_bluekenue==1) call bluekenue_export

  end subroutine solve_finite_volumes

  subroutine update_sol_with_flux_and_source
    use precision_kind
    use global_data

    implicit none

    integer :: gi, ggi,nelt_limit
    real(fp_kind) :: resini(3)

    do gi=1,nelt
      if ( friction == 1) then
        if ( fricimplic == 1 .or. fricimplic == 2 ) then
          resini = ( sourcbath(:,gi) + sourcfric(:,gi) - resc(:,gi) ) / surf(gi)
          res(1,gi) = sum(afm1(gi,1:3)*resini)
          res(2,gi) = sum(afm1(gi,4:6)*resini)
          res(3,gi) = sum(afm1(gi,7:9)*resini)
        else
          res(:,gi) = (sourcbath(:,gi)+ sourcfric(:,gi) - resc(:,gi)) / surf(gi)
        endif
      else 
        res(:,gi) = (sourcbath(:,gi) - resc(:,gi)) / surf(gi)
      endif

      vdlg(:,gi) = vdlg0(:,gi) + dt * res(:,gi)

      if(vdlg(1,gi) <= tolisec) then
        vdlg(1,gi) = tolisec
        vdlg(2,gi) = 0.
        vdlg(3,gi) = 0.
      end if
    end do
  end subroutine update_sol_with_flux_and_source

  subroutine get_io_index
    use precision_kind
    use global_data
    implicit none
    integer :: i, j, k, counter

    if(ndi>0) then
      debit_entree_arr = 0.d0
    endif
    if(ndo>0) then
      debit_sortie_arr = 0.d0
    endif

    io_identifier = 0

    counter = 1
    do i = 1, nelt
      if(boundary(i,1,1) == -1 .or. boundary(i,2,1) == -1 .or. boundary(i,3,1) == -1) then 
        io_identifier(i) =  counter
        counter = counter + 1
      endif
    enddo

    counter = 1
    do i = 1, nelt
      if(boundary(i,1,1) == -2 .or. boundary(i,2,1) == -2 .or. boundary(i,3,1) == -2) then 
        io_identifier(i) =  counter
        counter = counter + 1
      endif
    enddo
  end subroutine get_io_index
  
  subroutine print_status_in_console 
    use precision_kind
    use global_data
    implicit none

    integer :: iter_entree

    ! *** Affichage de l'avancement ***
    if ( mod(nt,freqaffich)==0 ) then
      print*,'-----------------------------------------------'

      write(*,*) 'Iteration number ', nt, ", t = ", tc
      ! if(solvisu>0) print*,'Next solution time save = ', tsolvisu(cptsolvisu), cptsolvisu

      do iter_entree=1,nombre_entrees
        print*, 'debit_entree ', iter_entree, " = " ,debit_entree(iter_entree)
      end do

      print*, 'debit_entree_sum =', debit_entree_sum, 'M3/S'  
      print*, 'debit_sortie     =', debit_sortie, 'M3/S'  

      print*,'-----------------------------------------------'
    endif

  end subroutine print_status_in_console 


  subroutine check_steady_state
    use precision_kind
    use global_data
    implicit none

    integer :: iter_entree

    debit_entree_sum = sum(debit_entree)

    if ( debit_entree_sum > 1e-8 .and. abs(debit_entree_sum - debit_sortie)/debit_entree_sum < tol_reg_perm ) then
      iter_reg_perm = iter_reg_perm + 1
      if( iter_reg_perm > 1000) then
        print*,'-----------------------------------------------'
        print*, 'LE REGIME PERMANENT EST ATTEINT'
        print*,'-----------------------------------------------'
        t_reg_perm = tc

        reg_perm_reached = .TRUE.
      end if
    else
      iter_reg_perm = 0
    endif

  end subroutine check_steady_state
end module finite_volumes_method
