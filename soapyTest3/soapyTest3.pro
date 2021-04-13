TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += /wo4251

INCLUDEPATH += $$quote("d:\program files\PothosSDR\include")
LIBS += -L$$quote("d:\program files\PothosSDR\lib")
LIBS += -lSoapySDR

SOURCES = \
        main.cpp


