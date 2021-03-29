rem ----------------------------------------------------------------
rem BUILDALL
rem builds SoapySDR utilities, library and devices support libraries
rem ----------------------------------------------------------------
SET INSTALL_PREFIX="c:\program files\Soapy"

rem GOTO SKIP

rem builds the unixem library firs
cd UNIXem\build\mingw730
make clean
make
cd ..\..\..
rem install not needed

cd SoapySDR\build
rmdir /s /q CMakeFiles
del /q CMakeCache.*

echo WARNING: in case of error like "The C compiler gcc.exe is not able to compile a simple test program."
echo try to disable the antivirus for some minutes.

cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% ..
make clean
make
make install
cd ..\..


cd librtlsdr
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% .
make clean
make
make install
cd ..

cd SoapyRTLSDR
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% .
make clean
make
make install

cd ..

