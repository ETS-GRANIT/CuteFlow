module --force purge;\
  module load StdEnv/2020 gcc/9.3.0 openmpi/4.0.3 paraview-offscreen/5.10.0;\
  srun pvserver
