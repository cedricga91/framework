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

# RLM environment
export CPATH="/work2/adm-ci-r11/data/rlm/licenseapi/include:$CPATH"
export LIBRARY_PATH="/work2/adm-ci-r11/data/rlm/licenseapi/lib:$LIBRARY_PATH"
export LD_LIBRARY_PATH="/work2/adm-ci-r11/data/rlm/licenseapi/lib:$LD_LIBRARY_PATH"
export RLM_ROOT="/work2/adm-ci-r11/data/rlm/licenseapi"

# CMake
unset CMAKE_LIBRARY_PATH
export GFORTRAN_ROOT="${EBROOTGCCCORE}"
export ARCANE_CONFIG_ENABLED_PACKAGES="LICENSE_SYSTEM"
# L'option LICENSE_SYSTEM peut prendre 2 valeur : RLM (défaut) ou FLEXLM
# RLM est cherché via la variable d'environnement RLM_ROOT
# FLEXLM est cherché via la variable d'environnement FLEXLM_ROOT
cmake -DREMOVE_UID_ON_DETACH=ON -DCMAKE_DISABLE_FIND_PACKAGE_Xdmf=TRUE -DUSE_GTEST_DEATH_TEST=OFF -DUSE_GRAPH_CONNECTIVITY_POLICY=ON -DALIEN_BUILD_COMPONENT=all -DLICENSE_SYSTEM=RLM ..

# Make
make -j12 arcane_impl

# Test
export RLM_LICENSE=5053@localhost
