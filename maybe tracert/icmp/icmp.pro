TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        byip.cpp

HEADERS += \
    Def.h
LIBS += -lws2_32 -lIPHLPAPI
