rem ----------------------------------------------------------------
rem CLEANALL
rem cleans SoapySDR utilities, library and devices support libraries
rem ----------------------------------------------------------------
@echo on 

SET INSTALL_PREFIX="D:\Users\massimo\Documents\echoes-git\trunk\echoes\deps\Soapy"

SET make="D:\Qt\Tools\mingw730_64\bin\mingw32-make.exe"
SET SRCDIR=%CD%

rem builds the unixem library first
cd %SRCDIR%\UNIXem\build\mingw730
%make% clean
cd %SRCDIR%

cd %SRCDIR%\SoapySDR\build
%make% clean
cd %SRCDIR%\SoapySDR\tests
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd %SRCDIR%
del /q /s %SRCDIR%\SoapySDR\build

cd %SRCDIR%\librtlsdr
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd %SRCDIR%

cd %SRCDIR%\SoapyRTLSDR
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd %SRCDIR%

cd %SRCDIR%\airspyone_host
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd %SRCDIR%

cd %SRCDIR%\SoapyAirspy
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd %SRCDIR%

cd %SRCDIR%\rtAudio-5.1.0
%make% clean
DEL /q *.vcxproj
DEL /q *.filters
DEL /q *.cmake
RMDIR /s /q CMakeFiles
del /q CMakeCache.*
DEL /q %SRCDIR%\rtAudio-5.1.0\_build_\*.*
RMDIR /s /q %SRCDIR%\rtAudio-5.1.0\_build_\CMakeFiles
RMDIR /s /q %SRCDIR%\rtAudio-5.1.0\_build_\Testing
cd %SRCDIR%

cd %SRCDIR%\SoapyAudio
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*

cd %SRCDIR%
del /q /s *.cmake


pause

