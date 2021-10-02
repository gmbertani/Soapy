# ----------------------------------------------------------------
# CLEANALL
# cleans SoapySDR utilities, library and devices support libraries
# ----------------------------------------------------------------
set -x

INSTALL_PREFIX="/usr/lib64/SoapySDR/modules0.8" ;

SRCDIR=$PWD ;

#cd $SRCDIR/UNIXem/build/mingw730
#make clean
#cd $SRCDIR

cd $SRCDIR/SoapySDR/build ;
make clean ;
cd $SRCDIR/SoapySDR/tests ;
rm -fr CMakeFiles ;
rmdir CMakeFiles ;
rm -fr CMakeCache.* ;
cd $SRCDIR ;
rm -fr $SRCDIR/SoapySDR/build ;

cd $SRCDIR/librtlsdr ;
make clean ;
rm -fr CMakeFiles ;
rmdir CMakeFiles ;
rm -fr CMakeCache.* ;
cd $SRCDIR ;

cd $SRCDIR/SoapyRTLSDR ;
make clean ;
rm -fr CMakeFiles ;
rmdir CMakeFiles ;
rm -fr CMakeCache.* ;
cd $SRCDIR ;

cd $SRCDIR/airspyone_host ;
make clean ;
rm -fr CMakeFiles ;
rmdir CMakeFiles ;
rm -fr CMakeCache.* ;
cd $SRCDIR ;

cd $SRCDIR/SoapyAirspy ;
make clean ;
rm -fr CMakeFiles ;
rmdir CMakeFiles ;
rm -fr CMakeCache.* ;
cd $SRCDIR ;

# cd $SRCDIR/rtAudio ;
# make clean ;
# rm -fr  *.vcxproj ;
# rm -fr  *.filters ;
# rm -fr CMakeFiles ;
# rmdir CMakeFiles ;
# rm -fr  CMakeCache.* ;
# rm -fr  $SRCDIR/rtAudio-5.1.0/_build_/*.* ;
# rm -fr $SRCDIR/rtAudio/_build_/* ;
# rmdir $SRCDIR/rtAudio/_build_/CMakeFiles ;
# rmdir $SRCDIR/rtAudio/_build_/Testing ;
# cd $SRCDIR ;

cd $SRCDIR/SoapyAudio ;
make clean ;
rm -fr CMakeFiles ;
rmdir CMakeFiles ;
rm -fr CMakeCache.* ;
cd $SRCDIR ;

cd $SRCDIR/SoapyRemote ;
make clean ;
rm -fr CMakeFiles ;
rmdir CMakeFiles ;
rm -fr CMakeCache.* ;
cd $SRCDIR ;

sudo rm -i $INSTALL_PREFIX/* ;

echo "press a key to quit" ;
read x ;

