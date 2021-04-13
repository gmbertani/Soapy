rem ----------------------------------------------------------------
rem BUILDALL
rem builds SoapySDR utilities, library and devices support libraries
rem ----------------------------------------------------------------
@echo off 

SET INSTALL_PREFIX="d:\program files\Soapy"
rem rmdir /s /q %INSTALL_PREFIX%
rem GOTO SKIP

rem builds the unixem library first
cd UNIXem\build\mingw730
make clean
cd ..\..\..

cd SoapySDR\build
make clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd ..\..

cd librtlsdr
make clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd ..

cd SoapyRTLSDR
make clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd ..

cd airspyone_host
make clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd ..

cd SoapyAirspy
make clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd ..



cd SoapyAudio
make clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*


pause

