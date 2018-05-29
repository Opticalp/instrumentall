# Installation

The installation can only be achieved via build (Jan., 2017). 

## Requested tools
 * A compiler:
    * gcc (Linux)
    * clang (Linux, OSX)
    * VS C++ (Win)
 * [CMake](http://cmake.org)

## Dependencies
 * [Poco libs](http://pocoproject.org)
 * [Python 2.7](http://python.org) is the common CLI
 * [OpenCV](http://opencv.org) if you want to work with images
 * [wxWidgets 3.x](https://www.wxwidgets.org) for the GUI elements
 * [EMVA GenAPI reference implementation](https://www.emva.org/standards-technology/genicam/genicam-downloads/) for the GenICam usage
 
## Other tools
 * To render the graphs exported by *InstrumentAll*, a [DOT](http://www.graphviz.org/Documentation.php)-language interpreter is needed
 * You may want to generate documentation using [Doxygen](http://www.doxygen.org)
 
## Example scripts
 Some install examples can be found in the CI files:
  * [appveyor.yml](../appveyor.yml) and its [install script](../CI/appveyor/install_dependencies.bat)
  * [.travis.yml](../.travis.yml) and its [install script](../CI/travis/install_dependencies.sh)
