#-------------------------------------------------
#
# Project created by QtCreator 2019-02-02T13:43:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MungPlex
TEMPLATE = app
INCLUDEPATH += $$PWD/extern_resources
LIBS += -L$$PWD/extern_resources -llua53

#QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    search.cpp \
    memoryviewer.cpp \
    cheats.cpp \
    conversionwindow.cpp \
    pointersearch.cpp \
    processselect.cpp \
    processlist.cpp

HEADERS += \
        mainwindow.h \
    hook.h \
    memregion.h \
    search.h \
    rangelayout.h \
    processinfo.h \
    memoryviewer.h \
    cheats.h \
    lookup.h \
    dataconversion.h \
    conversionwindow.h \
    settings.h \
    pointersearch.h \
    processselect.h \
    processlist.h \
    operativearray.h

FORMS += \
        mainwindow.ui \
    memoryviewer.ui \
    conversionwindow.ui \
    processlist.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    notes.txt \
    gamelists\gamecube.xml \
    gamelists\wii.xml \
    gamelists\wiiware.xml \
    ToDo.txt

