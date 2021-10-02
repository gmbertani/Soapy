#!/bin/sh
# ----------------------------------------------------------------
# BUILD
# builds SoapySDR utilities, library and devices support libraries
# ----------------------------------------------------------------
INSTALL_PREFIX="/usr/lib64/SoapySDR/modules0.8" 
LIBUSB_INCLUDE_DIR="/usr/include/libusb-1.0"
LIBUSB_LIBRARY_DIRS="/usr/lib64"
LIBUSB_LIBRARIES="libusb-1.0.so" ;
BUILD_TYPE="Release" ;
SRCDIR=$PWD ;
# ----------------------------------------------------------------


function yes_or_no() {
    read -p "$1 ([y]es or [N]o): "
    case $(echo $REPLY | tr '[A-Z]' '[a-z]') in
        y|yes) echo "yes" ;;
        *)     echo "no" ;;
    esac
}


if [[ "yes" == $(yes_or_no "Build SoapySDR? (y/n):") ]] ; then 
    mkdir SoapySDR/build ;
    cd SoapySDR/build ;
    cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX  -DCMAKE_BUILD_TYPE=$BUILD_TYPE .. ;
    make ;
    make install ;
    cd $SRCDIR ;
    echo "SoapySDR installed, please take a look above for errors before continuing" ;
fi


if [[ "yes" == $(yes_or_no "Build librtlsdr? (y/n):") ]] ; then 
    cd librtlsdr ;
    cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DLIBUSB_INCLUDE_DIR=$LIBUSB_INCLUDE_DIR -DLIBUSB_LIBRARY_DIRS=$LIBUSB_LIBRARY_DIRS -DLIBUSB_LIBRARIES=$LIBUSB_LIBRARIES . ;
    make VERBOSE=1 ;
    make install ;
    cd $SRCDIR ;
echo "librtlsdr SDK installed, please take a look above for errors before continuing" ;
fi

if [[ "yes" == $(yes_or_no "Build SoapyRTLSDR? (y/n):") ]] ; then 
    cd SoapyRTLSDR ;
    cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX -DCMAKE_BUILD_TYPE=$BUILD_TYPE . ;
    make ;
    make install ;
    cd $SRCDIR ;
    echo "SoapyRTLSDR support library installed, please take a look above for errors before continuing" ;
fi


if [[ "yes" == $(yes_or_no "Build airspyone_host? (y/n):") ]] ; then 
    cd airspyone_host ;
    cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DLIBUSB_INCLUDE_DIR=$LIBUSB_INCLUDE_DIR -DLIBUSB_LIBRARY_DIRS=$LIBUSB_LIBRARY_DIRS -DLIBUSB_LIBRARIES=$LIBUSB_LIBRARIES -DPTHREAD_LIBRARY_DIRS=$LIBUSB_LIBRARY_DIRS . ;
    make ;
    make install ;
    cd $SRCDIR ;
    echo "airspyone_host SDK installed, please take a look above for errors before continuing" ;
fi

if [[ "yes" == $(yes_or_no "Build SoapyAirspy? (y/n):") ]] ; then 
    cd SoapyAirspy ;
    cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX -DCMAKE_BUILD_TYPE=$BUILD_TYPE . ;
    make ;
    make install ;
    cd $SRCDIR ;
    echo "SoapyAirspy support library installed, please take a look above for errors before continuing" ;
fi

# if [[ "yes" == $(yes_or_no "Build rtAudio? (y/n):") ]] ; then 
#     cd rtAudio/_build_ ;
#     cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX -DCMAKE_BUILD_TYPE=$BUILD_TYPE .. ;
#     make VERBOSE=1 ;
#     make install ;
#     cd $SRCDIR ;
#     echo "rtAudio SDK installed, please take a look above for errors before continuing" ;
# fi

if [[ "yes" == $(yes_or_no "Build SoapyAudio? (y/n):") ]] ; then 
    cd SoapyAudio ;
    cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_PREFIX_PATH=$INSTALL_PREFIX . ;
    make ;
    make install ;
    cd $SRCDIR ;
    echo "SoapyAudio support library installed, please take a look above for errors before continuing" ;
fi


if [[ "yes" == $(yes_or_no "Build SoapyRemote? (y/n):") ]] ; then 
    cd SoapyRemote ;
    cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_PREFIX_PATH=$INSTALL_PREFIX . ;
    make -k ;
    make install ;
    cd $SRCDIR ;
    echo "SoapyRemote support library installed, please take a look above for errors before continuing" ;
fi

cd $SRCDIR ;
