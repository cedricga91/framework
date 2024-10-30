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

# Patch for Arcane RLM Feature
sed -i_orig -e 's/ArcaneCore/Arcane/g' ../arcane/src/arcane/impl/FlexLMTools.cc ../arcane/src/arcane/impl/FlexLMTools.h
#echo "20241231.0" > ../arcane/version
#See : arcane/src/arcane/impl/FlexLMTools.h
#  static Real getVersion(eFeature feature)
#  {
#    ARCANE_UNUSED(feature);
#    // Ecrit une version comparable numériquement; ex: 1.0610 (au lieu de 1.6.1)
#    return (Real)ARCANE_VERSION_MAJOR + (Real)ARCANE_VERSION_MINOR / 100 + (Real)ARCANE_VERSION_RELEASE / 1000 + (Real)ARCANE_VERSION_BETA / 10000;
#  }

# CMake
unset CMAKE_LIBRARY_PATH
export GFORTRAN_ROOT="${EBROOTGCCCORE}"
export ARCANE_CONFIG_ENABLED_PACKAGES="LICENSE_SYSTEM"
# L'option LICENSE_SYSTEM peut prendre 2 valeur : RLM (défaut) ou FLEXLM
# RLM est cherché via la variable d'environnement RLM_ROOT
# FLEXLM est cherché via la variable d'environnement FLEXLM_ROOT
cmake -DREMOVE_UID_ON_DETACH=ON -DCMAKE_DISABLE_FIND_PACKAGE_Xdmf=TRUE -DUSE_GTEST_DEATH_TEST=OFF -DUSE_GRAPH_CONNECTIVITY_POLICY=ON -DALIEN_BUILD_COMPONENT=all -DLICENSE_SYSTEM=RLM ..

# Make
make -j12
#make -j12 arcane_impl

# Test
export RLM_LICENSE=5053@localhost
ctest --output-on-failure -R 'alien.bench.trilinosmuelu.parallel'
