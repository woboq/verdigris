CONFIG += testcase
TARGET = tst_qmetamethod
QT = core testlib
greaterThan(QT_MAJOR_VERSION, 5) {
SOURCES = tst_qmetamethod6.cpp
} else {
SOURCES = tst_qmetamethod5.cpp
}
mac:CONFIG -= app_bundle
include(../../../src/verdigris.pri)

