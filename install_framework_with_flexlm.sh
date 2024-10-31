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

# FlexLM environment
module load FlexNetPublisher/11.14.1.3_v6-beicip
#module load FlexNetPublisher/11.14.1.3_v6-ifp_lmd

# CMake
unset CMAKE_LIBRARY_PATH
export GFORTRAN_ROOT="${EBROOTGCCCORE}"
export ARCANE_CONFIG_ENABLED_PACKAGES="LICENSE_SYSTEM"
# L'option LICENSE_SYSTEM peut prendre 2 valeur : RLM (défaut) ou FLEXLM
# RLM est cherché via la variable d'environnement RLM_ROOT
# FLEXLM est cherché via la variable d'environnement FLEXLM_ROOT
cmake -DREMOVE_UID_ON_DETACH=ON -DCMAKE_DISABLE_FIND_PACKAGE_Xdmf=TRUE -DUSE_GTEST_DEATH_TEST=OFF -DUSE_GRAPH_CONNECTIVITY_POLICY=ON -DALIEN_BUILD_COMPONENT=all -DLICENSE_SYSTEM=FLEXLM ..

# Make
make -j12

# Test
# Beicip FlexLM
export BEICIP_LICENSE_FILE=1761@irlinv-lic1:1761@irlinv-lic2:1761@irlinv-lic3
export LM_LICENSE_FILE=1761@irlinv-lic1:1761@irlinv-lic2:1761@irlinv-lic3
ctest --output-on-failure -R 'alien.bench.trilinosmuelu.parallel.mpi-4'
