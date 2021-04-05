TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

#INCLUDEPATH += $$quote("d:\program files\PothosSDR\include")
#LIBS += -L$$quote("d:\program files\PothosSDR\bin") -lSoapySDR

INCLUDEPATH += $$quote("C:\program files\Soapy\include")
LIBS += -L$$quote("c:\program files\Soapy\bin") -lSoapySDR

SOURCES = \
        main.cpp


