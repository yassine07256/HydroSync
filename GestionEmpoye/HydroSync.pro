QT       += core gui widgets sql svg
CONFIG   += c++17
TARGET    = HydroSync
TEMPLATE  = app

# Chaînes UTF-8 (accents) avec MSVC
msvc: QMAKE_CXXFLAGS += /utf-8

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    homepage.cpp \
    employeepage.cpp \
    employeedialog.cpp \
    employee.cpp \
    employeemodel.cpp \
    employeefilterproxymodel.cpp \
    database.cpp \
    pdfexporter.cpp \
    charts.cpp \
    glasspanel.cpp \
    backgroundwidget.cpp \
    actiondelegate.cpp

HEADERS += \
    mainwindow.h \
    homepage.h \
    employeepage.h \
    employeedialog.h \
    employee.h \
    employeemodel.h \
    employeefilterproxymodel.h \
    database.h \
    pdfexporter.h \
    charts.h \
    glasspanel.h \
    backgroundwidget.h \
    actiondelegate.h

RESOURCES += resources.qrc
