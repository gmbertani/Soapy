TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOAPY_INSTALL_PATH = $$quote("D:/program files/Soapy")
INCLUDEPATH += $$SOAPY_INSTALL_PATH/include
LIBS += -L$$SOAPY_INSTALL_PATH/bin -lSoapySDR

SOURCES = \
        main.cpp


