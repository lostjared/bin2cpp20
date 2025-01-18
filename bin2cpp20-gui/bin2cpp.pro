QT += core gui
greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

# We want to use C++20 features
CONFIG += c++20

TEMPLATE = app
TARGET = bin2cpp

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    worker.cpp

HEADERS += \
    mainwindow.h \
    worker.h
