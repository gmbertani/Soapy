TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

#SOAPY_INSTALL_PATH = $$quote("d:\Users\massimo\Documents\echoes-git\trunk\echoes\deps\x86_64\Soapy")

INCLUDEPATH += "D:/program files/PothosSDR/include"
LIBS += "D:/program files/PothosSDR/lib/SoapySDR.lib"


SOURCES = \
        main.cpp


