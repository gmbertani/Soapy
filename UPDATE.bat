REM -----------------------------------------------------------------------------
REM UPDATE
REM Copies the source files from original directories and copies them on my repos
REM -----------------------------------------------------------------------------
@ECHO off
SET ORIGINAL_REPOS=..\officialRepos
SET MY_REPOS=%CD%
SET COPY=XCOPY /Y /F /E /D

ECHO updating repositories...
CD %ORIGINAL_REPOS%
cd airspyhf
git pull origin master
cd ..
cd host
git pull origin master
cd ..
cd librtlsdr
git pull origin master
cd ..
cd LimeSuite
git pull origin master
cd ..
cd LMS7002M-driver
git pull origin master
cd ..
cd Novena-RF
git pull origin master
cd ..
cd rtaudio
git pull origin master
cd ..
cd SoapyAirspy
git pull origin master
cd ..
cd SoapyAirspyHF
git pull origin master
cd ..
cd SoapyAudio
git pull origin master
cd ..
cd SoapyBladeRF
git pull origin master
cd ..
cd SoapyFCDPP
git pull origin master
cd ..
cd SoapyHackRF
git pull origin master
cd ..
cd SoapyMultiSDR
git pull origin master
cd ..
cd SoapyNetSDR
git pull origin master
cd ..
cd SoapyOsmo
git pull origin master
cd ..
cd SoapyPlutoSDR
git pull origin master
cd ..
cd SoapyRedPitaya
git pull origin master
cd ..
cd SoapyRemote
git pull origin master
cd ..
cd SoapyRTLSDR
git pull origin master
cd ..
cd SoapySDR
git pull origin master
cd ..
cd SoapySDRPlay3
git pull origin master
cd ..
cd SoapySidekiq
git pull origin master
cd ..
cd SoapyUHD
git pull origin master
cd ..
CD %MY_REPOS%

ECHO updating sources...
%COPY% %ORIGINAL_REPOS%\airspyhf %MY_REPOS%\airspyhf
%COPY% %ORIGINAL_REPOS%\host %MY_REPOS%\airspyone_host
%COPY% %ORIGINAL_REPOS%\librtlsdr %MY_REPOS%\librtlsdr
%COPY% %ORIGINAL_REPOS%\rtaudio %MY_REPOS%\rtaudio
%COPY% %ORIGINAL_REPOS%\SoapyAirspy %MY_REPOS%\SoapyAirspy
%COPY% %ORIGINAL_REPOS%\SoapyAirspyHF %MY_REPOS%\SoapyAirspyHF
%COPY% %ORIGINAL_REPOS%\SoapyAudio %MY_REPOS%\SoapyAudio
%COPY% %ORIGINAL_REPOS%\SoapyBladeRF %MY_REPOS%\SoapyBladeRF
%COPY% %ORIGINAL_REPOS%\SoapyFCDPP %MY_REPOS%\SoapyFCDPP
%COPY% %ORIGINAL_REPOS%\SoapyHackRF %MY_REPOS%\SoapyHackRF
%COPY% %ORIGINAL_REPOS%\SoapyMultiSDR %MY_REPOS%\SoapyMultiSDR
%COPY% %ORIGINAL_REPOS%\SoapyPlutoSDR %MY_REPOS%\SoapyPlutoSDR
%COPY% %ORIGINAL_REPOS%\SoapyRTLSDR %MY_REPOS%\SoapyRTLSDR
%COPY% %ORIGINAL_REPOS%\SoapySDR %MY_REPOS%\SoapySDR
%COPY% %ORIGINAL_REPOS%\SoapySDRPlay3 %MY_REPOS%\SoapySDRPlay3
%COPY% %ORIGINAL_REPOS%\SoapyUHD %MY_REPOS%\SoapyUHD
%COPY% %ORIGINAL_REPOS%\SoapyRedPitaya %MY_REPOS%\SoapyRedPitaya
%COPY% %ORIGINAL_REPOS%\SoapySidekiq %MY_REPOS%\SoapySidekiq
%COPY% %ORIGINAL_REPOS%\SoapyOsmo %MY_REPOS%\SoapyOsmo
%COPY% %ORIGINAL_REPOS%\SoapyNetSDR %MY_REPOS%\SoapyNetSDR
%COPY% %ORIGINAL_REPOS%\Novena-RF %MY_REPOS%\Novena-RF
%COPY% %ORIGINAL_REPOS%\LMS7002M-driver %MY_REPOS%\LMS7002M-driver
%COPY% %ORIGINAL_REPOS%\LimeSuite %MY_REPOS%\LimeSuite


ECHO applying patches
SET COPY=COPY /V /Y 
%COPY% %MY_REPOS%\airspyone_host\CMakeLists.txt.GMB %MY_REPOS%\airspyone_host\CMakeLists.txt
%COPY% %MY_REPOS%\librtlsdr\CMakeLists.txt.GMB      %MY_REPOS%\librtlsdr\CMakeLists.txt
%COPY% %MY_REPOS%\SoapyAirspy\CMakeLists.txt.GMB    %MY_REPOS%\SoapyAirspy\CMakeLists.txt
%COPY% %MY_REPOS%\SoapyAudio\CMakeLists.txt.GMB     %MY_REPOS%\SoapyAudio\CMakeLists.txt

%COPY% %MY_REPOS%\SoapyRemote\client\LogAcceptor.cpp.GMB            %MY_REPOS%\SoapyRemote\client\LogAcceptor.cpp
%COPY% %MY_REPOS%\SoapyRemote\common\SoapyIfAddrsWindows.cpp.GMB    %MY_REPOS%\SoapyRemote\common\SoapyIfAddrsWindows.cpp
%COPY% %MY_REPOS%\SoapyRemote\common\SoapyInfoUtils.in.cpp.GMB      %MY_REPOS%\SoapyRemote\common\SoapyInfoUtils.in.cpp
%COPY% %MY_REPOS%\SoapyRemote\common\SoapyRPCSocket.cpp.GMB         %MY_REPOS%\SoapyRemote\common\SoapyRPCSocket.cpp
%COPY% %MY_REPOS%\SoapyRemote\common\SoapySocketDefs.in.hpp.GMB     %MY_REPOS%\SoapyRemote\common\SoapySocketDefs.in.hpp


%COPY% %MY_REPOS%\SoapyRTLSDR\CMakeLists.txt.GMB    %MY_REPOS%\SoapyRTLSDR\CMakeLists.txt
%COPY% %MY_REPOS%\SoapySDR\CMakeLists.txt.GMB       %MY_REPOS%\SoapySDR\CMakeLists.txt
%COPY% %MY_REPOS%\SoapySDR\lib\LoggerC.cpp.GMB      %MY_REPOS%\SoapySDR\lib\LoggerC.cpp
%COPY% %MY_REPOS%\SoapySDR\lib\Modules.in.cpp.GMB      %MY_REPOS%\SoapySDR\lib\Modules.in.cpp

copy %MY_REPOS%\UNIXem\build\mingw730\makefile.GMB   %MY_REPOS%\UNIXem\build\mingw730\makefile
copy %MY_REPOS%\UNIXem\src\dlfcn.c.GMB   %MY_REPOS%\UNIXem\src\dlfcn.c

ECHO done

:end
