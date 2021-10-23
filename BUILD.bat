REM ----------------------------------------------------------------
REM BUILD
REM builds SoapySDR utilities, library and devices support libraries
REM ----------------------------------------------------------------
@ECHO off
REM SET INSTALL_PREFIX="D:\program files\Soapy"
SET INSTALL_PREFIX="D:\Users\massimo\Documents\echoes-git\trunk\echoes\deps\x86_64\Soapy"
REM SET INSTALL_PREFIX="D:\Users\massimo\Documents\echoes-git\trunk\echoes\deps\x86_32\Soapy"
SET make="D:\Qt\Tools\mingw730_64\bin\mingw32-make.exe"
rem SET make="C:\Qt\Tools\mingw730_64\bin\mingw32-make.exe"

REM for laziness, the libusb provided is the static library with its header
REM so there is no .pc file and CMake is not able to detect it implicitly
SET LIBUSB_INCLUDE_DIR="D:\Users\massimo\Documents\sviluppo\echoes-related\soapy\myRepo\deps\include"
SET LIBUSB_LIBRARY_DIRS="D:\Users\massimo\Documents\sviluppo\echoes-related\soapy\myRepo\deps\lib64"
rem SET LIBUSB_INCLUDE_DIR="C:\Users\massimo\Documents\sviluppo\echoes-related\soapy\myRepo\deps\include"
rem SET LIBUSB_LIBRARY_DIRS="C:\Users\massimo\Documents\sviluppo\echoes-related\soapy\myRepo\deps\lib64"
REM ----------------------------------------------------------------



SET LIBUSB_LIBRARIES="libusb-1.0.a"
SET BUILD_TYPE="Release"
SET SRCDIR=%CD%

REM builds the unixem library first
SET /P yn=Build UNIXem? (y/n): 
IF %yn% EQU n GOTO skipUNIXem
IF %yn% EQU N GOTO skipUNIXem
CD UNIXem\build\mingw730
%make%
CD %SRCDIR%
REM install not needed
@ECHO "UNIXem done, please take a look above for errors before continuing"
REM

:skipUNIXem
SET /P yn=Build SoapySDR? (y/n): 
IF %yn% EQU n GOTO skipSoapySDR
IF %yn% EQU N GOTO skipSoapySDR

MD SoapySDR\build
CD SoapySDR\build
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX%  -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..
%make% 
%make% install
CD %SRCDIR%
@ECHO "SoapySDR installed, please take a look above for errors before continuing"
REM

:skipSoapySDR
SET /P yn=Build librtlsdr? (y/n): 
IF %yn% EQU n GOTO skiplibrtlsdr
IF %yn% EQU N GOTO skiplibrtlsdr
CD librtlsdr
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DLIBUSB_INCLUDE_DIR=%LIBUSB_INCLUDE_DIR% -DLIBUSB_LIBRARY_DIRS=%LIBUSB_LIBRARY_DIRS% -DLIBUSB_LIBRARIES=%LIBUSB_LIBRARIES% .
%make% VERBOSE=1
%make% install
CD %SRCDIR%
@ECHO "librtlsdr SDK installed, please take a look above for errors before continuing"
REM

:skiplibrtlsdr
SET /P yn=Build SoapyRTLSDR? (y/n): 
IF %yn% EQU n GOTO skipSoapyRTLSDR
IF %yn% EQU N GOTO skipSoapyRTLSDR
CD SoapyRTLSDR
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% .
%make%
%make% install
CD %SRCDIR%
@ECHO "SoapyRTLSDR support library installed, please take a look above for errors before continuing"
REM

:skipSoapyRTLSDR
SET /P yn=Build airspyone_host? (y/n): 
IF %yn% EQU n GOTO skipairspyone_host
IF %yn% EQU N GOTO skipairspyone_host
CD airspyone_host
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DLIBUSB_INCLUDE_DIR=%LIBUSB_INCLUDE_DIR% -DLIBUSB_LIBRARY_DIRS=%LIBUSB_LIBRARY_DIRS% -DLIBUSB_LIBRARIES=%LIBUSB_LIBRARIES% -DPTHREAD_LIBRARY_DIRS=%LIBUSB_LIBRARY_DIRS% .
%make%
%make% install
CD %SRCDIR%
@ECHO "airspyone_host SDK installed, please take a look above for errors before continuing"
REM

:skipairspyone_host
SET /P yn=Build SoapyAirspy? (y/n): 
IF %yn% EQU n GOTO skipSoapyAirspy
IF %yn% EQU N GOTO skipSoapyAirspy
CD SoapyAirspy
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% .
%make%
%make% install
CD %SRCDIR%
@ECHO "SoapyAirspy support library installed, please take a look above for errors before continuing"
REM

:skipSoapyAirspy
SET /P yn=Build rtAudio? (y/n): 
IF %yn% EQU n GOTO skiprtAudio
IF %yn% EQU N GOTO skiprtAudio
CD rtAudio\_build_
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..
%make% VERBOSE=1
%make% install
CD %SRCDIR%
@ECHO "rtAudio SDK installed, please take a look above for errors before continuing"
REM

:skiprtAudio
SET /P yn=Build SoapyAudio? (y/n): 
IF %yn% EQU n GOTO skipSoapyAudio
IF %yn% EQU N GOTO skipSoapyAudio
CD SoapyAudio
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_PREFIX_PATH=%INSTALL_PREFIX% .
%make% 
%make% install
CD %SRCDIR%
@ECHO "SoapyAudio support library installed, please take a look above for errors before continuing"
REM

:skipSoapyAudio
SET /P yn=Build SoapyRemote? (y/n): 
IF %yn% EQU n GOTO skipSoapyRemote
IF %yn% EQU N GOTO skipSoapyRemote
CD SoapyRemote
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_PREFIX_PATH=%INSTALL_PREFIX% .
%make% -k
%make% install
CD %SRCDIR%
@ECHO "SoapyRemote support library installed, please take a look above for errors before continuing"
REM

:skipSoapyRemote
:end
CD %SRCDIR%
