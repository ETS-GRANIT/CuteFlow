#!/bin/bash
#SBATCH --ntasks=1
#SBATCH --gres=gpu:1
#SBATCH --mem=8000M
#SBATCH --time=0-12:00
#SBATCH --account=def-soulaima
#SBATCH --array=1-20

module load pgi/19.4 cuda/10.0.130 openmpi/3.1.2

cd multi_${SLURM_ARRAY_TASK_ID}
mpirun --mca pml ob1 --mca btl openib -n 1 sh -c './runfile > outfile.$OMPI_COMM_WORLD_RANK'
