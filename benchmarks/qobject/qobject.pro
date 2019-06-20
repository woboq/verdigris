QT = core testlib

TEMPLATE = app
TARGET = tst_bench_qobject

HEADERS += object.h
SOURCES += main.cpp object.cpp

include(../../src/verdigris.pri)
