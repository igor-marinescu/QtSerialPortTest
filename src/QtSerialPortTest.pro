#-------------------------------------------------
#
# Project created by QtCreator 2013-11-27T12:10:14
#
#-------------------------------------------------

QT      +=  core gui
QT      +=  serialport
QT      +=  network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32:RC_FILE = QtSerialPortTest.rc

TARGET = QtSerialPortTest
TEMPLATE = app

#debug (see debug_cout.h)
DEFINES += DEBUGCOUT
DEFINES += DEBUGON

SOURCES +=  main.cpp\
            mainwindow.cpp \
            ConfFile.cpp \
            AppInterface.cpp \
            debug_cout.cpp \
            Serial.cpp \
            utils.cpp \
            SettingsDialog.cpp \
            ComInterface.cpp \
            udp.cpp \
            output.cpp \
            options.cpp \
            input.cpp

HEADERS  += mainwindow.h \
            ConfFile.h \
            AppInterface.h \
            debug_cout.h \
            Serial.h \
            utils.h \
            SettingsDialog.h \
            ComInterface.h \
            udp.h \
            output.h \
            options.h \
            input.h

FORMS    += mainwindow.ui \
            settingsdialog.ui

OTHER_FILES += \
            QtSerialPortTest.rc
