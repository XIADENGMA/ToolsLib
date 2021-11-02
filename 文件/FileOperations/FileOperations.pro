TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH +=/usr/local/include/
INCLUDEPATH +=/usr/local/include/opencv4/
INCLUDEPATH +=/usr/local/include/opencv4/opencv2/
LIBS += /usr/local/lib/*.so

SOURCES += \
        FileOperations.cpp \
        main.cpp

HEADERS += \
    FileOperations.h
