#-------------------------------------------------
#
# Project created by QtCreator 2014-07-19T11:54:34
#
#-------------------------------------------------

QT       += core gui multimedia network sql multimediawidgets
linux {
    QT     += x11extras
    LIBS   +=  -lXext -lX11
}
mac {
    ICON = KylinPlayer.icns
}
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KylinPlayer
TEMPLATE = app


SOURCES += main.cpp\
        playermainwindow.cpp \
    network.cpp \
    database.cpp \
    osdlyricswidget.cpp

HEADERS  += playermainwindow.h \
    network.h \
    common.h \
    database.h \
    osdlyricswidget.h

FORMS    += playermainwindow.ui

RESOURCES += \
    resource.qrc
