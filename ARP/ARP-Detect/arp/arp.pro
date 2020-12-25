TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    arpDetect.cpp

LIBS += -lws2_32 -lIPHLPAPI

HEADERS += \
    ipTable.h
