# -------------------------------------------------
# Project created by QtCreator 2009-06-28T19:14:09
# -------------------------------------------------
QT += network
QT -= gui
TARGET = bottie
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    irc.cpp \
    lurker.cpp \
    chan.cpp \
    canigen.cpp
HEADERS += irc.h \
    lurker.h \
    chan.h \
    canigen.h
