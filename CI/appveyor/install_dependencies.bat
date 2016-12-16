REM @file    CI/appveyor/install_dependencies.bat
REM @date    nov. 2015
REM @author  PhRG / opticalp.fr
REM @license MIT

REM Install dependencies:
REM  - build POCO from latest git release. 

set CURRENT_DIR=%CD%
REM echo "current dir is %CURRENT_DIR%"

REM set POCO_INSTALL_PREFIX=%PROGRAMFILES%\Poco
REM echo "Poco install destination directory: %POCO_INSTALL_PREFIX%"

REM get last poco via github
echo "getting the last poco release from github"
mkdir dependencies
cd dependencies
git clone https://github.com/pocoproject/poco.git
echo "Poco is now cloned. "
cd poco

REM now using msys to make things easier (getting last release)
%SH_COMMAND% -c "POCO_TAG_NAME=$(git describe --tags) ; git checkout $POCO_TAG_NAME"

REM build using cmake
echo "building poco with cmake, configuration: %CONFIGURATION%" 
cd .. 
mkdir poco-build 
cd poco-build

echo "Preparing to install POCO into %POCO_INSTALL_PREFIX%"
echo "CMake generator is: %GENERATOR%"
cmake ../poco -DENABLE_XML=ON -DENABLE_JSON=ON -DENABLE_MONGODB=OFF -DENABLE_PDF=OFF ^
    -DENABLE_UTIL=ON -DENABLE_NET=OFF -DENABLE_NETSSL=OFF -DENABLE_NETSSL_WIN=OFF -DENABLE_CRYPTO=OFF ^
    -DENABLE_DATA=OFF -DENABLE_DATA_SQLITE=OFF -DENABLE_DATA_MYSQL=OFF -DENABLE_DATA_ODBC=OFF ^
    -DENABLE_SEVENZIP=OFF -DENABLE_ZIP=OFF -DENABLE_APACHECONNECTOR=OFF -DENABLE_CPPPARSER=OFF ^
    -DENABLE_POCODOC=OFF -DENABLE_PAGECOMPILER=OFF -DENABLE_PAGECOMPILER_FILE2PAGE=OFF ^
    -DENABLE_REDIS=OFF ^
    -DCMAKE_INSTALL_PREFIX="%POCO_INSTALL_PREFIX%" -G"%GENERATOR%" 
cmake --build . --config %CONFIGURATION%
REM echo "Poco build done. "
cmake --build . --target install --config %CONFIGURATION%
REM echo "Poco install done. "

REM %PATH% has to be modified to take into account poco .dlls
cd %CURRENT_DIR%

REM fix python configuration for 2.7.11
if "%ARCH%"=="Win64" (
  set PYTHONHOME=C:\Python27-x64
) else (
  set PYTHONHOME=C:\Python27
)

echo "Install OpenCV using chocolatey"
choco upgrade chocolatey
choco info opencv
set OPENCV=c:\jojo
choco install opencv --params="/Environment /InstallationPath:%OPENCV%" --version=2.4.13 -y
echo "OpenCV installed"

call refreshenv
echo "PATH is now: %PATH%"
echo "OPENCV_DIR is: %OPENCV_DIR%"
