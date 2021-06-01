REM -----------------------------------------------------------------------------
REM UPDATE
REM Copies the source files from original directories and copies them on my repos
REM -----------------------------------------------------------------------------
@ECHO off
SET ORIGINAL_REPOS=..\officialRepos
SET MY_REPOS=.
SET COPY=XCOPY /Y /F /E /D

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

ECHO applying patches
SET COPY=COPY /V /Y 
%COPY% %MY_REPOS%\airspyone_host\CMakeLists.txt.GMB %MY_REPOS%\airspyone_host\CMakeLists.txt
%COPY% %MY_REPOS%\librtlsdr\CMakeLists.txt.GMB      %MY_REPOS%\librtlsdr\CMakeLists.txt
%COPY% %MY_REPOS%\SoapyAirspy\CMakeLists.txt.GMB    %MY_REPOS%\SoapyAirspy\CMakeLists.txt
%COPY% %MY_REPOS%\SoapyAudio\CMakeLists.txt.GMB     %MY_REPOS%\SoapyAudio\CMakeLists.txt
%COPY% %MY_REPOS%\SoapyRTLSDR\CMakeLists.txt.GMB    %MY_REPOS%\SoapyRTLSDR\CMakeLists.txt
%COPY% %MY_REPOS%\SoapySDR\CMakeLists.txt.GMB       %MY_REPOS%\SoapySDR\CMakeLists.txt
%COPY% %MY_REPOS%\SoapySDR\lib\LoggerC.cpp.GMB      %MY_REPOS%\SoapySDR\lib\LoggerC.cpp


ECHO done

:end
