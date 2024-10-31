#!/bin/bash

set -e

# Clone
#git clone --recurse-submodules -b cmake/rlm4 https://github.com/cedricga91/framework.git
#cd framework

# Build directory
mkdir build
cd build

# Modules environment
source /soft/irsrvsoft1/expl/eb/r11/centos_8/envs/toolchain.sh
source /soft/irsrvsoft1/expl/eb/r11/centos_8/envs/arcuser.sh
source /soft/irsrvsoft1/expl/eb/r11/centos_8/envs/arcdev.sh
source /soft/irsrvsoft1/expl/eb/r11/centos_8/envs/arcsolver.sh
source /soft/irsrvsoft1/expl/eb/r11/centos_8/envs/arcsolver-advanced.sh

# CMake
unset CMAKE_LIBRARY_PATH
export GFORTRAN_ROOT="${EBROOTGCCCORE}"
cmake -DREMOVE_UID_ON_DETACH=ON -DCMAKE_DISABLE_FIND_PACKAGE_Xdmf=TRUE -DUSE_GTEST_DEATH_TEST=OFF -DUSE_GRAPH_CONNECTIVITY_POLICY=ON -DALIEN_BUILD_COMPONENT=all ..

# Make
make -j12

# Test
ctest --output-on-failure -R 'alien.bench.trilinosmuelu.parallel.mpi-4'
