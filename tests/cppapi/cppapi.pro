CONFIG += testcase
TARGET = tst_cppapi
QT = core testlib
include(../../src/verdigris.pri)
SOURCES = tst_cppapi.cpp

msvc:QMAKE_CXXFLAGS += -permissive-
