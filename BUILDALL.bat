rem ----------------------------------------------------------------
rem BUILDALL
rem builds SoapySDR utilities, library and devices support libraries
rem ----------------------------------------------------------------

SET INSTALL_PREFIX="c:\program files\Soapy"

rem for laziness, the libusb provided is the static library with its header
rem so there is no .pc file and CMake is not able to detect it implicitly
SET LIBUSB_INCLUDE_DIR="D:\Users\massimo\Documents\sviluppo\echoes-related\soapy\deps\include"
SET LIBUSB_LIBRARY_DIRS="D:\Users\massimo\Documents\sviluppo\echoes-related\soapy\deps\lib64"
SET LIBUSB_LIBRARIES="libusb-1.0.a"
GOTO SKIP

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

cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX%  ..
make clean
make 
make install
cd ..\..


cd librtlsdr
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DLIBUSB_INCLUDE_DIR=%LIBUSB_INCLUDE_DIR% -DLIBUSB_LIBRARY_DIRS=%LIBUSB_LIBRARY_DIRS% -DLIBUSB_LIBRARIES=%LIBUSB_LIBRARIES% .
make clean
make VERBOSE=1
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


cd airspyone_host
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DLIBUSB_INCLUDE_DIR=%LIBUSB_INCLUDE_DIR% -DLIBUSB_LIBRARY_DIRS=%LIBUSB_LIBRARY_DIRS% -DLIBUSB_LIBRARIES=%LIBUSB_LIBRARIES% .
make clean
make
make install
cd ..



cd SoapyAirspy
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% .
make clean
make
make install
cd ..


:SKIP


cd SoapyAudio
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%INSTALL_PREFIX% -DCMAKE_PREFIX_PATH=%INSTALL_PREFIX% .
make clean
make
make install

GOTO END 

:END

cd ..