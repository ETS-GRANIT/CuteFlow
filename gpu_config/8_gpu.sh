#!/bin/bash
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=4
#SBATCH --gres=gpu:4
#SBATCH --mem=8000M
#SBATCH --time=0-02:00
#SBATCH --account=def-soulaima
#SBATCH --output=outfile.out

module load pgi/19.4 cuda/10.0.130 openmpi/3.1.2

mpirun --mca pml ob1 --mca btl openib -n 8 sh -c './runfile > outfile.$OMPI_COMM_WORLD_RANK'
