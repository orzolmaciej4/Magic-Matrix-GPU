#!/bin/bash

#SBATCH --job-name=magic_matrix
#SBATCH --output=magic_matrix_%j.out
#SBATCH --nodes=1	#number of nodes
#SBATCH --ntasks-per-node=1 #number of tasks per node
#SBATCH --cpus-per-task=4 #number of cores per task
#SBATCH --partition=cpu	#number of compute nodes
#SBATCH --time 0:0:30	#time limit for the job

module load nvidia-hpc
nvc++ -fopenmp -mp=gpu magic_matrix.cpp -o magic_matrix

DATA_DIR=./data_sets
N_VALUES="3 10"

for N in $N_VALUES; do
	PATTERN_FILE="${DATA_DIR}/pattern${N}x${N}.dat"
	MODIFIER_FILE="${DATA_DIR}/modifier${N}x${N}.dat"

	echo "Running magic_matrix for N=${N}"

	omp_num_threads=4
	OMP_NUM_THREADS=${omp_num_threads} srun -c ${omp_num_threads} ./magic_matrix ${PATTERN_FILE} ${MODIFIER_FILE}
done

wait #wait for all jobs to finish

echo "The parallel execution of magic_matrix.cpp on the data sets of size N=3 and N=10 in the directory data_sets is completed"
