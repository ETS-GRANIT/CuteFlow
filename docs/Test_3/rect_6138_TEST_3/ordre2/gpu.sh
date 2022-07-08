#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --gres=gpu:p100:1
#SBATCH --mem=8000M
#SBATCH --time=0-00:15
#SBATCH --account=rrg-soulaima-ac

mpirun --mca pml ob1 -n 1 sh -c './cuteflow > outfile.$OMPI_COMM_WORLD_RANK'
