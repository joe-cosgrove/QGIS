export DEBIAN_FRONTEND=noninteractive
export CORES=2

##################################################
#
# Get precompiled dependencies
#
##################################################

pushd ${HOME}

curl -L https://github.com/opengisch/osgeo4travis/archive/qt5bin.tar.gz | tar -xzC /home/travis --strip-components=1
curl -L https://cmake.org/files/v3.5/cmake-3.5.0-Linux-x86_64.tar.gz | tar --strip-components=1 -zxC /home/travis/osgeo4travis
popd

export CXX="clang++-${LLVM_VERSION}"
export CC="clang-${LLVM_VERSION}"
export PATH=${HOME}/osgeo4travis/bin:${PATH}

ln -s ${HOME}/osgeo4travis/bin/ccache ${HOME}/osgeo4travis/bin/clang++-${LLVM_VERSION}
ln -s ${HOME}/osgeo4travis/bin/ccache ${HOME}/osgeo4travis/bin/clang-${LLVM_VERSION}
ccache -s

#checkout known good commit from clazy and build
export LLVM_ROOT="/usr/lib/llvm-3.8"
git clone --depth 1 https://github.com/nyalldawson/clazy.git
cd clazy
cmake .
make -j2
pwd
echo $TRAVIS_BUILD_DIR
cd ..
