#!/bin/bash
#SBATCH --ntasks=1
#SBATCH --mem=16000M
#SBATCH --time=0-05:00
#SBATCH --account=def-soulaima
#SBATCH --output=outfile.out

./split_mesh Mille_Iles_mesh_5237773_elts.txt 32
