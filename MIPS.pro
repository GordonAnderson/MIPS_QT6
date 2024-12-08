#-------------------------------------------------
#
# Project created by QtCreator 2015-06-27T10:54:52
#
#-------------------------------------------------

QT       += core
QT       += gui
QT       += serialport
QT       += network
QT       += widgets printsupport
lessThan(QT_MAJOR_VERSION, 6):    QT += script
#greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat
greaterThan(QT_MAJOR_VERSION, 5): QT += qml
QT       += serialbus serialport widgets

win32:RC_ICONS += GAACElogo.ico
ICON = GAACElogo.icns

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MIPS
TEMPLATE = app

CONFIG += static

SOURCES += main.cpp\
    Utilities.cpp \
        mips.cpp \
        console.cpp \
    modbus.cpp \
        settingsdialog.cpp \
        ringbuffer.cpp \
        pse.cpp \
        comms.cpp \
        twave.cpp \
    dcbias.cpp \
    dio.cpp \
    rfdriver.cpp \
    psg.cpp \
    program.cpp \
    help.cpp \
    arb.cpp \
    faims.cpp \
    filament.cpp \
    arbwaveformedit.cpp \
    qcustomplot.cpp \
    psviewer.cpp \
    adc.cpp \
    controlpanel.cpp \
    mipscomms.cpp \
    cmdlineapp.cpp \
    cdirselectiondlg.cpp \
    scriptingconsole.cpp \
    rfamp.cpp \
    tcpserver.cpp \
    timinggenerator.cpp \
    compressor.cpp \
    properties.cpp \
    plot.cpp \
    device.cpp

HEADERS  += mips.h \
    Utilities.h \
    console.h \
    modbus.h \
    settingsdialog.h \
    ringbuffer.h \
    pse.h \
    comms.h \
    twave.h \
    dcbias.h \
    dio.h \
    rfdriver.h \
    psg.h \
    program.h \
    help.h \
    arb.h \
    faims.h \
    filament.h \
    arbwaveformedit.h \
    qcustomplot.h \
    psviewer.h \
    adc.h \
    controlpanel.h \
    mipscomms.h \
    cmdlineapp.h \
    cdirselectiondlg.h \
    scriptingconsole.h \
    rfamp.h \
    tcpserver.h \
    timinggenerator.h \
    compressor.h \
    properties.h \
    plot.h \
    device.h

FORMS    += mips.ui \
    settingsdialog.ui \
    pse.ui \
    help.ui \
    arbwaveformedit.ui \
    psviewer.ui \
    controlpanel.ui \
    mipscomms.ui \
    cmdlineapp.ui \
    scriptingconsole.ui \
    rfamp.ui \
    timinggenerator.ui \
    compressor.ui \
    properties.ui \
    plot.ui

RESOURCES += \
    files.qrc

QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64

CONFIG+=sdk_no_version_check
#QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
#QMAKE_CXXFLAGS *= "-Xpreprocessor -fopenmp"
#QMAKE_MAC_SDK = macosx10.12
MAKE_MAC_SDK = macosx
#!host_build:QMAKE_MAC_SDK = macosx


#QMAKE_MAC_SDK = macosx10.12







