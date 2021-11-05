rem ----------------------------------------------------------------
rem CLEANALL
rem cleans SoapySDR utilities, library and devices support libraries
rem ----------------------------------------------------------------
@echo on 

REM SET INSTALL_PREFIX="D:\Users\massimo\Documents\echoes-git\trunk\echoes\deps\x86_64\Soapy"
REM SET INSTALL_PREFIX="C:\temp\radio\echoes-git\trunk\echoes\deps\x86_32\Soapy"
SET INSTALL_PREFIX="C:\Users\Massimo\Documents\echoes-git\trunk\echoes\deps\x86_64\Soapy"

REM SET make="D:\Qt\Tools\mingw730_64\bin\mingw32-make.exe"
REM SET make="C:\Qt\Tools\mingw730_32\bin\mingw32-make.exe"
SET make="C:\Qt\Tools\mingw730_64\bin\mingw32-make.exe"

SET SRCDIR=%CD%

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

cd %SRCDIR%\rtAudio
%make% clean
DEL /q *.vcxproj
DEL /q *.filters
RMDIR /s /q CMakeFiles
del /q CMakeCache.*
DEL /q %SRCDIR%\rtAudio-5.1.0\_build_\*.*
RMDIR /s /q %SRCDIR%\rtAudio\_build_\CMakeFiles
RMDIR /s /q %SRCDIR%\rtAudio\_build_\Testing
cd %SRCDIR%

cd %SRCDIR%\SoapyAudio
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd %SRCDIR%

cd %SRCDIR%\SoapyRemote
%make% clean
rmdir /s /q CMakeFiles
del /q CMakeCache.*
cd %SRCDIR%

pause

