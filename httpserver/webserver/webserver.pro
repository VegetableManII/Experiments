TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        chttpprotocol.cpp \
        main.cpp
LIBS += -lws2_32 -lIPHLPAPI

HEADERS += \
    chttpprotocol.h
