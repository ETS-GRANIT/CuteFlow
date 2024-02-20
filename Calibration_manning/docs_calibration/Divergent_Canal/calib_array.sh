#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --gres=gpu:p100:1
#SBATCH --mem=4000M
#SBATCH --time=0-00:5
#SBATCH --account=def-soulaima
#SBATCH --array=1-256

cd Case_${SLURM_ARRAY_TASK_ID}
mpirun --mca pml ob1 -n 1 sh -c './cuteflow > outfile.$OMPI_COMM_WORLD_RANK'
