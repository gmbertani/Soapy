REM ----------------------------------------------------------------
REM BUILDALL
REM builds SoapySDR utilities, library and devices support libraries
REM ----------------------------------------------------------------
@ECHO off
SET INSTALL_PREFIX="D:\program files\Soapy"
SET make="D:\Qt\Tools\mingw730_64\bin\mingw32-make.exe"

REM for laziness, the libusb provided is the static library with its header
REM so there is no .pc file and CMake is not able to detect it implicitly
SET LIBUSB_INCLUDE_DIR="D:\Users\massimo\Documents\sviluppo\ECHOes-related\soapy\deps\include"
SET LIBUSB_LIBRARY_DIRS="D:\Users\massimo\Documents\sviluppo\ECHOes-related\soapy\deps\lib64"
SET LIBUSB_LIBRARIES="libusb-1.0.a"

REM builds the unixem library first
SET /P yn=Build UNIXem? (y/n): 
IF %yn% EQU n GOTO skipUNIXem

CD UNIXem\build\mingw730
%make% clean
%make%
CD ..\..\..
REM install not needed
@ECHO "UNIXem done, please take a look above for errors before continuing"
REM

:skipUNIXem
SET /P yn=Build SoapySDR? (y/n): 
IF %yn% EQU n GOTO skipSoapySDR

CD SoapySDR\build
%make% clean
RMDIR /s /q CMakeFiles
DEL /q CMakeCache.*

ECHO WARNING: in case of error like "The C compiler gcc.exe is not able to compile a simple test program."
ECHO try to disable the antivirus for some minutes.
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX%  ..
%make% 
%make% install
CD ..\..
@ECHO "SoapySDR installed, please take a look above for errors before continuing"
REM

:skipSoapySDR
SET /P yn=Build librtlsdr? (y/n): 
IF %yn% EQU n GOTO skiplibrtlsdr

CD librtlsdr
%make% clean
RMDIR /s /q CMakeFiles
DEL /q CMakeCache.*
cd tests
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd ..
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DLIBUSB_INCLUDE_DIR=%LIBUSB_INCLUDE_DIR% -DLIBUSB_LIBRARY_DIRS=%LIBUSB_LIBRARY_DIRS% -DLIBUSB_LIBRARIES=%LIBUSB_LIBRARIES% .
%make% VERBOSE=1
%make% install
CD ..
@ECHO "librtlsdr SDK installed, please take a look above for errors before continuing"
REM

:skiplibrtlsdr
SET /P yn=Build SoapyRTLSDR? (y/n): 
IF %yn% EQU n GOTO skipSoapyRTLSDR

CD SoapyRTLSDR
%make% clean
RMDIR /s /q CMakeFiles
DEL /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% .
%make%
%make% install
CD ..
@ECHO "SoapyRTLSDR support library installed, please take a look above for errors before continuing"
REM

:skipSoapyRTLSDR
SET /P yn=Build airspyone_host? (y/n): 
IF %yn% EQU n GOTO skipairspyone_host

CD airspyone_host
%make% clean
RMDIR /s /q CMakeFiles
DEL /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DLIBUSB_INCLUDE_DIR=%LIBUSB_INCLUDE_DIR% -DLIBUSB_LIBRARY_DIRS=%LIBUSB_LIBRARY_DIRS% -DLIBUSB_LIBRARIES=%LIBUSB_LIBRARIES% .
%make%
%make% install
CD ..
@ECHO "airspyone_host SDK installed, please take a look above for errors before continuing"
REM

:skipairspyone_host
SET /P yn=Build SoapyAirspy? (y/n): 
IF %yn% EQU n GOTO skipSoapyAirspy


CD SoapyAirspy
%make% clean
RMDIR /s /q CMakeFiles
DEL /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% .
%make%
%make% install
CD ..
@ECHO "SoapyAirspy support library installed, please take a look above for errors before continuing"
REM

:skipSoapyAirspy
SET /P yn=Build rtAudio? (y/n): 
IF %yn% EQU n GOTO skiprtAudio

CD rtAudio-5.1.0
%make% clean
DEL /q *.vcxproj
DEL /q *.filters
DEL /q *.cmake
RMDIR /s /q CMakeFiles
del /q CMakeCache.*
CD _build_
DEL /q *.*
RMDIR /s /q CMakeFiles
RMDIR /s /q Testing
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% ..
%make% VERBOSE=1
%make% install
CD ..\..
@ECHO "rtAudio SDK installed, please take a look above for errors before continuing"
REM

:skiprtAudio
SET /P yn=Build SoapyAudio? (y/n): 
IF %yn% EQU n GOTO skipSoapyAudio

CD SoapyAudio
%make% clean
RMDIR /s /q CMakeFiles
DEL /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_PREFIX_PATH=%INSTALL_PREFIX% .
%make%
%make% install
CD ..
@ECHO "SoapyAudio support library installed, please take a look above for errors before continuing"
REM

:skipSoapyAudio

