#-------------------------------------------------
#
# Project created by QtCreator 2016-03-23T14:30:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += network


TARGET = FileShare
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    manager.cpp \
    sendfiledialog.cpp \
    fileserver.cpp \
    downloader.cpp \
    settingsdialog.cpp \
    chatform.cpp \
    sendbroadcastform.cpp

HEADERS  += mainwindow.h \
    manager.h \
    sharedfile.h \
    sendfiledialog.h \
    fileserver.h \
    downloader.h \
    settingsdialog.h \
    chatform.h \
    sendbroadcastform.h

FORMS += \
    sendfiledialog.ui \
    settingsdialog.ui \
    chatform.ui \
    sendbroadcastform.ui

RESOURCES += \
    myresource.qrc


OTHER_FILES += \
    icon.rc
RC_FILE += icon.rc

