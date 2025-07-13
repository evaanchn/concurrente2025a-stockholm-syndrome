#!/bin/bash

#SBATCH --overcommit          # Allow more processes than CPUs per node
#SBATCH -J omp_mpi          # Job name
#SBATCH -o job.%j.txt         # Stdout output file (%j expands to jobId)
#SBATCH -N 7                  # Total number of nodes requested
#SBATCH -n 7                  # Total number of processes requested
#SBATCH -t 00:29:00           # Run time (hh:mm:ss) - 1 min
#SBATCH -p normal             # Partition to use (default, normal, debug)

prun perf stat bin/nbody 100 1 1000.0 1 10 1 5 -100 100 0 0