include ../common/Makefile

INCLUDE += -Iinclude

CC=mpicc
XC=mpic++

# MPI
# export LD_LIBRARY_PATH=/usr/local/mpich/lib:$LD_LIBRARY_PATH #temp

ARGS=universes/univ011.tsv 10 30

.PHONY:random_mode file_mode

random_mode:
	$(MAKE) run ARGS='10 60.0 7200.0'

file_mode:
	$(MAKE) run ARGS='universes/univ002.tsv 60 60'
