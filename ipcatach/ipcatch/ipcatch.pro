TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        cipnode.cpp \
        cnodelist.cpp \
        main.cpp

HEADERS += \
    cipnode.h \
    cnodelist.h
LIBS += -lws2_32
