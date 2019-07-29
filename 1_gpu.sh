#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --gres=gpu:1
#SBATCH --mem=2000M
#SBATCH --time=0-00:10
#SBATCH --account=def-soulaima
#SBATCH --output=outfile.out

module load pgi/17.3

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/cvmfs/restricted.computecanada.ca/easybuild/software/2017/Core/pgi/17.3/linux86-64/17.3/lib"

mpirun -n 1 sh -c './runfile > outfile.$OMPI_COMM_WORLD_RANK'
