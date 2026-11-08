# Unit tests for devicescriptproto (no hardware, no GUI).
#   qmake && make && ./devicescriptproto_test        (Windows: release\devicescriptproto_test.exe)
QT       += core testlib
QT       -= gui
CONFIG   += console testcase
CONFIG   -= app_bundle
TEMPLATE  = app
TARGET    = devicescriptproto_test
INCLUDEPATH += ../..
SOURCES  += tst_devicescriptproto.cpp ../../devicescriptproto.cpp
HEADERS  += ../../devicescriptproto.h

# Same workaround MIPS.pro uses for the macOS SDK (Qt 6's yield intrinsic).
greaterThan(QT_MAJOR_VERSION, 5): macx: QMAKE_CXXFLAGS += -include arm_acle.h
CONFIG += sdk_no_version_check
