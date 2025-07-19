include ../common/Makefile

FLAGS += -fopenmp

INCLUDE += -Iinclude

CC=mpicc
XC=mpic++

# MPI
# export LD_LIBRARY_PATH=/usr/local/mpich/lib:$LD_LIBRARY_PATH #temp

ARGS=universes/univ002.tsv 60 7200

.PHONY:random_mode file_mode

random_mode:
	$(MAKE) run ARGS='100 1 1000.0 1 10 1 5 -100 100 0 0'

file_mode:
	$(MAKE) run ARGS='universes/univ002.tsv 60 7200'

RUNPRE = mpiexec -np 3
