REM ----------------------------------------------------------------
REM BUILDALL
REM builds SoapySDR utilities, library and devices support libraries
REM without ask.
REM ----------------------------------------------------------------
@ECHO off
SET INSTALL_PREFIX="D:\Users\massimo\Documents\echoes-git\trunk\echoes\deps\x86_64\Soapy"
rem SET INSTALL_PREFIX="C:\temp\radio\echoes-git\trunk\echoes\deps\x86_32\Soapy"
REM SET INSTALL_PREFIX="C:\Users\Massimo\Documents\echoes-git\trunk\echoes\deps\x86_64\Soapy"

SET make="D:\Qt\Tools\mingw810_64\bin\mingw32-make.exe"
REM SET make="D:\Qt\Tools\mingw730_64\bin\mingw32-make.exe"
REM SET make="C:\Qt\Tools\mingw730_32\bin\mingw32-make.exe"
REM SET make="C:\Qt\Tools\mingw730_64\bin\mingw32-make.exe"


REM for laziness, the libusb provided is the static library with its header
REM so there is no .pc file and CMake is not able to detect it implicitly

SET LIBUSB_INCLUDE_DIR="D:\Users\massimo\Documents\sviluppo\echoes-related\soapy\myRepo\deps\include"
SET LIBUSB_LIBRARY_DIRS="D:\Users\massimo\Documents\sviluppo\echoes-related\soapy\myRepo\deps\lib64"
REM SET LIBUSB_INCLUDE_DIR="C:\temp\Soapy\deps\include"
REM SET LIBUSB_LIBRARY_DIRS="C:\temp\Soapy\deps\lib32"
REM SET LIBUSB_INCLUDE_DIR="C:\Users\Massimo\Documents\sviluppo\echoes-related\Soapy\myRepo\deps\include"
REM SET LIBUSB_LIBRARY_DIRS="C:\Users\Massimo\Documents\sviluppo\echoes-related\Soapy\myRepo\deps\lib64"

REM ----------------------------------------------------------------


SET LIBUSB_LIBRARIES="libusb-1.0.a"
SET BUILD_TYPE="Release"
SET SRCDIR=%CD%

REM builds the unixem library first

CD UNIXem\build\mingw730
%make%
CD %SRCDIR%
REM install not needed
@ECHO "UNIXem done, please take a look above for errors before continuing"
REM

:skipUNIXem
CD SoapySDR\build
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX%  -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..
%make% 
%make% install
CD %SRCDIR%
@ECHO "SoapySDR installed, please take a look above for errors before continuing"
REM

:skipSoapySDR
CD librtlsdr
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DLIBUSB_INCLUDE_DIR=%LIBUSB_INCLUDE_DIR% -DLIBUSB_LIBRARY_DIRS=%LIBUSB_LIBRARY_DIRS% -DLIBUSB_LIBRARIES=%LIBUSB_LIBRARIES% .
%make% VERBOSE=1
%make% install
CD %SRCDIR%
@ECHO "librtlsdr SDK installed, please take a look above for errors before continuing"
REM

:skiplibrtlsdr
CD SoapyRTLSDR
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% .
%make%
%make% install
CD %SRCDIR%
@ECHO "SoapyRTLSDR support library installed, please take a look above for errors before continuing"
REM

:skipSoapyRTLSDR
CD airspyone_host
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DLIBUSB_INCLUDE_DIR=%LIBUSB_INCLUDE_DIR% -DLIBUSB_LIBRARY_DIRS=%LIBUSB_LIBRARY_DIRS% -DLIBUSB_LIBRARIES=%LIBUSB_LIBRARIES% -DPTHREAD_LIBRARY_DIRS=%LIBUSB_LIBRARY_DIRS% .
%make%
%make% install
CD %SRCDIR%
@ECHO "airspyone_host SDK installed, please take a look above for errors before continuing"
REM

:skipairspyone_host
CD SoapyAirspy
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% .
%make%
%make% install
CD %SRCDIR%
@ECHO "SoapyAirspy support library installed, please take a look above for errors before continuing"
REM

:skipSoapyAirspy
CD rtAudio\_build_
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..
%make% VERBOSE=1
%make% install
CD %SRCDIR%
@ECHO "rtAudio SDK installed, please take a look above for errors before continuing"
REM

:skiprtAudio
CD SoapyAudio
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_PREFIX_PATH=%INSTALL_PREFIX% .
%make%
%make% install
CD %SRCDIR%
@ECHO "SoapyAudio support library installed, please take a look above for errors before continuing"
REM

:skipSoapyAudio
CD SoapyRemote
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_PREFIX_PATH=%INSTALL_PREFIX% .
%make%
%make% install
CD %SRCDIR%
@ECHO "SoapyRemote support library installed, please take a look above for errors before continuing"
REM

:skipSoapyRemote
