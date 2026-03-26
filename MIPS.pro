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
DEFINES += APP_VERSION=\"2.23\"
win32:RC_ICONS += GAACElogo.ico
ICON = GAACElogo.icns
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = MIPS
TEMPLATE = app
CONFIG += shared
SOURCES += main.cpp connection.cpp fileops.cpp\
    Utilities.cpp \
    aboutdialog.cpp \
    dcbgroups.cpp \
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
    timinggenerator.cpp acquiredata.cpp timingcontrol.cpp eventcontrol.cpp \
    compressor.cpp \
    properties.cpp \
    plot.cpp \
    device.cpp \
    zmqworker.cpp \
    TextLabel.cpp Shutdown.cpp SaveLoad.cpp CPbutton.cpp DACchannel.cpp ESI.cpp \
    Ccontrol.cpp Cpanel.cpp StatusLight.cpp TextMessage.cpp Table.cpp Slider.cpp
HEADERS  += mips.h \
    Utilities.h \
    aboutdialog.h \
    console.h \
    dcbgroups.h \
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
    device.h \
    zmqworker.h \
    TextLabel.h Shutdown.h SaveLoad.h CPbutton.h DACchannel.h ESI.h \
    Ccontrol.h Cpanel.h StatusLight.h TextMessage.h Table.h Slider.h
FORMS    += mips.ui \
    settingsdialog.ui \
    aboutdialog.ui \
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
#QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64
QMAKE_APPLE_DEVICE_ARCHS = arm64

# macOS Homebrew paths
macx {
    INCLUDEPATH += /opt/homebrew/include
    LIBS += -L/opt/homebrew/lib -lzmq
}

# Windows vcpkg paths
win32 {
    INCLUDEPATH += C:/vcpkg/installed/x64-windows/include
    LIBS += -LC:/vcpkg/installed/x64-windows/lib -llibzmq-mt-4_3_5
}

# These two commands allow memory leak testing
#QMAKE_CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
#QMAKE_LFLAGS += -fsanitize=address
CONFIG+=sdk_no_version_check
#QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
#QMAKE_CXXFLAGS *= "-Xpreprocessor -fopenmp"
#QMAKE_MAC_SDK = macosx15.2
DISTFILES += \
    Revision.md
