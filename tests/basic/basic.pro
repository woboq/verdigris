CONFIG += testcase
TARGET = tst_basic
QT = core testlib
include(../../src/verdigris.pri)
SOURCES = tst_basic.cpp anothertu.cpp
contains(QT_CONFIG, c++1z): CONFIG += c++1z

