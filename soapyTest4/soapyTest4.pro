TEMPLATE = app
CONFIG += console c99
CONFIG -= app_bundle
CONFIG -= qt


INCLUDEPATH += $$quote("d:\program files\PothosSDR\include")
LIBS += -L$$quote("d:\program files\PothosSDR\lib")
LIBS += -lSoapySDR

SOURCES = \
        main.c \

