#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --gres=gpu:p100:2
#SBATCH --mem=8000M
#SBATCH --time=0-00:20
#SBATCH --account=def-soulaima
#SBATCH --output=outfile.out

mpirun --mca pml ob1 -n 2 sh -c './cuteflow > outfile.$OMPI_COMM_WORLD_RANK'
