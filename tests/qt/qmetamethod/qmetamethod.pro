CONFIG += testcase
TARGET = tst_qmetamethod
QT = core testlib
SOURCES = tst_qmetamethod.cpp
mac:CONFIG -= app_bundle
CONFIG += c++14
INCLUDEPATH += ../../../src

