# -------------------------------------------------
# Project created by QtCreator 2009-06-28T19:14:09
# -------------------------------------------------
QT += network
QT -= gui
TARGET = bottie
CONFIG += console \
    static
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    irc.cpp \
    lurker.cpp
HEADERS += irc.h \
    lurker.h
