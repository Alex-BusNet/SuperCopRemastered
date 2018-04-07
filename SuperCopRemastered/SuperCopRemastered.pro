#-------------------------------------------------
#
# Project created by QtCreator 2018-03-02T14:45:41
#
#-------------------------------------------------

QT       += core gui gamepad quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SuperCopRemastered
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    Game/supercoprmgame.cpp \
    Game/levelbase.cpp \
    Game/player.cpp \
    Game/blockbase.cpp \
    Game/bonusblock.cpp \
    Game/itembase.cpp \
    Game/enemybase.cpp \
    Game/gamescene.cpp \
    Game/gameview.cpp \
    Game/robocoprmgame.cpp

HEADERS += \
        mainwindow.h \
    Game/supercoprmgame.h \
    Game/levelbase.h \
    Game/player.h \
    Game/datatypes.h \
    Game/blockbase.h \
    Game/bonusblock.h \
    Game/itembase.h \
    Game/enemybase.h \
    Game/gamescene.h \
    Game/gameview.h \
    Game/robocoprmgame.h

FORMS += \
        mainwindow.ui
