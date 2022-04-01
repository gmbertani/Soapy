TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOAPY_INSTALL_PATH = $$quote("d:\Users\massimo\Documents\echoes-git\trunk\echoes\deps\x86_64\Soapy")

INCLUDEPATH += $$SOAPY_INSTALL_PATH/include
LIBS += -L$$SOAPY_INSTALL_PATH/bin -lSoapySDR


SOURCES = \
        main.cpp


