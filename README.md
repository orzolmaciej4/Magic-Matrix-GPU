# Magic-Matrix-GPU

## Coursework Overview:
This project is part of the COMP3467 Advanced Computer Systems module at Durham University for the academic year 2023-24, focusing on parallel programming and system administration.

## Project Goals:
The primary aim is to parallelize a sequential implementation of a magic square algorithm and set up a framework for automatic correctness testing and performance measurements on the department's GPU cluster, NCC. The magic square algorithm generates large magic squares from smaller ones, and the goal is to optimize its performance using parallel programming techniques.

This project encapsulates the application of parallel programming techniques and system administration skills, highlighting the integration of GPU programming, cluster computing, and effective resource management in a high-performance computing environment.

## Technologies and Methods:
### Programming and Parallelization:
1. OpenMP for GPU Programming:
    - Task: The magic_matrix.cpp file was instrumented with OpenMP’s time measurement routines to identify computationally intensive functions.
    - Implementation: Developed a GPU implementation of the algorithm, saved as magic_matrix_gpu.cpp, using OpenMP. The code maintains correctness under concurrent execution and is compiled using a provided Makefile with a target named mmgpu.
    - Documentation: A report was created, detailing the approach to porting the code to GPUs, justifying the parallelization directives and memory management techniques, and comparing the runtime of the GPU implementation to the original sequential code for a matrix size of N=20.

### Cluster Computing:
2. sbatch Script for Parallel Execution:
    - Created a script (run_all_magic_matrix.sh) to allow the parallel execution of magic_matrix.cpp on data sets located in the directory data_sets on NCC.

3. MPI Parallelization Proposal:
    - The report includes a proposal for parallelizing magic_matrix.cpp with MPI, featuring a graphic of the data distribution and a discussion of the MPI functions used.

4. NUMA Control:
    - Developed a shell script (install_numactl.sh) to install the numactl tool locally in the home directory to ensure data locality for processes or threads operating on the same data.
Output: Documented the output of numactl -H executed on the login node in numactl.out.

5. Cluster Installation Proposal:
   The report explains how to make an installation of magic_matrix.cpp available to all users on the PVM (virtual machine cluster setup during practicals), discussing the pros and cons of the chosen approach.
