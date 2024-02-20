#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --gres=gpu:p100:1
#SBATCH --mem=4000M
#SBATCH --time=0-00:5
#SBATCH --account=def-soulaima
#SBATCH --output=outfile.out

mpirun --mca pml ob1 -n 1 sh -c ./cuteflow
