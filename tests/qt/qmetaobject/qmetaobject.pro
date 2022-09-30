CONFIG += testcase
qtConfig(c++20): CONFIG += c++20
TARGET = tst_qmetaobject
QT = core testlib
SOURCES = tst_qmetaobject6.cpp
include(../../../src/verdigris.pri)
