#!/bin/bash

# @file    CI/travis/install_dependencies.sh
# @date    nov. 2015
# @author  PhRG / opticalp.fr
# @license MIT
#
# Install dependencies:  
#  - Poco, check POCO_BUILD to install either from source or from packages
#


# exit as soon as a line fails, and print lines before executing it
set -ev

CURRENT_DIR=$(pwd)
# echo "current dir is $CURRENT_DIR"

# test if poco has to be built
if [ $BUILD_POCO == true ] || [ "$TRAVIS_OS_NAME" == "linux" ]
then
    echo "BUILD_POCO option active"

    wget -V
    echo "as soon as wget version is > 1.10, we should disable certificate checking"

    if [ "$TRAVIS_OS_NAME" == "linux" ]
    then 
        echo "current cmake version is:"
        cmake --version
        CMAKE_VERSION_MAJOR_MINOR="3.9"
        CMAKE_VERSION_PATCH="3"
        CMAKE_VERSION="${CMAKE_VERSION_MAJOR_MINOR}.${CMAKE_VERSION_PATCH}"
        echo "updating cmake to ${CMAKE_VERSION} (3.2.0 required for poco build) "
        echo "Linux: getting directly recent CMake binaries..."
        cd
        wget --no-check-certificate "https://www.cmake.org/files/v${CMAKE_VERSION_MAJOR_MINOR}/cmake-${CMAKE_VERSION}-Linux-x86_64.tar.gz"
        tar -xzvf cmake-${CMAKE_VERSION}-Linux-x86_64.tar.gz --strip 1 --exclude="*/doc/*" --exclude="*/man/*"
        cd $CURRENT_DIR
#    else
#        echo "Not Linux. CMake needs to be built from sources. "
#        wget --no-check-certificate "https://www.cmake.org/files/v${CMAKE_VERSION_MAJOR_MINOR}/cmake-${CMAKE_VERSION}.tar.gz"
#        tar xzf "cmake-${CMAKE_VERSION}.tar.gz"
#        cd "cmake-${CMAKE_VERSION}"
#        cmake -DCMAKE_INSTALL_PREFIX=~ .
#        make -j2
#        make install
#        cd ..
    fi
    export PATH="~/bin:${PATH}"
    # check that the version in PATH is the right one
    which cmake
    cmake --version
fi

if [ $BUILD_POCO == true ]
then
    # get last poco via github
    echo "getting the last poco release from github"
    mkdir dependencies
    cd dependencies
    git clone https://github.com/pocoproject/poco.git
    cd poco
    POCO_TAG_NAME=$(git describe --match "*release" --abbrev=0)
    echo "checking out poco at tag: $POCO_TAG_NAME"
    git checkout $POCO_TAG_NAME

    # build using cmake, selecting some components
    echo "building poco with cmake" # which components?
    cd ..; mkdir poco-build; cd poco-build
    cmake ../poco -DENABLE_XML=ON -DENABLE_JSON=ON -DENABLE_MONGODB=OFF -DENABLE_PDF=OFF \
    -DENABLE_UTIL=ON -DENABLE_NET=OFF -DENABLE_NETSSL=OFF -DENABLE_NETSSL_WIN=OFF -DENABLE_CRYPTO=OFF \
    -DENABLE_DATA=OFF -DENABLE_DATA_SQLITE=OFF -DENABLE_DATA_MYSQL=OFF -DENABLE_DATA_ODBC=OFF \
    -DENABLE_SEVENZIP=OFF -DENABLE_ZIP=OFF -DENABLE_APACHECONNECTOR=OFF -DENABLE_CPPPARSER=OFF \
    -DENABLE_POCODOC=OFF -DENABLE_PAGECOMPILER=OFF -DENABLE_PAGECOMPILER_FILE2PAGE=OFF \
    -DENABLE_REDIS=OFF
    make help
    make -j2

    # install using cmake
    echo "poco lib: cmake install"
    sudo make install
fi

# if poco is not being built, then get it from the packages
if [ $BUILD_POCO == false ]
then
    echo "Get poco from the packages: "
    # installing from packages
    if [ $TRAVIS_OS_NAME == linux ] 
    then 
    	echo "using apt-get"
        sudo apt-get install libpoco-dev -q -y 
        # echo "poco lib installed with apt-get" 
    fi
    if [ $TRAVIS_OS_NAME == osx ] 
    then 
        echo "using homebrew"
        brew info poco
        brew install poco
        # echo "poco lib installed with homebrew" 
    fi
fi

echo "Installing openCV from the packages"
if [ $TRAVIS_OS_NAME == linux ] 
then 
    echo "using apt-get"
    sudo apt-get install libopencv-dev xvfb -q -y 
    # echo "opencv lib installed with apt-get" 
fi
if [ $TRAVIS_OS_NAME == osx ] 
then 
    echo "using homebrew"
    /usr/bin/yes | pip2 uninstall numpy # see: travis-ci/travis-ci#6688
    brew install opencv
    # echo "opencv lib installed with homebrew" 
fi
echo "openCV installed"

echo "Installing wxWidgets from the packages"
if [ $TRAVIS_OS_NAME == linux ] 
then 
    echo "using apt-get"
    sudo apt-get install libwxgtk3.0-dev xvfb -q -y 
    echo "wxWidgets lib (wxgtk) installed with apt-get" 
fi
if [ $TRAVIS_OS_NAME == osx ] 
then 
    echo "using homebrew"
    brew install wxmac
    echo "wxwidgets lib (wxmac) installed with homebrew" 
fi
echo "wxWidgets installed"

cd $CURRENT_DIR
pwd
