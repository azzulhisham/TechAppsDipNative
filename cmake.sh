#!/usr/bin/env bash

#source /hpcfs2/development/intel_psxe_2020/parallel_studio_xe_2020/bin/psxevars.sh

module load intel/psxe/2020
~/cmake-3.19.4268486-MSVC_2-Linux-x64/bin/cmake -Bbuild -H. -D CMAKE_C_COMPILER=icc -D CMAKE_CXX_COMPILER=icc -DCMAKE_BUILD_TYPE=Release
cd build && make
cp -av mpi/ENTD_HPC /hpcfs2/GRT/Exp_Tech/ENTD/ENTD_HPC
