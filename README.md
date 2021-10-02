# Soapy
SoapySDR distribution patched for build with MingGW under Windows (no MSYS neither Cygwin needed)

##Notes
1. this is WORK IN PROGRESS. It aims to integrate Echoes (https://sourceforge.net/projects/echoes)
with SoapySDR (https://github.com/pothosware/SoapySDR/wiki) and could even be left incomplete. 

It successfully builds and install in my Win10 pc by launching BUILDALL.bat that builds and installs soapySDR dll, its utilities
and the drivers dll (only rtlsdr/audio/airspy currently... work in progress!).

To try it, remember to adjust the paths on the top of BUILDALL.bat first to your environment. On my PC I develop on
drive D: and you probably don't.

2. The libUSB binary and libpthread present under deps/ has been compiled under MSYS2/mingw 64bit

3. the UNIXem library supplies the glob and dlfnc apis missing in mingw

4. my patches have been saved in files with GMB extension. To update the sources, overwrite with the original ones
then overwrite/merge with the patched files.

5. the \*.sh shell scripts are needed to build under Linux. In this case there is no need to patch the original files.
Oct2,2021
