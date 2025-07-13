#!/bin/bash

#SBATCH --overcommit          # Allow more processes than CPUs per node
#SBATCH -J omp_mpi          # Job name
#SBATCH -o job.%j.txt         # Stdout output file (%j expands to jobId)
#SBATCH -N 3                  # Total number of nodes requested
#SBATCH -n 3                  # Total number of processes requested
#SBATCH -t 00:29:00           # Run time (hh:mm:ss) - 1 min
#SBATCH -p normal             # Partition to use (default, normal, debug)

prun perf stat bin/nbody universes/univ002.tsv 60 7200