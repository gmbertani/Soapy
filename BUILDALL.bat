rem ----------------------------------------------------------------
rem BUILDALL
rem builds SoapySDR utilities, library and devices support libraries
rem ----------------------------------------------------------------
@echo off
SET INSTALL_PREFIX="D:\program files\Soapy"
SET MAKE="C:\Qt\Tools\mingw730_64\bin\mingw32-make.exe"

rem for laziness, the libusb provided is the static library with its header
rem so there is no .pc file and CMake is not able to detect it implicitly
SET LIBUSB_INCLUDE_DIR="D:\Users\massimo\Documents\sviluppo\echoes-related\soapy\deps\include"
SET LIBUSB_LIBRARY_DIRS="D:\Users\massimo\Documents\sviluppo\echoes-related\soapy\deps\lib64"
SET LIBUSB_LIBRARIES="libusb-1.0.a"

rem builds the unixem library firs
cd UNIXem\build\mingw730
%make% clean
%make%
cd ..\..\..
rem install not needed
@echo "UNIXem done, please take a look above for errors before continuing"
pause

cd SoapySDR\build
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*

echo WARNING: in case of error like "The C compiler gcc.exe is not able to compile a simple test program."
echo try to disable the antivirus for some minutes.
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX%  ..
%make% 
%make% install
cd ..\..
@echo "SoapySDR installed, please take a look above for errors before continuing"
pause

cd librtlsdr
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DLIBUSB_INCLUDE_DIR=%LIBUSB_INCLUDE_DIR% -DLIBUSB_LIBRARY_DIRS=%LIBUSB_LIBRARY_DIRS% -DLIBUSB_LIBRARIES=%LIBUSB_LIBRARIES% .
%make% VERBOSE=1
%make% install
cd ..
@echo "librtlsdr SDK installed, please take a look above for errors before continuing"
pause

cd SoapyRTLSDR
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% .
%make%
%make% install
cd ..
@echo "SoapyRTLSDR support library installed, please take a look above for errors before continuing"
pause

cd airspyone_host
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DLIBUSB_INCLUDE_DIR=%LIBUSB_INCLUDE_DIR% -DLIBUSB_LIBRARY_DIRS=%LIBUSB_LIBRARY_DIRS% -DLIBUSB_LIBRARIES=%LIBUSB_LIBRARIES% .
%make%
%make% install
cd ..
@echo "airspyone_host SDK installed, please take a look above for errors before continuing"
pause


cd SoapyAirspy
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% .
%make%
%make% install
cd ..
@echo "SoapyAirspy support library installed, please take a look above for errors before continuing"
pause


cd SoapyAudio
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_PREFIX_PATH=%INSTALL_PREFIX% .
%make%
%make% install
cd ..
@echo "SoapyAudio support library installed, please take a look above for errors before continuing"
pause