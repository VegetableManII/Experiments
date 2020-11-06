TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

HEADERS += \
    Server.h \
    def.h
LIBS += -lws2_32 -lIPHLPAPI
