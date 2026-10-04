QT       += core gui widgets

CONFIG   += c++17

TARGET = StreamUFSC
TEMPLATE = app

# Suprime avisos antigos do Qt (nao afeta funcionamento)
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    movie.cpp \
    theme.cpp \
    startwindow.cpp \
    mainwindow.cpp \
    addmoviewindow.cpp \
    aboutwindow.cpp

HEADERS += \
    movie.h \
    theme.h \
    startwindow.h \
    mainwindow.h \
    addmoviewindow.h \
    aboutwindow.h

FORMS += \
    startwindow.ui \
    mainwindow.ui \
    addmoviewindow.ui \
    aboutwindow.ui

RESOURCES += \
    resources.qrc
