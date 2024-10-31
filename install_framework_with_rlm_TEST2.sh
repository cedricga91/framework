#!/bin/bash

set -e

# Clone
#git clone --recurse-submodules -b cmake/rlm4 https://github.com/cedricga91/framework.git
#cd framework

# Build directory
cd build

# Modules environment
source /soft/irsrvsoft1/expl/eb/r11/centos_8/envs/toolchain.sh
source /soft/irsrvsoft1/expl/eb/r11/centos_8/envs/arcuser.sh
source /soft/irsrvsoft1/expl/eb/r11/centos_8/envs/arcdev.sh
source /soft/irsrvsoft1/expl/eb/r11/centos_8/envs/arcsolver.sh
source /soft/irsrvsoft1/expl/eb/r11/centos_8/envs/arcsolver-advanced.sh

# RLM environment
export CPATH="/work2/adm-ci-r11/data/rlm/licenseapi/include:$CPATH"
export LIBRARY_PATH="/work2/adm-ci-r11/data/rlm/licenseapi/lib:$LIBRARY_PATH"
export LD_LIBRARY_PATH="/work2/adm-ci-r11/data/rlm/licenseapi/lib:$LD_LIBRARY_PATH"
export RLM_ROOT="/work2/adm-ci-r11/data/rlm/licenseapi"

# CMake
unset CMAKE_LIBRARY_PATH
export GFORTRAN_ROOT="${EBROOTGCCCORE}"
export ARCANE_CONFIG_ENABLED_PACKAGES="LICENSE_SYSTEM"

# Make
#make -j12
make -j12 arcane_impl

# Test
# Beicip RLM
export BEICIP_LICENSE_FILE=5053@localhost
export LM_LICENSE_FILE=5053@localhost
export RLM_LICENSE=5053@localhost
ctest --output-on-failure -R 'alien.bench.trilinosmuelu.parallel.mpi-4'
