TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt c++11

#SOAPY_INSTALL_PATH = $$quote("D:/program files/Soapy")
SOAPY_INSTALL_PATH = $$quote("d:\Users\massimo\Documents\echoes-git\trunk\echoes\deps\x86_64\Soapy")

INCLUDEPATH += $$SOAPY_INSTALL_PATH/include
LIBS += -L$$SOAPY_INSTALL_PATH/bin -lSoapySDR

SOURCES = \
        main.cpp


