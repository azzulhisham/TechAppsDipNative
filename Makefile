SHELL=/bin/bash
MKLROOT=/hpcfs2/development/intel_psxe_2020/compilers_and_libraries_2020.1.217/linux/mkl
CC=mpicc -cc=icc
CPP_FLAGS=-std=c++11 -m64 -O3 
INCLUDE=-I src -I segy -I mpi
LIBS=${MKLROOT}/lib/intel64/libmkl_intel_lp64.a ${MKLROOT}/lib/intel64/libmkl_sequential.a ${MKLROOT}/lib/intel64/libmkl_core.a
VPATH = segy src
ENV = source /hpcfs2/development/intel_psxe_2020/parallel_studio_xe_2020/bin/psxevars.sh

all: segy src mpi hpc

obj/%.o: %.cpp
	$(CC) -c $< -o $@ $(INCLUDE) $(CPP_FLAGS)

segy:  $(patsubst segy/%.cpp, obj/%.o, $(wildcard segy/*.cpp))
src:  $(patsubst src/%.cpp, obj/%.o, $(wildcard src/*.cpp))
mpi:  $(patsubst src/%.cpp, obj/%.o, $(wildcard src/*.cpp))

hpc:
	$(CC) -Wl,--start-group $(LIBS) obj/*.o -Wl,--end-group  -DMKL_ILP64 -mkl=parallel -liomp5 -lpthread -lm -ldl -qopenmp $(CPP_FLAGS) -o bin/ENTD_HPC
test:
	mpirun ./bin/ENTD_HPC -I ~/AHPC/salina_test.sgy -F 2,15,15,30,30,45 -v 2
clean:
		rm -rf obj/*
