TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


#INCLUDEPATH += $$quote("d:\Users\massimo\Documents\sviluppo\echoes-related\soapy\SoapySDR\include")
#LIBS += -L$$quote("d:\Users\massimo\Documents\sviluppo\echoes-related\soapy\SoapySDR\build\lib") -lSoapySDR

INCLUDEPATH += $$quote("d:\program files\PothosSDR\include")
LIBS += -L$$quote("d:\program files\PothosSDR\bin") -lSoapySDR



SOURCES = \
        main.c \

