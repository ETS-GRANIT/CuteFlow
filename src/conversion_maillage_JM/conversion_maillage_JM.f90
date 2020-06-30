program Maillage_Bluekenue_CuteFlow
  !
  ! ======================================================================
  !
  !	Maillage_CuteFlow : programme de generation du maillage pour CuteFlow à partir de BlueKenue
  !
  ! ======================================================================
  !
  !     Auteur : ean-Marie Zokagoa GRANIT ETSMTL
  !
  !     version : Septembre 2018
  !
  ! =============================================================================================================
  !
  !	use global
  !	use m_param
  !
  implicit none
  !
  ! *** bloc interface ***
  !
  !	interface
  !	
  !	end interface
  !
  !	------------------               Declaration des variables         ----------------------
  !
  ! ***************** Variables du Fichier Parametres *******************************
  !
  character(len=60) :: nom_test, fich_xyz,fich_xyman, fich_bc
  namelist/dataf/ nom_test, fich_xyz, fich_xyman, fich_bc
  !
  character (30)   :: genrtest
  character (200) :: fich_vu_tecplot_mail, fich_min_z, fich_param_mail, nom_maillage
  character (16)  :: n_elts, n_noeuds
  !
  integer, parameter :: r_bkn_xyz		    = 10	! fichier lecture
  integer, parameter :: r_bkn_xyman       = 11	! fichier lecture
  integer, parameter :: r_bkn_fond        = 12	! fichier lecture
  integer, parameter :: w_mail_CF  	    = 13	! fichier ecriture
  integer, parameter :: w_mail_tecplot  	= 14	! fichier ecriture
  integer, parameter :: w_min_z           = 15	! fichier ecriture 
  integer, parameter :: w_nelt_nnt        = 16	! fichier ecriture 
  integer, parameter :: r_para			= 17 

  integer :: Nb_line
  integer :: ierr ! successful (if == 0) ou failed (si /= 0) 	
  integer								   :: nelt, nnt
  integer, dimension(:),     allocatable :: ndinput, ndoutput, ndwall
  integer, dimension(:,:),   allocatable :: connectivites
  real(8), dimension(:,:),   allocatable :: matvar1, matvar2, coordonnees, manning_nd

  integer :: ni, no, nw, ki, ko, kw, ndi, ndo, ndw 
  real(8) :: man1, man2, man3, minz
  !
  !
  character (2)  :: A8, A20, A34, A36, A38, A41, A52, A55, A57, A61		
  character (5)  :: A3, A7, A13, A14, A16, A17, A24, A28, A29, A30, A31, A33
  character (7)  :: A5, A6, A19, A32, A42, A56
  character (10) :: A2, A4, A23, A39, A53
  character (12) :: A9, A11, A12, A15, A18, A22, A25, A26, A58, A62
  character (16) :: A10, A21, A27, A43, A44, A45, A46, A47, A48, A49, A50, A59, A60
  character (20) :: A37, A40, A51, A54
  character (50) :: A1, A35		
  !
  integer :: i, j
  !
  character (2)  :: c1, c4
  character (7)  :: c5
  character (12) :: c2, c3
  integer        :: n1, n2, n3, n4, n5, n6
  real(8)        :: f1, f2, f3, f4, f5, f6, f7
  !
  !	-----------------------------------------------------------------------------------------
  print*, 'DEBUT DU TRAITEMENT'
  !	******************************************************************************************
  !
  !
  ! Lecture des données dans le fichier ''Parametres'' ============================================================
  open(r_para,file='Parametres.txt',form='formatted',status='old')
  read(r_para,nml=dataf)
  close(r_para)
  ! =============================================================================================================
  !

  minz = 10000.0
  !
  open(unit=r_bkn_xyz,file=fich_xyz,form='formatted',status='old')
  read(r_bkn_xyz,*) A1
  read(r_bkn_xyz,*) A2, A3, A4, A5, A6
  read(r_bkn_xyz,*) A7, A9, A10, A11, A12, A13, A14
  read(r_bkn_xyz,*) A8, A15, A16, A17, A18, A19  
  read(r_bkn_xyz,*) A20
  read(r_bkn_xyz,*) A21, A22
  read(r_bkn_xyz,*) A23, A24
  read(r_bkn_xyz,*) A25, A26
  read(r_bkn_xyz,*) A27, A28, A29, A30, A31, A32, A33
  read(r_bkn_xyz,*) A34
  read(r_bkn_xyz,*) A35
  read(r_bkn_xyz,*) A36
  read(r_bkn_xyz,*) A37, A38, A39
  read(r_bkn_xyz,*) A40 , A41, A42, A43, A44, A45, A46 , A47, A48, A49, A50
  read(r_bkn_xyz,*) A51, A52, A53
  read(r_bkn_xyz,*) A54, A55, A56
  read(r_bkn_xyz,*) A57
  read(r_bkn_xyz,*) A58, nnt
  read(r_bkn_xyz,*) A59, nelt
  read(r_bkn_xyz,*) A60, A54
  read(r_bkn_xyz,*) A61
  read(r_bkn_xyz,*) A62
  !
  allocate( coordonnees(nnt,3), connectivites(nelt,3) )
  !
  do i=1,nnt
    read(r_bkn_xyz,*) coordonnees(i,1), coordonnees(i,2), n1, n2, coordonnees(i,3)
    minz = min(minz, coordonnees(i,3))
  enddo 
  !
  do j=1,nelt
    read(r_bkn_xyz,*) connectivites(j,1), connectivites(j,2), connectivites(j,3)
  enddo 
  close(r_bkn_xyz)
  !
  write(n_elts,'(i10)') nelt
  write(n_noeuds,'(i10)') nnt

  !	Ajustement de la bathymetrie lorsque le niveau minimal est négatif
  fich_min_z = nom_test // n_elts // 'elts  MIN_Z.dat'
  if (minz < 0.0001) then
    do i=1,nnt
      coordonnees(i,3) = coordonnees(i,3) + abs(minz)
    enddo
    open(unit=w_min_z,file=fich_min_z,status="unknown")
    write(w_min_z,*) minz
    close(w_min_z)
  endif

  !============================================================================================
  !
  open(unit=r_bkn_xyman,file=fich_xyman,form='formatted',status='old')
  read(r_bkn_xyman,*) A1
  read(r_bkn_xyman,*) A2, A3, A4, A5, A6
  read(r_bkn_xyman,*) A7, A9, A10, A11, A12, A13, A14
  read(r_bkn_xyman,*) A8, A15, A16, A17, A18, A19  
  read(r_bkn_xyman,*) A20
  read(r_bkn_xyman,*) A21, A22
  read(r_bkn_xyman,*) A23, A24
  read(r_bkn_xyman,*) A25, A26
  read(r_bkn_xyman,*) A27, A28, A29, A30, A31, A32, A33
  read(r_bkn_xyman,*) A34
  read(r_bkn_xyman,*) A35
  read(r_bkn_xyman,*) A36
  read(r_bkn_xyman,*) A37, A38, A39
  read(r_bkn_xyman,*) A40 , A41, A42, A43, A44, A45, A46 , A47, A48, A49, A50
  read(r_bkn_xyman,*) A51, A52, A53
  read(r_bkn_xyman,*) A54, A55, A56
  read(r_bkn_xyman,*) A57
  read(r_bkn_xyman,*) A58, nnt
  read(r_bkn_xyman,*) A59, nelt
  read(r_bkn_xyman,*) A60, A54
  read(r_bkn_xyman,*) A61
  read(r_bkn_xyman,*) A62
  !
  allocate( manning_nd(nnt,3) )
  !
  do i=1,nnt
    read(r_bkn_xyman,*) manning_nd(i,1), manning_nd(i,2), n1, n2, manning_nd(i,3)
  enddo 
  !
  close(r_bkn_xyman)
  !
  !============================================================================================
  !	Determination du nombre de lignes du fichier FOND_BC
  open(unit=r_bkn_fond,file=fich_bc,form='formatted',iostat=ierr,status='old')
  Nb_line = 0
  do while (ierr==0)
    read(r_bkn_fond,*,iostat=ierr)
    if (ierr==0) Nb_line = Nb_line + 1
  enddo
  close(r_bkn_fond)
  !
  !	Determination du nombre de noeuds à l'entree, à la sortie et aux murs
  open(unit=r_bkn_fond,file=fich_bc,form='formatted',status='old')
  !
  ki=0
  ko=0
  kw=0
  !
  do i=1,Nb_line
    read(r_bkn_fond,*) n1, n2, n3 !, f1, f2, f3, f4, n4, f5, f6, f7, n5 !, n6, c1, c2 , c3, c4, c5		
    if (n1==4 .and. n2==5 .and. n3==5) ki = ki+1
    if (n1==2 .and. n2==2 .and. n3==2) kw = kw+1
    if (n1==5 .and. n2==4 .and. n3==4) ko = ko+1
  enddo
  !
  close(r_bkn_fond)
  !
  ndi=ki
  ndo=ko
  ndw=kw
  allocate(ndinput(ndi), ndoutput(ndo), ndwall(ndw) )
  !
  !	Stockage des noeuds  à l'entree, à la sortie et aux murs
  open(unit=r_bkn_fond,file=fich_bc,form='formatted',status='old')
  !
  ki=0
  ko=0
  kw=0
  !
  do i=1,Nb_line
    read(r_bkn_fond,*) n1, n2, n3, f1, f2, f3, f4, n4, f5, f6, f7, n5 !, n6, c1, c2 , c3, c4, c5		
    if (n1==4 .and. n2==5 .and. n3==5) then
      ki = ki+1
      ndinput(ki) = n5
    endif
    if (n1==2 .and. n2==2 .and. n3==2) then
      kw = kw+1
      ndwall(kw) = n5
    endif
    if (n1==5 .and. n2==4 .and. n3==4) then
      ko = ko+1
      ndoutput(ko) = n5
    endif
  enddo
  !
  close(r_bkn_fond)
  !
  !	****************************************************************************************************************************
  !					             GENERATION DU FICHIER DE MAILLAGE POUR CUTEFLOW
  !	****************************************************************************************************************************
  !
  nom_maillage = nom_test // n_elts // 'elts  Maillage.txt'
  open(unit=w_mail_CF,file=nom_maillage,status="unknown")
  write(w_mail_CF,*) 'Table de coordonnees'
  write(w_mail_CF,*) nnt
  do i=1,nnt
    write(w_mail_CF,'(i,4f16.4)') i, coordonnees(i,1), coordonnees(i,2), coordonnees(i,3) , manning_nd(i,3)
  enddo
  write(w_mail_CF,*) 'Table de connectivités'
  write(w_mail_CF,*) nelt
  do i=1,nelt
    man1 = manning_nd(connectivites(i,1),3)
    man2 = manning_nd(connectivites(i,2),3)
    man3 = manning_nd(connectivites(i,3),3)
    write(w_mail_CF,'(4i,f16.4)') i, connectivites(i,1),connectivites(i,2),connectivites(i,3), ( man1 + man2 + man3)/3
  enddo
  !
  write(w_mail_CF,*) 'Noeuds a l entree'
  write(w_mail_CF,*) ndi
  do i=1,ndi
    write(w_mail_CF,*) ndinput(i)
  enddo
  !
  write(w_mail_CF,*) 'Noeuds a la sortie'
  write(w_mail_CF,*) ndo
  do i=1,ndo
    write(w_mail_CF,*) ndoutput(i)
  enddo
  write(w_mail_CF,*) 'Noeuds au murs'
  write(w_mail_CF,*) ndw
  do i=1,ndw
    write(w_mail_CF,*) ndwall(i)
  enddo
  !
  close(w_mail_CF)
  !
  !	****************************************************************************************************************************
  !	****************************************************************************************************************************
  !					             GENERATION DU FICHIER DE MAILLAGE POUR TECPLOT
  !	****************************************************************************************************************************
  fich_vu_tecplot_mail = nom_test // n_elts // 'elts  Vu_Tecplot.dat'
  open(unit=w_mail_tecplot,file=fich_vu_tecplot_mail,status="unknown")
  !
  write(w_mail_tecplot,*) 'title = "hauteur"'
  write(w_mail_tecplot,*) 'variables = "x","y","z","manning"'
  write(w_mail_tecplot,*) 'zone t="pas:',0.0, '" , f=fepoint, n=',nnt, ',e=',nelt, ',et=triangle'
  do i=1, nnt
    write(w_mail_tecplot,'(8f16.4)') coordonnees(i,1),coordonnees(i,2),coordonnees(i,3),manning_nd(i,3)
  enddo
  write(w_mail_tecplot,*)' '
  write(w_mail_tecplot,*)' '
  !
  do i=1, nelt
    write(w_mail_tecplot,*) connectivites(i,1) , connectivites(i,2) , connectivites(i,3)
  enddo
  !
  close(w_mail_tecplot)
  !
  !	****************************************************************************************************************************
  !
  fich_param_mail = nom_test // n_elts // 'elts Parametres Maillage.dat'
  open(unit=w_nelt_nnt,file=fich_param_mail,status="unknown")
  !
  write(w_nelt_nnt,*) 'nbre total d elements =', nelt
  write(w_nelt_nnt,*) 'nbre total de noeuds =', nnt
  write(w_nelt_nnt,*) 'nbre total de noeuds aus frontieres =', Nb_line
  write(w_nelt_nnt,*) 'nbre total de noeuds a l entree =', ndi+2
  write(w_nelt_nnt,*) 'nbre total de noeuds a la sortie =', ndo
  write(w_nelt_nnt,*) 'nbre total de noeuds murs =', ndw
  !
  close(w_nelt_nnt)
  !
  !	****************************************************************************************************************************

  print*, 'FICHIER MAILLAGE GENERE'
  !*****************************************************************************

  !
end program
