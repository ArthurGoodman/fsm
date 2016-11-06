TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    fsm.cpp

HEADERS += \
    fsm.h

QMAKE_CXXFLAGS += -MD
