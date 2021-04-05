rem ----------------------------------------------------------------
rem BUILDALL
rem builds SoapySDR utilities, library and devices support libraries
rem ----------------------------------------------------------------
SET INSTALL_PREFIX="c:\program files\Soapy"

rem GOTO SKIP

rem builds the unixem library firs
cd UNIXem\build\mingw730
make clean
cd ..\..\..

cd SoapySDR\build
rmdir /s /q CMakeFiles
del /q CMakeCache.*

make clean
cd ..\..

cd librtlsdr
rmdir /s /q CMakeFiles
del /q CMakeCache.*
make clean
cd ..

cd SoapyRTLSDR
rmdir /s /q CMakeFiles
del /q CMakeCache.*
make clean
cd ..


cd airspyone_host
rmdir /s /q CMakeFiles
del /q CMakeCache.*
make clean
cd ..


cd SoapyAirspy
rmdir /s /q CMakeFiles
del /q CMakeCache.*
make clean
cd ..



cd SoapyAudio
rmdir /s /q CMakeFiles
del /q CMakeCache.*
make clean



