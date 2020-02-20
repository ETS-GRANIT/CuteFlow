#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --gres=gpu:2
#SBATCH --mem=8000M
#SBATCH --time=0-02:00
#SBATCH --account=def-soulaima
#SBATCH --array=1-10

module load pgi/19.4 cuda/10.0.130 openmpi/3.1.2

cd multi_${SLURM_ARRAY_TASK_ID}
mpirun --mca pml ob1 --mca btl openib -n 2 sh -c './runfile > outfile.$OMPI_COMM_WORLD_RANK'
