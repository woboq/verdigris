CONFIG += testcase console
QT = network testlib
TARGET = tst_qobject

versionAtLeast(QT_VERSION, 6.2.0) {
  QT += testlib-private
}

SOURCES = tst_qobject6.cpp

# Force C++17 if available (needed due to P0012R1)
contains(QT_CONFIG, c++1z): CONFIG += c++1z

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0

include(../../../src/verdigris.pri)
