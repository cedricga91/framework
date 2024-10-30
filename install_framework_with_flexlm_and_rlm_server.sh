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

# Patch for Arcane RLM Feature
sed -i_orig 's/ArcaneCore/Arcane/g' ../arcane/src/arcane/impl/FlexLMTools.cc
sed -i_orig \
  -e 's/ArcaneCore/Arcane/g' \
  -e 's,return (Real)ARCANE_VERSION_MAJOR + (Real)ARCANE_VERSION_MINOR / 100 + (Real)ARCANE_VERSION_RELEASE / 1000 + (Real)ARCANE_VERSION_BETA / 10000;,return (Real)20241231.0;,g' \
  ../arcane/src/arcane/impl/FlexLMTools.h
#cp -avf ../arcane/src/arcane/impl/FlexLMTools.h_StringVersion ../arcane/src/arcane/impl/FlexLMTools.h

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

set +e

#export BEICIP_LICENSE_FILE=1761@irlinv-lic1:1761@irlinv-lic2:1761@irlinv-lic3 # BEICIP
#export LM_LICENSE_FILE=1610@irlinv-lic1 # IFPEN
ctest --output-on-failure -R 'alien.bench.trilinosmuelu.parallel'

export RLM_LICENSE=5053@localhost
ctest --output-on-failure -R 'alien.bench.trilinosmuelu.parallel'
