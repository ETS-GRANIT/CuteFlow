#!/bin/bash
#SBATCH --nodes=8
#SBATCH --ntasks-per-node=2
#SBATCH --gres=gpu:2
#SBATCH --mem=2000M
#SBATCH --time=0-03:00
#SBATCH --account=def-soulaima
#SBATCH --output=outfile.out

module load pgi/17.3

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/cvmfs/restricted.computecanada.ca/easybuild/software/2017/Core/pgi/17.3/linux86-64/17.3/lib"

mpirun -n 16 sh -c './runfile > outfile.$OMPI_COMM_WORLD_RANK'
