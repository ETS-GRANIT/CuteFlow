module maillage
  implicit none
contains
  subroutine lecture_maillage

    use precision_kind
    use global_data
    use file_id
    use mpi

    implicit none

    integer                     ::  premnoeud, deuxnoeud

    integer                     :: i, nc, kvol, nc1, nc2, ierr
    real(fp_kind)               :: x1, x2, y1, y2, ln, errf
    integer                     :: nc3
    real(fp_kind)               :: x3, y3, p, ca, cb, cc, aire
    real(fp_kind)               :: start_cpu, stop_cpu, errf1, errf2, errf3

    !! ns = nombre de sommet ici 3 pour des triangles
    ns = 3

    print*,mpi_process_id, ' lecture du maillage en cours'
    if(is_cgns==1) then
      call lec_mail_cgns
    else
      call lec_mail_tri
    end if
    print*,mpi_process_id, ' lecture du maillage reussie'
    call MPI_BARRIER(MPI_COMM_WORLD,mpi_ierr)

    allocate(boundary(nelt,3,2))

    ! if(iflux >=3 .and. iupwind==2) then
    if(.TRUE.) then
      allocate(vertex_neighbors(6,nnt))
      vertex_neighbors = -1.
    end if

    if (elt_bound==0) then
      call tab_bound_tri
      print*,mpi_process_id,"tab_bound_tri ok", nelt, nnt
      call MPI_BARRIER(MPI_COMM_WORLD,mpi_ierr)
    endif

    call construct_vertex_neighbors

    !!  Construction/Lecture des éléments voisins
    if(num_mpi_process>1) then
      file_bc = trim(adjustl(mpi_process_id_string)) //'_boundary_table_' // trim(adjustl(meshfile))
    else
      file_bc = 'boundary_table_' // trim(adjustl(meshfile))
    end if

    if (elt_bound==0) then
      !! On construit puis sauvegarde la table des éléments voisins
      open(unit=1100,file=file_bc,status="unknown")
      call boundary_kind
      print*,mpi_process_id,"boundary kind ok"
      call MPI_BARRIER(MPI_COMM_WORLD,mpi_ierr)

      do i=1,nelt
        write(1100,*) boundary(i,1,1), boundary(i,2,1), boundary(i,3,1), boundary(i,1,2), boundary(i,2,2), boundary(i,3,2)
      enddo
      close(1100)
    else
      !! On lit la table des éléments voisins
      open(unit=1101,file=file_bc,status="unknown")
      do i=1,nelt
        read(1101,*) boundary(i,1,1), boundary(i,2,1), boundary(i,3,1), boundary(i,1,2), boundary(i,2,2), boundary(i,3,2)
      enddo
      close(1101)

    endif

    !!Si iflux==3 alors on calcule les elements upwind de chaque coté de chaque cellule
    if(iflux>=3) then
      allocate(upwind_cells(2,3,nelt)) !!Pas dealocate pour l'instant
      call find_upwind_cells()
    end if


    !! Calcul des longueurs des entrées
    if(nombre_entrees > 0) then
      long_entree = 0.
      if(ndi > 0) then 
        call calcul_long_entree
        call compute_input_elems
        surf_entree = 0.
      end if

      call mpi_allreduce(mpi_in_place,long_entree,nombre_entrees,fp_kind_mpi,mpi_sum,mpi_comm_world, mpi_ierr)
    end if

    !! calculation of the elementwise coordinates table
    allocate(coo_table_elemwise(nelt,3*ns))
    allocate(x_centroid(nelt), y_centroid(nelt))

    call construct_coo_table_elemwise
    print*,mpi_process_id,"apres contruct coo table elemwise"
    call MPI_BARRIER(MPI_COMM_WORLD,mpi_ierr)


    !! distance cote entre 1er et 2eme noeuds ***
    premnoeud = connectivite(1,1)
    deuxnoeud = connectivite(1,2)

    distcote = sqrt( ( coordonnees(premnoeud,1) - coordonnees(deuxnoeud,1) )**2 + &
      ( coordonnees(premnoeud,2) - coordonnees(deuxnoeud,2) )**2 )

    !! Calcul de la longueur caracteristique pour le calcul du pas de temps via la cfl
    !! On choisit de prendre le rayon du cercle inscrit dans l triangle

    allocate(cotemin_arr(nelt))
    cotemin = 1000000.
    do kvol=1,nelt 
      nc1 = connectivite(kvol,1)
      nc2 = connectivite(kvol,2)
      nc3 = connectivite(kvol,3)

      x1 = coordonnees(nc1,1)
      y1 = coordonnees(nc1,2)

      x2 = coordonnees(nc2,1)
      y2 = coordonnees(nc2,2)

      x3 = coordonnees(nc3,1)
      y3 = coordonnees(nc3,2)

      ca = sqrt((x1-x2)**2+(y1-y2)**2)
      cb = sqrt((x2-x3)**2+(y2-y3)**2)
      cc = sqrt((x3-x1)**2+(y3-y1)**2)

      p = (ca + cb+ cc)/2.

      aire = sqrt(p*(p-ca)*(p-cb)*(p-cc))

      cotemin_arr(kvol) = 0.5*aire/p
      cotemin_arr(kvol) = 1.8 * cotemin_arr(kvol) !! 1.8 * rayon du cercle inscrit
      cotemin = min(cotemin,cotemin_arr(kvol))
    enddo

    call MPI_BARRIER(MPI_COMM_WORLD,mpi_ierr)
    call mpi_allreduce(mpi_in_place,cotemin,1,fp_kind_mpi,mpi_min,mpi_comm_world, mpi_ierr)
    call MPI_BARRIER(MPI_COMM_WORLD,mpi_ierr)
  end subroutine lecture_maillage

  subroutine lec_donnees

    use precision_kind
    use global_data
    use file_id

    implicit none

    open(lu_fich,file='donnees.f',form='formatted',status='old')
    read(lu_fich,nml=donnees_namelist)
    close(lu_fich)

    inlet = trim(adjustl(inlet))

    !! Ajout du numéro de process MPI en préfixe des fichiers de maillage et de solution initiale
    if(num_mpi_process>1) then
      if(is_cgns==0) then
        meshfile = trim(mpi_process_id_string)//'_'//trim(meshfile)
      else
        meshfile = trim(meshfile)
      end if
      fich_sol_init = trim(mpi_process_id_string)//'_'//trim(fich_sol_init)
    else
      fich_sol_init = trim(fich_sol_init)
    end if

  end subroutine lec_donnees

  subroutine lec_mail_tri
    ! ======================================================================
    !     lec_mail_tri = routine de lecture d'un maillage triangulaire
    !     auteur : Delmas vincent
    !     version : 1.0  ;  may 28 2019
    ! ======================================================================

    use precision_kind
    use global_data
    use file_id

    implicit none

    character(len=1) :: tarati
    integer          :: i, n, ierr, curproc

    open(lec_mesh,file=trim(adjustl(meshfile_path))//trim(adjustl(meshfile)),form='formatted',status='old')

    !! table des coordonnees :
    read(lec_mesh,*) tarati
    read(lec_mesh,*) nnt
    print*,"id ", mpi_process_id, ", ", nnt, " nodes"
    allocate(coordonnees(nnt,3), manning_nd(nnt))

    do i=1,nnt
      read(lec_mesh,*) n, coordonnees(i,1), coordonnees(i,2), coordonnees(i,3) , manning_nd(i)
    enddo

    !! table de connectivité :
    read(lec_mesh,*) tarati
    if(num_mpi_process>1) then
      read(lec_mesh,*) nelt, nelt_non_fant
      nelt_fant_recep = nelt-nelt_non_fant
    else
      read(lec_mesh,*) nelt
      nelt_non_fant=nelt
      nelt_fant_recep = 0
    end if
    print*,"id ", mpi_process_id, ", ", nelt, " elems"
    allocate(connectivite(nelt,3), manning_nelt(nelt))

    do i=1,nelt
      read(lec_mesh,*) n,connectivite(i,1),connectivite(i,2),connectivite(i,3), manning_nelt(i)
    enddo

    !! table des noeuds de l'entrée :
    read(lec_mesh,*) tarati
    if(nombre_entrees==1) then
      read(lec_mesh,*) ndi
      allocate(long_entree(1))
      allocate(surf_entree(1))
      if (ndi>0) then
        allocate(ndinput(ndi))
        allocate(numero_ndinput(ndi))
        do i=1,ndi
          read(lec_mesh,*) ndinput(i)
          numero_ndinput(i) = 1
        enddo
      endif
    else
      read(lec_mesh,*) ndi, nombre_entrees
      allocate(long_entree(nombre_entrees))
      allocate(surf_entree(nombre_entrees))
      if (ndi>0) then
        allocate(ndinput(ndi))
        allocate(numero_ndinput(ndi))
        do i=1,ndi
          read(lec_mesh,*) ndinput(i), numero_ndinput(i)
        enddo
      endif
    end if
    print*,"id ", mpi_process_id, ", ", ndi, " noeuds d'entree"

    !! table des noeuds de la sortie :
    read(lec_mesh,*) tarati
    if(nombre_sorties==1) then
      read(lec_mesh,*) ndo
      if (ndo>0) then
        allocate(numero_ndoutput(ndo))
        allocate(ndoutput(ndo))
        do i=1,ndo
          read(lec_mesh,*) ndoutput(i)
          numero_ndoutput(i) = 1
        enddo
      end if
    else
      read(lec_mesh,*) ndo, nombre_sorties
      if(ndo>0) then
        allocate(numero_ndoutput(ndo))
        allocate(ndoutput(ndo))
        do i=1,ndo
          read(lec_mesh,*) ndoutput(i), numero_ndoutput(i)
        enddo
      endif
    endif
    print*,"id ", mpi_process_id, ", ", ndo, " noeuds de sortie"

    !! table des noeuds de la frontière solide :
    read(lec_mesh,*) tarati
    read(lec_mesh,*) ndw
    print*,"id ", mpi_process_id, ", ", ndw, " noeuds de murs"
    if (ndw>0) then
      allocate(ndwall(ndw))
      do i=1,ndw
        read(lec_mesh,*) ndwall(i)
      enddo
    endif

    if(num_mpi_process>1) then
      ! !! table des mailles fantomes a recep:
      ! read(lec_mesh,*) tarati
      ! read(lec_mesh,*) nelt_fant_recep
      ! print*,"id ", mpi_process_id, ", ", nelt_fant_recep, " mailles fantomes a recep"
      ! if (nelt_fant_recep>0) then
      !   allocate(elt_fant_recep(nelt_fant_recep,3))
      !   do i=1,nelt_fant_recep
      !     read(lec_mesh,*) elt_fant_recep(i, 1), elt_fant_recep(i, 2), elt_fant_recep(i, 3)
      !   enddo
      ! endif

      ! !! table des mailles fantomes a envoyer:
      ! read(lec_mesh,*) tarati
      ! read(lec_mesh,*) nelt_fant_envoi
      ! print*,"id ", mpi_process_id, ", ", nelt_fant_envoi, " mailles fantomes a envoyer"
      ! if (nelt_fant_envoi>0) then
      !   allocate(elt_fant_envoi(nelt_fant_envoi,3))
      !   do i=1,nelt_fant_envoi
      !     read(lec_mesh,*) elt_fant_envoi(i, 1), elt_fant_envoi(i, 2), elt_fant_envoi(i, 3)
      !   enddo
      ! endif

      !! table des mailles fantomes à receptionner par bloc
      read(lec_mesh,*) tarati
      read(lec_mesh,*) nelt_fant_recep_bloc
      print*,"id ", mpi_process_id, ", ", nelt_fant_recep_bloc, " bloc fantomes a receptionner"
      if (nelt_fant_recep_bloc>0) then
        allocate(elt_fant_recep_bloc(nelt_fant_recep_bloc,3))
        do i=1,nelt_fant_recep_bloc
          read(lec_mesh,*) elt_fant_recep_bloc(i, 1), elt_fant_recep_bloc(i, 2), elt_fant_recep_bloc(i, 3)
          print*, elt_fant_recep_bloc(i, 1), elt_fant_recep_bloc(i, 2), elt_fant_recep_bloc(i, 3)
        enddo
      endif

      !! table des mailles fantomes à envoyer par bloc
      read(lec_mesh,*) tarati
      read(lec_mesh,*) nelt_fant_envoi_bloc
      print*,"id ", mpi_process_id, ", ", nelt_fant_envoi_bloc, " bloc fantomes a envoyer"
      if (nelt_fant_envoi_bloc>0) then
        allocate(elt_fant_envoi_bloc(nelt_fant_envoi_bloc,3))
        do i=1,nelt_fant_envoi_bloc
          read(lec_mesh,*) elt_fant_envoi_bloc(i, 1), elt_fant_envoi_bloc(i, 2), elt_fant_envoi_bloc(i, 3)
          print*, elt_fant_envoi_bloc(i, 1), elt_fant_envoi_bloc(i, 2), elt_fant_envoi_bloc(i, 3)
        enddo
      endif

    end if 
    close(lec_mesh)
  end subroutine lec_mail_tri

  subroutine lec_mail_cgns
    use global_data
    use precision_kind
    use cgns

    implicit none

    integer :: dummy_int, dummy_wall, i, j, it
    real(fp_kind) :: dummy_real
    character(200) :: dummy_char
    logical :: found

    integer :: ierr, cellDim, physDim
    integer :: fn, n_bases, base, n_zones, zone, zoneType, n_sections, section, ord
    integer :: n_bocos, boco, bocoType, normListFlag, normDataType, nDataSet, n_bcnodes 
    integer, dimension(3) :: normalIndex
    integer :: ptSetType, n_userdata, n_ptSet, n_userdata2
    integer :: sizes(3*3), sectionType, nBeg, nEnd, eBeg, eEnd, nBdry, flag
    character(200) :: filename, basename, command, zonename, sectionname, boconame
    character(200) :: userdataname, userdataname2

    real(fp_kind), dimension(:), allocatable :: coord
    integer, dimension(:), allocatable :: conn

    meshfile_out = trim(adjustl("out_"))//trim(adjustl(meshfile))
    meshfile_nodes_out = trim(adjustl("out_nodes_"))//trim(adjustl(meshfile))

    !!Copy meshfile localy and use it for further processing
    if(mpi_process_id==0) then
      if(solvisu==1 .or. solvisu==3) then
        write(command, *) "cp "//trim(adjustl(meshfile_path))//trim(adjustl(meshfile))//&
          &" "//trim(adjustl(meshfile_out))
        call system(command)
        ! write(command, *) "cp "//trim(adjustl(meshfile_path))//trim(adjustl(meshfile))//&
        !   &" "//trim(adjustl(meshfile_nodes_out))
        ! call system(command)
      end if
    end if
    call MPI_BARRIER(MPI_COMM_WORLD,mpi_ierr)

    call cg_open_f(trim(adjustl(meshfile_path))//trim(adjustl(meshfile)), CG_MODE_READ, fn, ierr)
    if (ierr .ne. CG_OK) call cg_error_exit_f

    call cg_nbases_f(fn, n_bases, ierr)
    if (ierr .ne. CG_OK) call cg_error_exit_f
    base = 1

    call cg_base_read_f(fn, base, basename, cellDim, physDim, ierr)
    if (ierr .ne. CG_OK) call cg_error_exit_f

    call cg_nzones_f(fn, base, n_zones, ierr)
    if (ierr .ne. CG_OK) call cg_error_exit_f

    do zone=1,n_zones
      call cg_zone_type_f(fn, base, zone, zoneType, ierr)
      if (ierr .ne. CG_OK) call cg_error_exit_f

      call cg_zone_read_f(fn, base, zone, zonename, sizes, ierr)
      if (ierr .ne. CG_OK) call cg_error_exit_f

      call cg_goto_f(fn, base, ierr, trim(adjustl(zonename)), 0, "end")
      if (ierr .ne. CG_OK) call cg_error_exit_f

      call cg_ordinal_read_f(ord, ierr)
      if (ierr .ne. CG_OK) call cg_error_exit_f

      if(ord == mpi_process_id) then
        exit
      end if
    end do

    print*,mpi_process_id,zone


    nnt = sizes(1)
    nelt = sizes(2)

    allocate(coord(nnt), coordonnees(nnt,3), manning_nd(nnt))

    call cg_coord_read_f(fn, base, zone, "CoordinateX", 4, 1, nnt, coord, ierr)
    if (ierr .ne. CG_OK) call cg_error_exit_f

    do i=1,nnt
      coordonnees(i,1) = coord(i)
    end do

    call cg_coord_read_f(fn, base, zone, "CoordinateY", 4, 1, nnt, coord, ierr)
    if (ierr .ne. CG_OK) call cg_error_exit_f

    do i=1,nnt
      coordonnees(i,2) = coord(i)
    end do

    call cg_coord_read_f(fn, base, zone, "CoordinateZ", 4, 1, nnt, coord, ierr)
    if (ierr .ne. CG_OK) call cg_error_exit_f

    do i=1,nnt
      coordonnees(i,3) = coord(i)
    end do

    deallocate(coord)


    call cg_nsections_f(fn, base, zone, n_sections, ierr)
    if (ierr .ne. CG_OK) call cg_error_exit_f
    section = 1

    call cg_section_read_f(fn, base, zone, section, sectionname,  sectionType,&
      &eBeg, eEnd, nBdry, flag, ierr)
    if (ierr .ne. CG_OK) call cg_error_exit_f

    nelt = nelt
    allocate(conn(nelt*3), manning_nelt(nelt))
    allocate(connectivite(nelt,3))
    call cg_elements_read_f(fn, base, zone, section, conn, NULL, ierr)
    if (ierr .ne. CG_OK) call cg_error_exit_f
    do i=1,nelt
      connectivite(i,1) = conn(3*(i-1)+1)
      connectivite(i,2) = conn(3*(i-1)+2)
      connectivite(i,3) = conn(3*(i-1)+3)
    end do
    deallocate(conn)

    call cg_nbocos_f(fn, base, zone, n_bocos, ierr)
    if (ierr .ne. CG_OK) call cg_error_exit_f

    ndi = 0
    ndo = 0

    ! print*, n_bocos, "boundary conditions"

    !!Read a first time to see how many local input/output nodes there are
    do boco=1,n_bocos
      call cg_boco_info_f(fn, base, zone, boco, boconame, bocoType,&
        &ptsetType, n_bcnodes, &
        &normalIndex, normListFlag, normDataType, nDataset, ierr)
      if (ierr .ne. CG_OK) call cg_error_exit_f

      if(n_bcnodes>1) then
        if(bocoType == 9) then !!Inflow
          ndi = ndi + n_bcnodes
        else if(bocoType == 13) then !!Outflow
          ndo = ndo + n_bcnodes
        end if
      end if
    end do

    print*, "INPUT " , nombre_entrees, ndi
    if(nombre_entrees>0) then
      allocate(long_entree(nombre_entrees))
      allocate(surf_entree(nombre_entrees))
      if(ndi>0) then
        allocate(ndinput(ndi))
        allocate(numero_ndinput(ndi))
      end if
    end if

    print*, "OUTPUT " , nombre_sorties, ndo
    if(nombre_sorties>0 .and. ndo>0) then
      allocate(ndoutput(ndo))
      allocate(numero_ndoutput(ndo))
    end if

    ndi = 0
    ndo = 0

    do boco=1,n_bocos
      call cg_boco_info_f(fn, base, zone, boco, boconame, bocoType,&
        &ptsetType, n_bcnodes, &
        &normalIndex, normListFlag, normDataType, nDataset, ierr)
      if (ierr .ne. CG_OK) call cg_error_exit_f
      boconame = trim(adjustl(boconame))

      if(n_bcnodes>1) then

        if(bocoType == 9) then
          call cg_boco_read_f(fn, base, zone, boco, ndinput(ndi+1:), NULL, ierr)
          !!Find which input from the donnees.f file it corresponds to
          found = .FALSE.
          do it=1,nombre_entrees
            if(boconame == inlet_name(it)) then
              numero_ndinput(ndi+1:) = it
              found = .TRUE.
            end if
          end do
          if (.not. found) then
            print*,"Inlet name : ", trim(adjustl(boconame)), ", not found in donnees.f file ! Setting it to 1"
            numero_ndinput(ndi+1:) = 1
          end if
          ! print*,mpi_process_id," input nodes ",ndi,numero_ndinput(ndi+1),ndinput(ndi+1)
          if (ierr .ne. CG_OK) call cg_error_exit_f
          ndi = ndi + n_bcnodes
        else if(bocoType == 13) then
          call cg_boco_read_f(fn, base, zone, boco, ndoutput(ndo+1:), NULL, ierr)
          !!Find which output from the donnees.f file it corresponds to
          found = .FALSE.
          do it=1,nombre_sorties
            if(boconame == outlet_name(it)) then
              numero_ndoutput(ndo+1:) = it
              found = .TRUE.
            end if
          end do
          if (.not. found) then
            print*,"Outlet name : ", trim(adjustl(boconame)), ", not found in donnees.f file ! Setting it to 1"
            numero_ndoutput(ndo+1:) = 1
          end if
          print*,mpi_process_id," output nodes ",ndo,numero_ndoutput(ndo+1),ndoutput(ndo+1)
          if (ierr .ne. CG_OK) call cg_error_exit_f
          ndo = ndo + n_bcnodes
        end if
      end if
    end do

    call cg_goto_f(fn, base, ierr, zonename, 0, "end")
    if (ierr .ne. CG_OK) call cg_error_exit_f
    call cg_nuser_data_f(n_userdata, ierr)
    if (ierr .ne. CG_OK) call cg_error_exit_f
    do i=1,n_userdata
      call cg_user_data_read_f(i,userdataname,ierr)
      if (ierr .ne. CG_OK) call cg_error_exit_f
      !!Si elems à envoyer
      if(userdataname == "ElemsToSend") then
        call cg_goto_f(fn, base, ierr, zonename, 0, userdataname, 0, "end")
        if (ierr .ne. CG_OK) call cg_error_exit_f
        call cg_nuser_data_f(n_userdata2, ierr)
        if (ierr .ne. CG_OK) call cg_error_exit_f

        nelt_fant_envoi_bloc = n_userdata2
        allocate(elt_fant_envoi_list(nelt_fant_envoi_bloc))
        allocate(elt_fant_envoi_length(nelt_fant_envoi_bloc))

        !!Pour tous les voisins
        do j=1,n_userdata2

          call cg_user_data_read_f(j,userdataname2,ierr)
          if (ierr .ne. CG_OK) call cg_error_exit_f

          call cg_goto_f(fn, base, ierr, zonename, 0, userdataname, 0, userdataname2, 0, "end")
          if (ierr .ne. CG_OK) call cg_error_exit_f

          call cg_ptset_info_f(ptSetType, n_ptset, ierr)
          if (ierr .ne. CG_OK) call cg_error_exit_f

          call cg_ordinal_read_f(ord, ierr)
          if (ierr .ne. CG_OK) call cg_error_exit_f

          allocate(elt_fant_envoi_list(j)%elem_id(n_ptset))

          elt_fant_envoi_list(j)%length = n_ptset !! Longueur du message
          elt_fant_envoi_list(j)%proc = ord !! Destinataire

          call cg_ptset_read_f(elt_fant_envoi_list(j)%elem_id, ierr)
          if (ierr .ne. CG_OK) call cg_error_exit_f

          call cg_goto_f(fn, base, ierr, zonename, 0, userdataname, 0, "end")
          if (ierr .ne. CG_OK) call cg_error_exit_f

          nmax_ptset = max(nmax_ptset, n_ptset)

          print*, mpi_process_id, j, " ElemsToSend", elt_fant_envoi_list(j)%length, elt_fant_envoi_list(j)%proc, elt_fant_envoi_list(j)%elem_id(1)

        end do

        allocate(elt_fant_envoi_id(nmax_ptset,nelt_fant_envoi_bloc))
        allocate(elt_fant_envoi_sol(3,nmax_ptset,nelt_fant_envoi_bloc))

        elt_fant_envoi_id = -1
        do j=1,n_userdata2
          elt_fant_envoi_length(j) = elt_fant_envoi_list(j)%length
          ! ierr = cudamemcpy(elt_fant_envoi_id(1,j),elt_fant_envoi_list(j)%elem_id,elt_fant_envoi_list(j)%length)
          ! elt_fant_envoi_id(:elt_fant_envoi_list(j)%length,j) = elt_fant_envoi_list(j)%elem_id(:)
          elt_fant_envoi_id(:,j) = elt_fant_envoi_list(j)%elem_id(:)
        end do

        call cg_goto_f(fn, base, ierr, zonename, 0, "end")
        if (ierr .ne. CG_OK) call cg_error_exit_f

      else if(userdataname == "ElemsToRecv") then

        call cg_goto_f(fn, base, ierr, zonename, 0, userdataname, 0, "end")
        if (ierr .ne. CG_OK) call cg_error_exit_f
        call cg_nuser_data_f(n_userdata2, ierr)
        if (ierr .ne. CG_OK) call cg_error_exit_f

        nelt_fant_recep_bloc = n_userdata2
        nelt_fant_recep = 0
        allocate(elt_fant_recep_range(nelt_fant_recep_bloc))
        ! allocate(reqrecv(nelt_fant_recep_bloc), recvstat(MPI_STATUS_SIZE, nelt_fant_recep_bloc))

        !!Pour tous les voisins
        do j=1,n_userdata2

          call cg_user_data_read_f(j,userdataname2,ierr)
          if (ierr .ne. CG_OK) call cg_error_exit_f

          call cg_goto_f(fn, base, ierr, zonename, 0, userdataname, 0, userdataname2, 0, "end")
          if (ierr .ne. CG_OK) call cg_error_exit_f

          call cg_ptset_info_f(ptSetType, n_ptset, ierr)
          if (ierr .ne. CG_OK) call cg_error_exit_f

          call cg_ordinal_read_f(ord, ierr)
          if (ierr .ne. CG_OK) call cg_error_exit_f

          elt_fant_recep_range(j)%length = n_ptset !! Longueur du message
          elt_fant_recep_range(j)%proc = ord !! Destinataire

          call cg_ptset_read_f(elt_fant_recep_range(j)%start, ierr)
          if (ierr .ne. CG_OK) call cg_error_exit_f

          if((nelt - elt_fant_recep_range(j)%start + 1) > nelt_fant_recep) nelt_fant_recep = nelt-elt_fant_recep_range(j)%start + 1

          print*, mpi_process_id, j, " ElemsToRecv", elt_fant_recep_range(j)%length, elt_fant_recep_range(j)%proc, elt_fant_recep_range(j)%start

          call cg_goto_f(fn, base, ierr, zonename, 0, userdataname, 0, "end")
          if (ierr .ne. CG_OK) call cg_error_exit_f
        end do
        call cg_goto_f(fn, base, ierr, zonename, 0, "end")
        if (ierr .ne. CG_OK) call cg_error_exit_f

      else if(userdataname == "Manning") then
        if(is_override_manning==1) then
          print*,"MANNING DETECTED IN MESH FILE BUT WILL NOT BE USED"
        else
          print*,"MANNING DETECTED IN MESH FILE WILL BE USED"
          call cg_goto_f(fn, base, ierr, zonename, 0, userdataname, 0, "end")
          if (ierr .ne. CG_OK) call cg_error_exit_f
          call cg_array_read_f(1, manning_nelt, ierr)
          if (ierr .ne. CG_OK) call cg_error_exit_f
        end if
        call cg_goto_f(fn, base, ierr, zonename, 0, "end")
        if (ierr .ne. CG_OK) call cg_error_exit_f
      end if
    end do
    ! if(cg_goto(index_file, index_base, zss.str().c_str(), 0, "end")) cg_error_exit();

    ndwall = 0

    call cg_close_f(fn, ierr)
    if (ierr .ne. CG_OK) call cg_error_exit_f
  end subroutine lec_mail_cgns


  subroutine tab_bound_tri
    ! ======================================================================
    !
    !     tab_bound_tri = construction table des elements frontiers (cas triangulaire)
    !     construction table of boundary elements (triangular case)
    !
    !     auteur : youssef loukili  granit etsmtl
    !              
    !     version : 1.0  ;  may 12 2003
    ! ======================================================================

    use precision_kind
    use global_data
    use file_id

    implicit none

    integer :: ierr_loc, nity, i

    call set_boundary()
    call find_neighbour()
  end subroutine tab_bound_tri


  subroutine set_boundary()
    use precision_kind
    use global_data
    implicit none

    integer :: gi

    do gi=1,nelt
      if(ns==3) then
        boundary(gi,1,1) = gi
        boundary(gi,2,1) = gi
        boundary(gi,3,1) = gi
        boundary(gi,1,2) = gi
        boundary(gi,2,2) = gi
        boundary(gi,3,2) = gi
      else if(ns==4) then
        boundary(gi,1,1) = gi
        boundary(gi,2,1) = gi
        boundary(gi,3,1) = gi
        boundary(gi,4,1) = gi
        boundary(gi,1,2) = gi
        boundary(gi,2,2) = gi
        boundary(gi,3,2) = gi
        boundary(gi,4,2) = gi
      end if
    end do
  end subroutine set_boundary 

  subroutine find_neighbour()
    use precision_kind
    use global_data
    implicit none

    integer :: gi,gj,m1,m2,m3,n1,n2,n3,k

    do gi=1,nelt
      do gj=1,nelt !gj=gi+1,nelt ?
        if(gi /= gj) then

        n1 = connectivite(gi,1)
        n2 = connectivite(gi,2)
        n3 = connectivite(gi,3)

        m1 = connectivite(gj,1)
        m2 = connectivite(gj,2)
        m3 = connectivite(gj,3)

        if ( m1==n1 .and. m3==n2 .or. m3==n1 .and. m2==n2 .or. m2==n1 .and. m1==n2 ) &
          & boundary(gi,1,1) = gj

        if ( m1==n2 .and. m3==n3 .or. m3==n2 .and. m2==n3 .or. m2==n2 .and. m1==n3 ) &
          & boundary(gi,2,1) = gj

        if ( m1==n3 .and. m3==n1 .or. m3==n3 .and. m2==n1 .or. m2==n3 .and. m1==n1 ) &
          & boundary(gi,3,1) = gj
      end if
    end do
  end do

  end subroutine find_neighbour

  subroutine find_upwind_cells()
    use precision_kind
    use global_data
    implicit none

    integer :: gi, kv, c

    do gi=1,nelt
      do c=1,3
        if(boundary(gi,1+mod((c+1)-1,3),1)>0) then
          upwind_cells(1,c,gi) = boundary(gi,1+mod((c+1)-1,3),1)
        else
          upwind_cells(1,c,gi) = gi
        endif
        if(boundary(gi,1+mod((c+2)-1,3),1)>0) then
          upwind_cells(2,c,gi) = boundary(gi,1+mod((c+2)-1,3),1)
        else
          upwind_cells(2,c,gi) = gi
        endif
      end do
    end do
  end subroutine find_upwind_cells

  subroutine boundary_kind
    ! ======================================================================
    !     boundary_kind = affectation du genre des frontieres (assignment of the kind of borders)
    !                     ( entree : -1 ; sortie : -2 : imperm : -3 )
    ! ======================================================================

    use precision_kind
    use global_data
    implicit none

    integer                                     :: kvol, kfac, n1, n2, i, j, check1, ierr
    integer                                     :: check
    integer                                     :: ierr_loc, already_assigned

    do kvol=1,nelt
      do kfac=1,ns

        if ( boundary(kvol,kfac,1) == kvol ) then
          n1 = connectivite(kvol,kfac)

          if ( kfac < ns ) then
            n2 = connectivite(kvol,kfac+1)
          else
            n2 = connectivite(kvol,1)
          endif

          already_assigned=0

          if(ndi > 0) then 

            call assign_type_1(ndinput,boundary,kvol,kfac, ndi, n1, n2, -1)
            call assign_type_1_numero(ndinput, numero_ndinput, boundary,kvol,kfac, ndi, n1, n2, -1)

          endif 

          if(ndo > 0 .and. boundary(kvol,kfac,1)==kvol) then 

            call assign_type_1(ndoutput,boundary,kvol,kfac, ndo, n1, n2, -2)
            call assign_type_1_numero(ndoutput,numero_ndoutput,boundary,kvol,kfac, ndo, n1, n2, -2)

          endif

          if(ndw > 0 .and. boundary(kvol,kfac,1)==kvol) then 

            call assign_type_1(ndwall,boundary,kvol,kfac, ndw, n1, n2, -3)

          endif

          if(ndi > 0 .and. ndw > 0 .and. boundary(kvol,kfac,1)==kvol) then 
            !! Si la fois noeud de mur et noeud input alors mur
            call assign_type_2(ndwall, ndinput,boundary,kvol,kfac,ndw, ndi, n1, n2, -3)
          endif

          if(ndo > 0 .and. ndw > 0 .and. boundary(kvol,kfac,1)==kvol) then 
            !! Si la fois noeud de mur et noeud sortie alors mur
            call assign_type_2(ndwall, ndoutput,boundary,kvol,kfac,ndw, ndo, n1, n2, -3)
          endif

        endif
      enddo
    enddo
  end subroutine boundary_kind

  subroutine check_assign(check, kvol, kfac)
    use precision_kind
    use global_data
    implicit none
    integer, intent(out) :: check
    integer, value :: kvol, kfac

    check = boundary(kvol, kfac, 1)
  end subroutine check_assign


  subroutine assign_type_1_numero(ndiow, numero_ndinput, boundary, kvol, kfac, nd, n1, n2, id)
    implicit none
    integer, intent(inout) :: boundary(:,:,:)
    integer, value :: kvol, kfac, nd, n1, n2, id
    integer, intent(in) :: ndiow(:), numero_ndinput(:)

    integer :: gi,gj,ndiow_i, ndiow_j

    do gi=1,nd-1
      do gj=gi+1,nd

        ndiow_i = ndiow(gi)
        ndiow_j = ndiow(gj)

        if ( n1 == ndiow_i .and. n2 == ndiow_j .or. n1 == ndiow_j .and. n2 == ndiow_i ) then  
          boundary(kvol,kfac,2) = numero_ndinput(gi)
        end if

      end do
    end do
  end subroutine assign_type_1_numero


  subroutine assign_type_1(ndiow, boundary, kvol, kfac, nd, n1, n2, id)
    implicit none
    integer, intent(inout) :: boundary(:,:,:)
    integer, value :: kvol, kfac, nd, n1, n2, id
    integer, intent(in) :: ndiow(:)

    integer :: gi,gj,ndiow_i, ndiow_j

    do gi=1,nd-1
      do gj=gi+1,nd

        ndiow_i = ndiow(gi)
        ndiow_j = ndiow(gj)

        if ( n1 == ndiow_i .and. n2 == ndiow_j .or. n1 == ndiow_j .and. n2 == ndiow_i ) then  
          boundary(kvol,kfac,1) = id
        end if

      end do
    end do
  end subroutine assign_type_1

  subroutine assign_type_2_numero_entree(ndwall, ndio, numero_ndinput, boundary, kvol, kfac, ndw, nd, n1, n2, id)
    implicit none
    integer, intent(inout) :: boundary(:,:,:)
    integer, value :: kvol, kfac, ndw, nd, n1, n2, id
    integer, intent(in) :: ndio(:), ndwall(:), numero_ndinput(:)

    integer :: gi,gj, ndiow_j, ndwall_i

    do gi=1,ndw
      do gj=1,nd

        ndiow_j  = ndio(gj)
        ndwall_i = ndwall(gi)

        if ( n1 == ndwall_i .and. n2 == ndiow_j .or. n1 == ndiow_j .and. n2 == ndwall_i ) then  
          boundary(kvol,kfac,2) = numero_ndinput(gj)
        end if

      end do
    end do
  end subroutine assign_type_2_numero_entree

  subroutine assign_type_2(ndwall, ndio, boundary, kvol, kfac, ndw, nd, n1, n2, id)
    implicit none
    integer, intent(inout) :: boundary(:,:,:)
    integer, value :: kvol, kfac, ndw, nd, n1, n2, id
    integer, intent(in) :: ndio(:), ndwall(:)

    integer :: gi,gj, ndiow_j, ndwall_i

    do gi=1,ndw
      do gj=1,nd

        ndiow_j  = ndio(gj)
        ndwall_i = ndwall(gi)

        if ( n1 == ndwall_i .and. n2 == ndiow_j .or. n1 == ndiow_j .and. n2 == ndwall_i ) then  
          boundary(kvol,kfac,1) = id
        end if

      end do
    end do
  end subroutine assign_type_2

  subroutine assign_dry_as_wall
    !! Si maille voisine est sche alors on le met en wall
    use precision_kind
    use global_data

    implicit none

    integer :: gi,kfac, kvoisin

    do gi=1,nelt
      do kfac=1,ns
        kvoisin = boundary(gi,kfac,1)
        if (kvoisin>0)then
          if(vdlg(1,kvoisin)<tolisec) then
            boundary(gi,kfac,1) = -3
          end if
        end if
      end do
    end do
  end subroutine assign_dry_as_wall

  subroutine construct_coo_table_elemwise

    ! caluclates the elementwise coordinate table and the x and y coordinate of the centroid of each element
    use precision_kind
    use global_data
    implicit none

    integer :: kvol, i, s1, s2, s3,s4, ierr1

    do kvol = 1,nelt    
      s1 = connectivite(kvol,1)
      s2 = connectivite(kvol,2)
      s3 = connectivite(kvol,3)
      coo_table_elemwise(kvol,1:3) = coordonnees(s1,1:3)
      coo_table_elemwise(kvol,4:6) = coordonnees(s2,1:3)
      coo_table_elemwise(kvol,7:9) = coordonnees(s3,1:3)
      x_centroid(kvol) = (coordonnees(s1,1) + coordonnees(s2,1) + coordonnees(s3,1))/3
      y_centroid(kvol) = (coordonnees(s1,2) + coordonnees(s2,2) + coordonnees(s3,2))/3

    end do

    coo_table_elemwise = coo_table_elemwise
    x_centroid = x_centroid
    y_centroid = y_centroid
  end subroutine construct_coo_table_elemwise

  subroutine compute_surface_and_bathy
    use precision_kind
    use global_data
    implicit none

    real(fp_kind) :: x1, x2 ,x3, x4, y1, y2, y3, y4, z1, z2, z3, z4, gz1, gz2
    real(fp_kind) :: xg, yg, zg, d1, d2, d3, d4, a, b, c, s, dx, surf_area

    integer :: gi

    do gi=1,nelt
      x1 = coo_table_elemwise(gi,1)
      y1 = coo_table_elemwise(gi,2)

      x2 = coo_table_elemwise(gi,4)
      y2 = coo_table_elemwise(gi,5)

      x3 = coo_table_elemwise(gi,7)
      y3 = coo_table_elemwise(gi,8)

      a = sqrt((x2-x1)**2+(y2-y1)**2)
      b = sqrt((x3-x2)**2+(y3-y2)**2)
      c = sqrt((x1-x3)**2+(y1-y3)**2)
      s = (a+b+c)/2.

      surf(gi) = sqrt(s*(s-a)*(s-b)*(s-c))
      if(surf(gi) < 1e-8) then
        print*,"erreur calcul surface elem",gi
      endif

      z1 = coo_table_elemwise(gi,3)
      z2 = coo_table_elemwise(gi,6)
      z3 = coo_table_elemwise(gi,9)

      xg = ( x1 + x2 + x3 ) / 3.00
      yg = ( y1 + y2 + y3 ) / 3.00
      d1 = sqrt( (x1-xg)**2 + (y1-yg)**2 )
      d2 = sqrt( (x2-xg)**2 + (y2-yg)**2 )
      d3 = sqrt( (x3-xg)**2 + (y3-yg)**2 )

      zm(gi) = ( d1*z1 + d2*z2 + d3*z3 ) / ( d1 + d2 + d3 ) ! planimetrique
    end do
  end subroutine compute_surface_and_bathy

  subroutine compute_input_elems
    use precision_kind
    use global_data
    implicit none

    integer :: i, kvol, j, k

    nelt_input = 0
    do kvol=1,nelt-nelt_fant_recep
      do j=1,ns
        if(boundary(kvol, j,1) == -1) then !!entree
          nelt_input = nelt_input+1
        end if
      end do
    end do

    allocate(elem_input(nelt_input))

    k=1
    do kvol=1,nelt-nelt_fant_recep
      do j=1,ns
        if(boundary(kvol, j,1) == -1) then !!entree
          elem_input(k) = kvol
          k = k+1
        end if
      end do
    end do

  end subroutine compute_input_elems

  subroutine calcul_surf_entree
    use precision_kind
    use global_data
    implicit none

    integer :: gi
    real(fp_kind), dimension(nombre_entrees) :: s

    integer k, kvol, kfac, n1, n2, id_entree, ientree, i, ierr
    integer :: id

    do id=1,nombre_entrees
      s(id) = 0
    end do

    do gi=1,nelt_input
      kvol = elem_input(gi)
      do kfac=1,ns
        n1 = connectivite(kvol,kfac)
        if ( kfac < ns ) then
          n2 = connectivite(kvol,kfac+1)
        else
          n2 = connectivite(kvol,1)
        endif

        if(boundary(kvol,kfac,1) == -1) then
          id_entree = boundary(kvol,kfac,2)
          s(id) = s(id) + sqrt((coordonnees(n1,1)-coordonnees(n2,1))**2+(coordonnees(n1,2)-coordonnees(n2,2))**2)*vdlg(1,kvol)
        end if
      end do
    end do
  end subroutine calcul_surf_entree

  subroutine calcul_long_entree
    use precision_kind
    use global_data
    use file_id
    implicit none

    integer kvol, kfac, n1, n2, id_entree, ientree

    do ientree=1,nombre_entrees
      long_entree(ientree) = 0.
    end do

    do kvol=1,nelt-nelt_fant_recep
      do kfac=1,ns
        n1 = connectivite(kvol,kfac)
        if ( kfac < ns ) then
          n2 = connectivite(kvol,kfac+1)
        else
          n2 = connectivite(kvol,1)
        endif

        if(boundary(kvol,kfac,1) == -1) then
          id_entree = boundary(kvol,kfac,2)
          long_entree(id_entree) = long_entree(id_entree) + sqrt((coordonnees(n1,1)-coordonnees(n2,1))**2+(coordonnees(n1,2)-coordonnees(n2,2))**2)
        end if
      end do
    end do
  end subroutine calcul_long_entree

  subroutine set_undefined_boudaries_as_walls()
    !! Si le voisin n'as pas changé depuis l'initialisation alors on met un mur
    use precision_kind
    use global_data
    implicit none

    integer :: gi,ki

    do gi=1,nelt
      do ki=1,ns
        if(boundary(gi,ki,1) == gi) then
          boundary(gi,ki,1) = -3 !wall
        end if
      end do
    end do
  end subroutine set_undefined_boudaries_as_walls

  subroutine centroid(kvol,xg,yg)
    ! ======================================================================
    !     centroid = calcul les coordonnees du centre de gravite de kvol
    !     auteur : Youssef Loukili  GRANIT ETSMTL
    !     version : 1.0  ;  May 12 2003
    ! ======================================================================

    use precision_kind
    use global_data
    use file_id

    implicit none

    integer , intent(in)         :: kvol
    real(fp_kind), intent(inout) :: xg, yg

    integer :: i, nc

    xg = 0.
    yg = 0.

    do i=1,ns
      nc = connectivite(kvol,i)
      xg = xg + coordonnees(nc,1)
      yg = yg + coordonnees(nc,2)
    enddo

    xg = xg / ns
    yg = yg / ns
  end subroutine centroid

  subroutine construct_vertex_neighbors
    use precision_kind
    use global_data

    implicit none

    integer :: i, j, k, s

    do i=1,nelt
      do j=1,3
        s = connectivite(i,j)
        do k=1,6
          if(vertex_neighbors(k,s) == -1) then
            vertex_neighbors(k,s) = i
            exit
          end if
        end do
      end do
    end do
  end subroutine construct_vertex_neighbors
end module maillage
