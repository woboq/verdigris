/****************************************************************************
**
** Copyright (C) 2015 Olivier Goffart <ogoffart@woboq.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#include <QtCore/qobject.h>
#include <QtCore/qmetaobject.h>
#include "wobjectimpl.h"

class tst_QMetaEnum : public QObject
{
    W_OBJECT(tst_QMetaEnum)
public:
    enum SuperEnum { SuperValue1 = 1 , SuperValue2 = 2 };
    enum Flag { Flag1 = 1 , Flag2 = 2 };
    W_DECLARE_FLAGS(Flags, Flag)
    W_ENUM(SuperEnum, SuperValue1, SuperValue2)
    W_FLAG(Flags, Flag1, Flag2)

private:
    void fromType(); W_SLOT(fromType)
    void valuesToKeys_data(); W_SLOT(valuesToKeys_data)
    void valuesToKeys(); W_SLOT(valuesToKeys)
    void defaultConstructed(); W_SLOT(defaultConstructed)
};

W_OBJECT_IMPL(tst_QMetaEnum)

void tst_QMetaEnum::fromType()
{
    QMetaEnum meta = QMetaEnum::fromType<SuperEnum>();
    QVERIFY(meta.isValid());
    QVERIFY(!meta.isFlag());
    QCOMPARE(meta.name(), "SuperEnum");
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    QCOMPARE(meta.enumName(), "SuperEnum");
#endif
    QCOMPARE(meta.enclosingMetaObject(), &staticMetaObject);
    QCOMPARE(meta.keyCount(), 2);

    meta = QMetaEnum::fromType<Flags>();
    QVERIFY(meta.isValid());
    QVERIFY(meta.isFlag());
    QCOMPARE(meta.name(), "Flags");
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    QCOMPARE(meta.enumName(), "Flag");
#endif
    QCOMPARE(meta.enclosingMetaObject(), &staticMetaObject);
    QCOMPARE(meta.keyCount(), 2);
}

Q_DECLARE_METATYPE(Qt::WindowFlags)

void tst_QMetaEnum::valuesToKeys_data()
{
   QTest::addColumn<Qt::WindowFlags>("windowFlags");
   QTest::addColumn<QByteArray>("expected");

   QTest::newRow("Window")
       << Qt::WindowFlags(Qt::Window)
       << QByteArrayLiteral("Window");

   // Verify that Qt::Dialog does not cause 'Window' to appear in the output.
   QTest::newRow("Frameless_Dialog")
       << (Qt::Dialog | Qt::FramelessWindowHint)
       << QByteArrayLiteral("Dialog|FramelessWindowHint");

   // Similarly, Qt::WindowMinMaxButtonsHint should not show up as
   // WindowMinimizeButtonHint|WindowMaximizeButtonHint
   QTest::newRow("Tool_MinMax_StaysOnTop")
       << (Qt::Tool | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint)
       << QByteArrayLiteral("Tool|WindowMinMaxButtonsHint|WindowStaysOnTopHint");
}

void tst_QMetaEnum::valuesToKeys()
{
    QFETCH(Qt::WindowFlags, windowFlags);
    QFETCH(QByteArray, expected);

    QMetaEnum me = QMetaEnum::fromType<Qt::WindowFlags>();
    QCOMPARE(me.valueToKeys(windowFlags), expected);
}

void tst_QMetaEnum::defaultConstructed()
{
    QMetaEnum e;
    QVERIFY(!e.isValid());
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    QVERIFY(!e.isScoped());
#endif
    QVERIFY(!e.isFlag());
    QCOMPARE(QByteArray(e.name()), QByteArray());
}

Q_STATIC_ASSERT(QtPrivate::IsQEnumHelper<tst_QMetaEnum::SuperEnum>::Value);
Q_STATIC_ASSERT(QtPrivate::IsQEnumHelper<Qt::WindowFlags>::Value);
Q_STATIC_ASSERT(QtPrivate::IsQEnumHelper<Qt::Orientation>::Value);
Q_STATIC_ASSERT(!QtPrivate::IsQEnumHelper<int>::Value);
Q_STATIC_ASSERT(!QtPrivate::IsQEnumHelper<QObject>::Value);
Q_STATIC_ASSERT(!QtPrivate::IsQEnumHelper<QObject*>::Value);
Q_STATIC_ASSERT(!QtPrivate::IsQEnumHelper<void>::Value);


QTEST_MAIN(tst_QMetaEnum)
