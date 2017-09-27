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
git tag
git describe --match *release --abbrev=0
%SH_COMMAND% -c "pwd"
%SH_COMMAND% -c "git describe --match *release --abbrev=0"
%SH_COMMAND% -c "POCO_TAG_NAME=$(git describe --match *release --abbrev=0) ; git checkout $POCO_TAG_NAME"

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

echo Install OpenCV using curl and 7zip
curl -fSL -o opencv.exe -m 600 http://downloads.sourceforge.net/project/opencvlibrary/opencv-win/%OPENCV_VERSION%/opencv-%OPENCV_VERSION%.exe
echo Downloading from sourceforge done. 
REM opencv.exe -y -o"%OPENCV_BASE_DIR"
call 7z x opencv.exe -o"%OPENCV_BASE_DIR%"
dir "%OPENCV_DIR%"
echo OpenCV installed

echo Install wxWidgets using curl 
curl -fSL -o wxWidgets.7z -m 600 https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.0/wxWidgets-3.1.0.7z
echo Downloading from github done. 
call 7z x wxWidgets.7z -o%WXWIN%
echo wxWidgets 3.0.2 downloaded and extracted. 
cd %WXWIN%\build\msw\
if "%Configuration%"=="Debug" (
	nmake -f makefile.vc BUILD=debug SHARED=1
) else (
	nmake -f makefile.vc BUILD=release SHARED=1
)
