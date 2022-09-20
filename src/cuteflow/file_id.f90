module file_id
  integer, parameter :: lu_fich             = 10        ! fichier lecture donnees limites (limit reading file)
  integer, parameter :: lu_init             = 40        ! fichier lecture donnees initiales (file reading initial data) 
  integer, parameter :: lec_mesh             = 41        ! fichier lecture donnees initiales (file reading initial data) 
  integer, parameter :: ec_Bluekenu_eta     = 803       ! fichier ecriture so lution 3d pour Bluekenu (3d writing file for Bluekenu)
  integer, parameter :: ec_Bluekenu_h       = 804       ! fichier ecriture solution 3d pour Bluekenu (file writing 3d solution for Bluekenu) 
  integer, parameter :: ec_Bluekenu_u       = 805       ! fichier ecriture solution 3d pour Bluekenu (file writing 3d solution for Bluekenu) 
  integer, parameter :: ec_Bluekenu_v       = 806       ! fichier ecriture solution 3d pour Bluekenu (file writing 3d solution for Bluekenu)
  integer, parameter :: ec_Paraview         = 901       ! fichier ecriture so lution 3d pour Paraview (3d writing file for Paraview)
  integer, parameter :: ec_sol_elt_t        = 1006      ! fichier ecriture stockage des solutions dans les cellules a intervalle de temps (file write storage solutions in cells at time interval)
  integer, parameter :: ec_sol_simple     = 1011    
end module file_id
