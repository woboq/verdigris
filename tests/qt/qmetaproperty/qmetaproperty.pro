CONFIG += testcase
TARGET = tst_qmetaproperty
QT = core testlib
greaterThan(QT_MAJOR_VERSION, 5) {
SOURCES = tst_qmetaproperty6.cpp
} else {
SOURCES = tst_qmetaproperty5.cpp
}
include(../../../src/verdigris.pri)
