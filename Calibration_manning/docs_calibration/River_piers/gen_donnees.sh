echo "&DONNEES_NAMELIST 

  ! Données du terrain
  GP=9.81,
  is_override_manning=0, override_manning=0.0220,               ! nombre de manning qui override les autes si is_...=1

  ! Données du maillage
  is_cgns=1,
  meshfile_path='./',
 ! meshfile='manning_DivCanal_mesh_416_1_Case_$4.cgns',                    ! Fichier de maillage
  meshfile='manning_River_wth_piers_Case_$4.cgns',
  elt_bound=0,                                                   ! 1 si le fichier boundary_table existe déja
  nombre_entrees=1, nombre_sorties=1,                                ! 0 si fichier non formaté pour plusieurs entrées/sorties

  ! Initialisation avec un Barrage
  eqlin_barrage=0,                                               ! 1 pour initialiser avec un barrage
  x1eqbar=0.0501, x2eqbar=0.0500,         ! Abscisses des deux points du barrage
  y1eqbar=1.000, y2eqbar=0.0000,          ! Ordonnées des deux points du barrage
  H_AMONT=$2, U_AMONT=0, V_AMONT=0,                            ! Valeur de l'init du coté - de la normale au barrage
  H_AVAL=$3, U_AVAL =0, V_AVAL =0,                            ! Valeur de l'init du coté + de la normale au barrage

  ! Initialisation avec un plan
  plan=1                                                         ! 1 pour initialiser avec un plan
  xplan=0.0000, yplan=0.0000, zplan=1.                          ! Vecteur normal au plan
  xpoint=14.6282, ypoint=17.8976, zpoint=0.050 ! Point appartenant au plan

  ! Initialisation a partir d'un fichier, fichier avec solution ax elements
  solinit=0,                                                     ! 1 pour initialiser a partir du fichier
  fich_sol_init='',      ! nom du fichier d'initialisation

  ! Conditions aux limites
  inlet='inflow',                                                ! Type d'entrée : {inflow,transm}
  inlet_name=\"Inflow nodes\",
  debitglob=$1,                                               ! Débit aux entrées, séparer les débits pas des ,
  outlet=\"fixedheight\",
  outlet_name=\"Outflow nodes\",
  H_sortie=0.0,                                                 ! Hauteur du niveau à la sortie du domaine

  ! Paramètres des schémas numériques
  IFLUX=2, ilimiteur=3, iupwind=1,                                ! 1 -> HLLC , 2 -> WAF Riadh, 3 -> WAF Loukili
  tolisec=1.0E-05,                                               ! Tolérence sec/mouillé
  friction=1,                                                    ! 1 pour prendre en compte la friction
  fricimplic=1,                                                  ! 0 -> explicite, 1 -> I-dt/2*J, 2 -> I-dt*B

  TS=360.0, CFL=0.9,                                             ! Temps maximal de simultion, nombre CFL
  is_dt_constant=0, constant_dt=1e-5,
  tol_reg_perm=1.0E-15,                                          ! Tolérence relative entre debit entrée et débit sortie
  freqaffich=100,                                               ! Frequence de print dans outfile.[0-9]

  sol_z_offset=0.
  solrestart=0,                                                  ! Save sol for restart
  solvisu=3, visu_snapshots=180,                                  ! 1 all, 2 vtk, 3 cgns, 4 simple nodes, 5 simple elems
  sortie_finale_bluekenue=0/                                     ! Sauvergarde fichiers T3S à la fin
" > donnees.f

