#-------------------------------------------------
#
# Project created by QtCreator 2025-09-22T10:29:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = yoloInfer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += $$PWD/backend
include($$PWD/backend/backend.pri)

INCLUDEPATH += E:\AAA24105033\tools\opencv\opencv\mingwbuild\install\include
LIBS += E:\AAA24105033\tools\opencv\opencv\mingwbuild\install\x64\mingw\lib\libopencv_*.a
LIBS += E:\AAA24105033\tools\opencv\opencv\mingwbuild\lib\libopencv_*.a

MNN_ROOT = E:\AAA24105033\tools

INCLUDEPATH +=$$MNN_ROOT/MNN/MNN/include\
$$MNN_ROOT/MNN/MNN/include/MNN\
$$MNN_ROOT/MNN/MNN/schema/current\
$$MNN_ROOT/MNN/MNN/tools\
$$MNN_ROOT/MNN/MNN/tools/cpp\
$$MNN_ROOT/MNN/MNN/source\
$$MNN_ROOT/MNN/MNN/source/backend\
$$MNN_ROOT/MNN/MNN/source/core\
$$MNN_ROOT/MNN/MNN/source/cv\
$$MNN_ROOT/MNN/MNN/source/math\
$$MNN_ROOT/MNN/MNN/source/shape\
$$MNN_ROOT/MNN/MNN/3rd_party\
$$MNN_ROOT/MNN/MNN/3rd_party/imageHelper

LIBS += -L$$MNN_ROOT\MNN\MNN\buildMinGW
LIBS += -lMNN


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
