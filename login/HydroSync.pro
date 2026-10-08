QT += core gui widgets

CONFIG += c++17

# Évite les problèmes d'accents avec le compilateur Microsoft
msvc: QMAKE_CXXFLAGS += /utf-8

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

RESOURCES += \
    resources.qrc
