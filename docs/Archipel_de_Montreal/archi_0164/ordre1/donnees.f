  &DONNEES_NAMELIST 

  ! Données du terrain
  GP=9.81,
  is_override_manning=1, override_manning=0.02000,               ! nombre de manning qui override les autes si is_...=1

  ! Données du maillage
  is_cgns=1,
  meshfile_path='',
  meshfile='rdp_4_2.cgns',                    ! Fichier de maillage
  elt_bound=0,                                                   ! 1 si le fichier boundary_table existe déja
  nombre_entrees=7, nombre_sorties=1,                                ! 0 si fichier non formaté pour plusieurs entrées/sorties

  ! Initialisation avec un Barrage
  eqlin_barrage=0,                                               ! 1 pour initialiser avec un barrage
  x1eqbar=5.0001, x2eqbar=5.,         ! Abscisses des deux points du barrage
  y1eqbar=0., y2eqbar=1.,          ! Ordonnées des deux points du barrage
  H_AMONT=1.0, U_AMONT=0, V_AMONT=0,                            ! Valeur de l'init du coté - de la normale au barrage
  H_AVAL=1e-5, U_AVAL =0, V_AVAL =0,                            ! Valeur de l'init du coté + de la normale au barrage

  ! Initialisation avec un plan 
  plan=1                                                         ! 1 pour initialiser avec un plan
  xplan=-0.0000, yplan=-0.0000, zplan=1                          ! Vecteur normal au plan
  xpoint=274956.783790834, ypoint=5043746.46205659, zpoint=7.05 ! Point appartenant au plan

  ! Initialisation a partir d'un fichier, fichier avec solution ax elements
  solinit=0,                                                     ! 1 pour initialiser a partir du fichier
  fich_sol_init='solstab1.txt',      ! nom du fichier d'initialisation

  ! Conditions aux limites
  inlet='inflow',                                                ! Type d'entrée : {inflow,transm}
  inlet_name="Inflow nodes 0", "Inflow nodes 1", "Inflow nodes 2", "Inflow nodes 3","Inflow nodes 4", "Inflow nodes 5", "Inflow nodes 6"
  ! debitglob=0.,0.,0.,0.,0.,0.,0.                             ! Débit aux entrées, séparer les débits pas des ,
  ! debitglob=125.,280.,166.,1262.,62.,123.,43.                             ! Débit aux entrées, séparer les débits pas des ,
  ! debitglob=1250.,2800.,1660.,12620.,62.,1230.,43.                             ! Débit aux entrées, séparer les débits pas des ,
  debitglob=200.,760.,2293.,11800.,16.,1.,10.                             ! Débit aux entrées, séparer les débits pas des ,
  outlet_name="Outflow nodes"
  H_sortie=7.05,                                                 ! Hauteur du niveau à la sortie du domaine

  ! Paramètres des schémas numériques
  IFLUX=2, ilimiteur=1, iupwind=1,                               ! 1 -> HLLC , 2 -> WAF Riadh, 3 -> WAF Loukili
  tolisec=1.0E-05,                                               ! Tolérence sec/mouillé
  friction=1,                                                    ! 1 pour prendre en compte la friction
  fricimplic=1,                                                  ! 0 -> explicite, 1 -> I-dt/2*J, 2 -> I-dt*B

  TS=40000., CFL=0.9,                                             ! Temps maximal de simultion, nombre CFL
  is_dt_constant=0, constant_dt=1e-4,
  tol_reg_perm=1.0E-15,                                          ! Tolérence relative entre debit entrée et débit sortie
  freqaffich=10000,                                               ! Frequence de print dans outfile.[0-9]

  sol_z_offset=0.
  solrestart=1,                            ! Sauvegarde en overwrite la solution pour restart
  solvisu=3, visu_snapshots=100,                         ! 1 all, 2 vtk, 3 cgns, 4 simple nodes, 5 simple elems
  sortie_finale_bluekenue=0/                                     ! Sauvergarde fichiers T3S à la fin
