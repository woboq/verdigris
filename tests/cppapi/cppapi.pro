CONFIG += testcase
TARGET = tst_cppapi
QT = core testlib
include(../../src/verdigris.pri)
SOURCES = tst_cppapi.cpp
contains(QT_CONFIG, c++1z): CONFIG += c++1z
