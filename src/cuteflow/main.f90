program main
  ! ======================================================================
  !   CUTEFLOW : programme de simulation par volumes finis / solveurs de Riemann
  !               des ecoulements de canalisations, bris de barrages et innondations.
  !               Equations de base : Shallow Water Equations
  ! ======================================================================
  !     Auteur : Azzeddine Soulaimani & Youssef Loukili  & Jean-Marie Zokagoa & 
  !              Vincent Delmas GRANIT ETSMTL
  !     version : 4.0  ;  December 2019
  ! ======================================================================

  use mpi

  use precision_kind
  use global_data
  use file_id
  use pre_post_traitement
  use finite_volumes_method
  use initial_condition
  use maillage

  implicit none

  integer         :: i, ii, j, p, kk, ss, ie, iel, n_MC, i_MC, af_input
  integer         :: nsbloc, nreste, tot
  character(8)    :: ii_str
  integer         :: status, l, ie_coupe_transv, ierr, dev
  integer                     :: stat(MPI_STATUS_SIZE)

  call MPI_INIT(mpi_ierr)
  call MPI_COMM_SIZE (MPI_COMM_WORLD, num_mpi_process, mpi_ierr)
  call MPI_COMM_RANK (MPI_COMM_WORLD, mpi_process_id, mpi_ierr)
  write(mpi_process_id_string, "(I3)") mpi_process_id

  !! Lecture du fichier "donnees.f"
  call lec_donnees

  if(inlet == 'transm') then
    inlet_id = 1
  else if(inlet == 'inflow') then
    inlet_id = 2
  else if(inlet == 'phys_inflow') then
    inlet_id = 3
  endif

  allocate(dt,dt_max)
  dt_max=1000000.0D0

  !! Initization of the iteration counters
  cptsolvisu       = 0
  tc_init          = 0

  !! Ouverture/creation des fichiers (opening / creating files)
  call ouv_fichiers

  !! Construction / lecture  et  stockage du maillage 
  call lecture_maillage

  call set_undefined_boudaries_as_walls

  !!Allocation des requettes send recv pour les echanges mpi non bloquant
  if(is_cgns==1) then
    allocate(reqsend(nelt_fant_envoi_bloc), sendstat(MPI_STATUS_SIZE,nelt_fant_envoi_bloc))
    allocate(reqrecv(nelt_fant_recep_bloc), recvstat(MPI_STATUS_SIZE,nelt_fant_recep_bloc))
  else
    tot=0
    do j=1,nelt_fant_envoi_bloc
      nsbloc = ndln*elt_fant_envoi_bloc(j,2)/768 + 1
      nreste = ndln*elt_fant_envoi_bloc(j,2)-(nsbloc-1)*768
      tot=tot+nsbloc
    end do
    allocate(reqsend(tot), sendstat(MPI_STATUS_SIZE,tot))

    tot=0
    do j=1,nelt_fant_recep_bloc
      nsbloc = ndln*elt_fant_recep_bloc(j,2)/768 + 1
      nreste = ndln*elt_fant_recep_bloc(j,2)-(nsbloc-1)*768
      tot=tot+nsbloc
    end do
    allocate(reqrecv(tot), recvstat(MPI_STATUS_SIZE,tot))
  end if

  !! Allocation des matrices et vecteurs
  allocate( surf(nelt), zm(nelt) )

  allocate( vdlg(ndln,nelt), vdlg0(ndln,nelt))
  allocate( vdlg_visu(nelt,8))
  allocate( vdlg_visu_nodes(nnt,8))

  allocate( vdlg_nodes(ndln,nnt))

  if(solvisu>0) then
    if(visu_snapshots==0) visu_snapshots=1
    allocate(tsolvisu(0:visu_snapshots), solname_visu(0:visu_snapshots))

    do ii = 0,visu_snapshots
      write(ii_str,"(I8)") ii
      solname_visu(ii) = "Solution_"//trim(adjustl(trim(ii_str)))
      tsolvisu(ii) = ii*ts/real(visu_snapshots)
    enddo
  endif

  call compute_surface_and_bathy

  write(*,*) '======================================================================'
  write(*,*) ' Parameters'
  write(*,*) '======================================================================'
  write(*,*) 'meshfile            = ', meshfile 
  write(*,*) 'cotemin             = ', cotemin
  write(*,*) 'elt_bound           = ', elt_bound
  write(*,*) 'H_AMONT             = ', H_AMONT 
  write(*,*) 'U_AMONT             = ', U_AMONT
  write(*,*) 'V_AMONT             = ', V_AMONT
  write(*,*) 'H_AVAL              = ', H_AVAL
  write(*,*) 'U_AVAL              = ', U_AVAL
  write(*,*) 'V_AVAL              = ', V_AVAL 
  write(*,*) 'iflux               =', iflux
  write(*,*) 'Fricimplic          =', fricimplic
  write(*,*) 'visu_snapshots     =', visu_snapshots
  write(*,*) 'tol_reg_perm        =', tol_reg_perm
  write(*,*) 'tol                 =', tol
  write(*,*) 'tolisec             =', tolisec
  write(*,*) 'tolaffiche          =', tolaffiche
  if(mpi_process_id==0) write(*,*) "nombre d'elements totaux = ",nelt
  write(*,*) 'freqaffich          =', freqaffich
  write(*,*) 'nombre entree       =', nombre_entrees
  do af_input=1,nombre_entrees
    write(*,*) 'debitglob       ', af_input, " =", debitglob(af_input)
    write(*,*) 'longueur entree ', af_input, " =", long_entree(af_input)
  end do
  write(*,*) 'nombre sortie       =', nombre_sorties
  do af_input=1,nombre_sorties
    write(*,*) 'H_SORTIE        ', af_input, " =", h_sortie(af_input)
  end do
  write(*,*) '======================================================================'
  write(*,*) '======================================================================'

  !! Application des conditions initiales
  if (solinit==1) then
    call read_initial_condition
  else
    call set_initial_condition
  endif               

  vdlg = vdlg0

  !!Save Initial condition
  call global_save_sol_visu

  !! Initialisation du temps et calcul de dt via la CFL
  tc = 0.00

  !! Résolution par méthode volumes finis
  call solve_finite_volumes

  !!Save Final solution
  call global_save_sol_visu

  print*, '==========================================================================='
  print*, '=========================   FIN DE LA SIMULATION   ========================'
  print*, '==========================================================================='

  call MPI_FINALIZE(mpi_ierr)
end program main
