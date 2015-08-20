#-------------------------------------------------
#
# Project created by QtCreator 2015-07-21T12:13:38
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = AllSteps
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    assert_t.cpp


#win32 {
#    INCLUDEPATH += C:/boost/include/boost-1_54
#    LIBS += -LC:/boost/lib \
#            -lboost_serialization-mgw46-mt-d-1_54 \
#            -lboost_filesystem-mgw46-mt-d-1_54 \
#            -lboost_system-mgw46-mt-d-1_54
#}

HEADERS += \
    stringt.hpp \
    intarray.hpp \
    exp.hpp
