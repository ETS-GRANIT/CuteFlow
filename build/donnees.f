&DONNEES_NAMELIST 

  ! Données du terrain
  GP=9.81,
  is_override_manning=1, override_manning=0.02200,               ! nombre de manning qui override les autes si is_...=1

  ! Données du maillage
  meshfile='Mille_Iles_mesh_481930_elts.txt',                    ! Fichier de maillage
  elt_bound=0,                                                   ! 1 si le fichier boundary_table existe déja
  multi_entree=0, multi_sortie=0,                                ! 0 si fichier non formaté pour plusieurs entrées/sorties

  ! Initialisation avec un Barrage
  eqlin_barrage=1,                                               ! 1 pour initialiser avec un barrage
  x1eqbar=274997.75521489076, x2eqbar=274756.1533974407,         ! Abscisses des deux points du barrage
  y1eqbar=5043578.489765059, y2eqbar=5043885.982987268,          ! Ordonnées des deux points du barrage
  H_AMONT=33.0, U_AMONT=0, V_AMONT=0,                            ! Valeur de l'init du coté - de la normale au barrage
  H_AVAL =0.0, U_AVAL =0, V_AVAL =0,                             ! Valeur de l'init du coté + de la normale au barrage

  ! Initialisation avec un plan 
  plan=0                                                         ! 1 pour initialiser avec un plan
  xplan=-0.0001, yplan=-0.0001, zplan=1                          ! Vecteur normal au plan
  xpoint=274956.783790834, ypoint=5043746.46205659, zpoint=31.35 ! Point appartenant au plan

  ! Initialisation a partir d'un fichier, fichier avec solution ax elements
  solinit=0,                                                     ! 1 pour initialiser a partir du fichier
  fich_sol_init='Mille_Iles_solution_initiale_t67500s.txt',      ! nom du fichier d'initialisation

  ! Conditions aux limites
  inlet='inflow',                                                ! Type d'entrée : {inflow,transm}
  debitglob=0.0,                                                 ! Débit aux entrées, séparer les débits pas des ,
  debit_var=0, loi_debitglob='Mille_iles_Qt.txt',                ! Loi de debit fonctionne uniquement avec 1 entrée
  H_sortie=0.0,                                                  ! Hauteur du niveau à la sortie du domaine

  ! Paramètres des schémas numériques
  IFLUX=2,                                                       ! 1 -> HLLC zoka, 2 -> HLLC Riadh
  tolisec=1.0E-05,                                               ! Tolérence sec/mouillé pour HLLC zoka
  timedisc='euler',                                              ! Schéma en temps : {euler,second,runge}
  friction=1,                                                    ! 1 pour prendre en compte la friction
  fricimplic=1,                                                  ! 0 -> explicite, 1 -> I-dt/2*J, 2 -> I-dt*B
  is_dry_as_wall=0,                                              ! 1 pour mettre les mailles seches comme des murs
  local_time_step=0,                                             ! 1 pour utiliser le local time step

  TS=10.0, CFL=0.2,                                              ! Temps maximal de simultion, nombre CFL
  tol_reg_perm=1.0E-15,                                          ! Tolérence relative entre debit entrée et débit sortie
  freqaffich=5000,                                               ! Frequence de print dans outfile.[0-9]

  ! Sauvegarde de la solution
  nbrjauges=0, jauges_snapshots=100,                             ! Nombre de jauges, nombre de snapshots
  xjauges = 272494.31, 274767.53, 279066.02,                     ! Abscisses des jauges
  yjauges = 5042222.41, 5046515.26, 5050380.38,                  ! Ordonnées des jauges

  nbrcoupes=0, coupes_snapshots=100,                             ! Nombre de coupes, nombre de snapshots
  coupe_a    = -0.7946,    -2.6754,    -1.4439                   ! Coeficient a de ax+b=y
  coupe_b    =  5.2585e+06, 5.7768e+06, 5.4397e+06               ! Coeficient b de ax+b=y

  solrestart=1, restart_snapshots=10,                            ! Sauvegarde en overwrite la solution pour restart
  solbasic=0, basic_snapshots=10,                                ! 1 pour sauvegarder h sur les noeud (pierre)
  solvtk=1, vtk_snapshots=10,                                    ! 1 pour sauvegarder les fichiers vkt pour video
  sortie_finale_bluekenue=0/                                     ! Sauvergarde fichiers T3S à la fin
