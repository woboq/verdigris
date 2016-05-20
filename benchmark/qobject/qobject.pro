QT += widgets testlib

TEMPLATE = app
TARGET = tst_bench_qobject

HEADERS += object.h
SOURCES += main.cpp object.cpp

CONFIG+=c++14
INCLUDEPATH += ../../src
