CONFIG += testcase
TARGET = tst_qmetamethod
QT = core testlib
SOURCES = tst_qmetamethod6.cpp
mac:CONFIG -= app_bundle
include(../../../src/verdigris.pri)

