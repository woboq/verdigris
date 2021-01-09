CONFIG += testcase
qtConfig(c++14): CONFIG += c++14
TARGET = tst_qmetaobject
QT = core testlib
greaterThan(QT_MAJOR_VERSION, 5) {
SOURCES = tst_qmetaobject6.cpp
} else {
SOURCES = tst_qmetaobject5.cpp
}
include(../../../src/verdigris.pri)
