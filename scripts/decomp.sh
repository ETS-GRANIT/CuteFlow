#!/bin/bash
#SBATCH --ntasks-per-node=1
#SBATCH --mem=32000M
#SBATCH --time=0-02:00
#SBATCH --account=def-soulaima
#SBATCH --output=outfile.out

module load gcc metis
./split_mesh nom_fichier_maillage nombre_sous_dom multi_entrees multi_sorties
