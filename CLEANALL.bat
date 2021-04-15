rem ----------------------------------------------------------------
rem CLEANALL
rem cleans SoapySDR utilities, library and devices support libraries
rem ----------------------------------------------------------------
@echo off 

SET INSTALL_PREFIX="d:\program files\Soapy"
rem rmdir /s /q %INSTALL_PREFIX%
rem GOTO SKIP

SET make="D:\Qt\Tools\mingw730_64\bin\mingw32-make.exe"

rem builds the unixem library first
cd UNIXem\build\mingw730
%make% clean
cd ..\..\..

cd SoapySDR\build
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd tests
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd ..
cd ..\..

cd librtlsdr
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd ..

cd SoapyRTLSDR
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd ..

cd airspyone_host
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd ..

cd SoapyAirspy
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd ..

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
cd ..\..

cd SoapyAudio
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*


pause

