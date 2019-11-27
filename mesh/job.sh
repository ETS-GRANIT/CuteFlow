#!/bin/bash
#SBATCH --ntasks=1
#SBATCH --mem=16000M
#SBATCH --time=0-05:00
#SBATCH --account=def-soulaima
#SBATCH --output=outfile.out

#./main Mille_Iles_mesh_11769800_elts.txt 64
./main Mille_Iles_mesh_5237773_elts.txt 32
