/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#include <qobject.h>
#include <qmetaobject.h>
#include <qabstractproxymodel.h>
#ifdef QT_BUILD_INTERNAL
#undef QT_BUILD_INTERNAL  // Verdigris don't test the internals
#endif

#include <wobjectimpl.h>

Q_DECLARE_METATYPE(const QMetaObject *)

W_REGISTER_ARGTYPE(QList<QString>)

struct MyStruct
{
    int i;
};

class MyGadget
{
    W_GADGET(MyGadget)
public:
    MyGadget() {}
    W_CONSTRUCTOR()
};

W_GADGET_IMPL(MyGadget)

namespace MyNamespace {
    // Used in tst_QMetaObject::checkScope
    class MyClass : public QObject
    {
        W_OBJECT(MyClass)

    public:
        enum MyEnum {
            MyEnum1,
            MyEnum2,
            MyEnum3
        };
        enum class MyScopedEnum {
            Enum1,
            Enum2,
            Enum3
        };
        enum MyAnotherEnum {
            MyAnotherEnum1 = 1,
            MyAnotherEnum2 = 2,
            MyAnotherEnum3 = -1
        };
        enum MyFlag {
            MyFlag1 = 0x01,
            MyFlag2 = 0x02,
            MyFlag3 = 0x04
        };
        enum class MyScopedFlag {
            MyFlag1 = 0x10,
            MyFlag2 = 0x20,
            MyFlag3 = 0x40
        };
        W_DECLARE_FLAGS(MyFlags, MyFlag)
        W_DECLARE_FLAGS(MyScopedFlags, MyScopedFlag)

        MyEnum myEnum() const { return m_enum; }
        void setMyEnum(MyEnum val) { m_enum = val; }

        MyFlags myFlags() const { return m_flags; }
        void setMyFlags(MyFlags val) { m_flags = val; }

        MyClass(QObject *parent = 0)
            : QObject(parent),
              m_enum(MyEnum1),
              m_flags(MyFlag1|MyFlag2)
                { }
    private:
        W_ENUM(MyEnum, MyEnum1, MyEnum2, MyEnum3)
        W_ENUM(MyScopedEnum, MyScopedEnum::Enum1, MyScopedEnum::Enum2, MyScopedEnum::Enum3)
        W_ENUM(MyAnotherEnum,MyAnotherEnum1,MyAnotherEnum2,MyAnotherEnum3)
        W_FLAG(MyFlags,MyFlag1,MyFlag2,MyFlag3)
        W_FLAG(MyScopedFlags, MyScopedFlag::MyFlag1, MyScopedFlag::MyFlag2, MyScopedFlag::MyFlag3)


        MyEnum m_enum;
        MyFlags m_flags;

        W_PROPERTY(MyEnum, myEnum READ myEnum WRITE setMyEnum)
        W_PROPERTY(MyFlags, myFlags READ myFlags WRITE setMyFlags)


    };
    Q_DECLARE_OPERATORS_FOR_FLAGS(MyClass::MyFlags)


    // test the old Q_ENUMS macro
    class MyClass2 : public QObject
    {
        W_OBJECT(MyClass2)

    public:
        enum MyEnum {
            MyEnum1,
            MyEnum2,
            MyEnum3
        };
        enum MyAnotherEnum {
            MyAnotherEnum1 = 1,
            MyAnotherEnum2 = 2,
            MyAnotherEnum3 = -1
        };
        enum MyFlag {
            MyFlag1 = 0x01,
            MyFlag2 = 0x02,
            MyFlag3 = 0x04
        };
        Q_DECLARE_FLAGS(MyFlags, MyFlag)

        MyEnum myEnum() const { return m_enum; }
        void setMyEnum(MyEnum val) { m_enum = val; }

        MyFlags myFlags() const { return m_flags; }
        void setMyFlags(MyFlags val) { m_flags = val; }

        MyClass2(QObject *parent = 0)
            : QObject(parent),
              m_enum(MyEnum1),
              m_flags(MyFlag1|MyFlag2)
        { }
    private:

        W_ENUM(MyEnum, MyEnum1, MyEnum2, MyEnum3)
        W_ENUM(MyAnotherEnum,MyAnotherEnum1,MyAnotherEnum2,MyAnotherEnum3)
        W_FLAG(MyFlags,MyFlag1,MyFlag2,MyFlag3)

        MyEnum m_enum;
        MyFlags m_flags;

        W_PROPERTY(MyEnum, myEnum READ myEnum WRITE setMyEnum)
        W_PROPERTY(MyFlags, myFlags READ myFlags WRITE setMyFlags)

    };

    // Test inherits
    class MyClassSubclass : public MyClass
    {
        W_OBJECT(MyClassSubclass)
    };

    class MyClassSubclass2 : public MyClass2
    {
        W_OBJECT(MyClassSubclass2)
    };

    class MyClass2Subclass : public MyClass
    {
        W_OBJECT(MyClass2Subclass)
    };

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    class ClassWithSetterGetterSignals : public QObject
    {
        W_OBJECT(ClassWithSetterGetterSignals)

        public:
            int value1() const { return m_value1; }
            void setValue1(int v) {
                if (v != m_value1) {
                    m_value1 = v;
                    Q_EMIT value1Changed();
                }
            }

            int value2() const { return m_value2; }
            void setValue2(int v) {
                if (v != m_value2) {
                    m_value2 = v;
                    Q_EMIT value2Changed();
                }
            }

        Q_SIGNALS:
            void value1Changed() W_SIGNAL(value1Changed)
            void value2Changed() W_SIGNAL(value2Changed)

        private:
            int m_value1 = 0;
            int m_value2 = 0;
    };

    class ClassWithSetterGetterSignalsAddsProperties : public ClassWithSetterGetterSignals
    {
        W_OBJECT(ClassWithSetterGetterSignalsAddsProperties)
        W_PROPERTY(int, value1 READ value1 WRITE setValue1 NOTIFY value1Changed)
        W_PROPERTY(int, value2 READ value2 WRITE setValue2 NOTIFY value2Changed)
    };

    class ClassWithChangedSignal : public QObject
    {
        W_OBJECT(ClassWithChangedSignal)

        public:
            int value1() const { return m_value1; }
            void setValue1(int v) {
                if (v != m_value1) {
                    m_value1 = v;
                    Q_EMIT propertiesChanged();
                }
            }

            void thisIsNotASignal() { }

        Q_SIGNALS:
            void propertiesChanged() W_SIGNAL(propertiesChanged)

        private:
            int m_value1 = 0;
    };

    class ClassWithChangedSignalNewValue : public ClassWithChangedSignal
    {
        W_OBJECT(ClassWithChangedSignalNewValue)


        private:
            int m_value2 = 0;
            int m_value3 = 0;

        W_PROPERTY(int, value2 MEMBER m_value2 NOTIFY propertiesChanged)
#if 0 // With verdigris, this is a compilation error when the NOTIFY signal is not a signal
        W_PROPERTY(int, value3 MEMBER m_value3 NOTIFY thisIsNotASignal)
#endif
    };
#endif
}
W_OBJECT_IMPL(MyNamespace::MyClass)
W_OBJECT_IMPL(MyNamespace::MyClass2)
W_OBJECT_IMPL(MyNamespace::MyClassSubclass)
W_OBJECT_IMPL(MyNamespace::MyClassSubclass2)
W_OBJECT_IMPL(MyNamespace::MyClass2Subclass)
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
W_OBJECT_IMPL(MyNamespace::ClassWithSetterGetterSignals)
W_OBJECT_IMPL(MyNamespace::ClassWithSetterGetterSignalsAddsProperties)
W_OBJECT_IMPL(MyNamespace::ClassWithChangedSignal)
W_OBJECT_IMPL(MyNamespace::ClassWithChangedSignalNewValue)
#endif

class tst_QMetaObject : public QObject
{
    W_OBJECT(tst_QMetaObject)

public:
    enum EnumType { EnumType1 };
    W_ENUM(EnumType,EnumType1);

    void setValue(EnumType) {}
    EnumType getValue() const { return EnumType1; }

    void set_value(EnumType) {}
    EnumType get_value() const { return EnumType1; }

    void setVal3(MyStruct) {}
    MyStruct val3() const { MyStruct s = {42}; return s; }

    void setVal4(const QList<QVariant> &list) { value4 = list; }
    QList<QVariant> val4() const { return value4; }

    void setVal5(const QVariantList &list) { value5 = list; }
    QVariantList val5() const { return value5; }

    int value6() const { return 1; }

    void setVal7(MyStruct) {}
    MyStruct value7() const { MyStruct s = {42}; return s; }

    int value8() const { return 1; }

    int value9() const { return 1; }

    int value10() const { return 1; }

    QList<QVariant> value4;
    QVariantList value5;

private slots:
    void connectSlotsByName();
    void invokeMetaMember();
    void invokePointer();
    void invokeQueuedMetaMember();
    void invokeQueuedPointer();
    void invokeBlockingQueuedMetaMember();
    void invokeBlockingQueuedPointer();
    void invokeCustomTypes();
    void invokeMetaConstructor();
    void invokeTypedefTypes();
    void invokeException();
    void invokeQueuedAutoRegister();
    void qtMetaObjectInheritance();
    void normalizedSignature_data();
    void normalizedSignature();
    void normalizedType_data();
    void normalizedType();
    void customPropertyType();
    void checkScope_data();
    void checkScope();
    void propertyNotify();
    void propertyConstant();
    void propertyFinal();

    void stdSet();
    void classInfo();

    void metaMethod();

    void indexOfMethod_data();
    void indexOfMethod();

    void indexOfMethodPMF();

    void signalOffset_data();
    void signalOffset();
    void signalCount_data();
    void signalCount();
    void signal_data();
    void signal();
    void signalIndex_data();
    void signalIndex();
    void enumDebugStream_data();
    void enumDebugStream();

    void inherits_data();
    void inherits();

    /***/
    W_SLOT(connectSlotsByName, W_Access::Private)
    W_SLOT(invokeMetaMember, W_Access::Private)
    W_SLOT(invokeQueuedMetaMember, W_Access::Private)
    W_SLOT(invokeBlockingQueuedMetaMember, W_Access::Private)
    W_SLOT(invokeCustomTypes, W_Access::Private)
    W_SLOT(invokeMetaConstructor, W_Access::Private)
    W_SLOT(invokeTypedefTypes, W_Access::Private)
    W_SLOT(invokeException, W_Access::Private)
    W_SLOT(qtMetaObjectInheritance, W_Access::Private)
    W_SLOT(normalizedSignature_data, W_Access::Private)
    W_SLOT(normalizedSignature, W_Access::Private)
    W_SLOT(normalizedType_data, W_Access::Private)
    W_SLOT(normalizedType, W_Access::Private)
    W_SLOT(customPropertyType, W_Access::Private)
    W_SLOT(checkScope_data, W_Access::Private)
    W_SLOT(checkScope, W_Access::Private)
    W_SLOT(propertyNotify, W_Access::Private)
    W_SLOT(propertyConstant, W_Access::Private)
    W_SLOT(propertyFinal, W_Access::Private)

    W_SLOT(stdSet, W_Access::Private)
    W_SLOT(classInfo, W_Access::Private)

    W_SLOT(metaMethod, W_Access::Private)

    W_SLOT(indexOfMethod_data, W_Access::Private)
    W_SLOT(indexOfMethod, W_Access::Private)

    W_SLOT(indexOfMethodPMF, W_Access::Private)

#ifdef QT_BUILD_INTERNAL
    W_SLOT(signalOffset_data, W_Access::Private)
    W_SLOT(signalOffset, W_Access::Private)
    W_SLOT(signalCount_data, W_Access::Private)
    W_SLOT(signalCount, W_Access::Private)
    W_SLOT(signal_data, W_Access::Private)
    W_SLOT(signal, W_Access::Private)
    W_SLOT(signalIndex_data, W_Access::Private)
    W_SLOT(signalIndex, W_Access::Private)
#endif
    W_SLOT(enumDebugStream, W_Access::Private)

    W_SLOT(inherits_data, W_Access::Private)
    W_SLOT(inherits, W_Access::Private)

    void notifySignalsInParentClass();

signals:
    void value6Changed()
    W_SIGNAL(value6Changed)
    void value7Changed(const QString &_)
    W_SIGNAL(value7Changed,_)


    W_PROPERTY(EnumType, value WRITE setValue READ getValue)
    W_PROPERTY(EnumType, value2 WRITE set_value READ get_value)
    W_PROPERTY(MyStruct, value3 WRITE setVal3 READ val3)
    W_PROPERTY(QList<QVariant>, value4 WRITE setVal4 READ val4)
    W_PROPERTY(QVariantList, value5 WRITE setVal5 READ val5)
    W_PROPERTY(int, value6 READ value6 NOTIFY value6Changed)
    W_PROPERTY(MyStruct, value7 READ value7 WRITE setVal7 NOTIFY value7Changed)
    W_PROPERTY(int, value8 READ value8)
    W_PROPERTY(int, value9 READ value9 CONSTANT)
    W_PROPERTY(int, value10 READ value10 FINAL)

};

W_OBJECT_IMPL(tst_QMetaObject)


void tst_QMetaObject::stdSet()
{
    QSKIP("Not supported by W_PROPERTY");

    const QMetaObject *mo = metaObject();

    QMetaProperty prop = mo->property(mo->indexOfProperty("value"));
    QVERIFY(prop.isValid());
    QVERIFY(prop.hasStdCppSet());

    prop = mo->property(mo->indexOfProperty("value2"));
    QVERIFY(prop.isValid());
    QVERIFY(!prop.hasStdCppSet());
}

class CTestObject: public QObject
{
    W_OBJECT(CTestObject)

public:
    CTestObject(): QObject(), invokeCount1(0), invokeCount2(0)
    {
    }

    void fire(const QString &name)
    {
        child = new QObject(this);
        child->setObjectName(name);
        QMetaObject::connectSlotsByName(this);
        delete child; child = 0;
    }

    int invokeCount1;
    int invokeCount2;
    QObject *child;

public slots:
    void on_child1_destroyed(QObject *obj = 0)
    {
        ++invokeCount1;
        if (!obj || obj != child)
            qWarning() << "on_child1_destroyed invoked with wrong child object";
    }
    W_SLOT(on_child1_destroyed)
    void on_child2_destroyed() { ++invokeCount2; }
    W_SLOT(on_child2_destroyed)
};

W_OBJECT_IMPL(CTestObject)


class CTestObjectOverloads: public QObject
{
    W_OBJECT(CTestObjectOverloads)

public:
    CTestObjectOverloads(): invokeCount1(0), invokeCount2(0) {}

    int invokeCount1;
    int invokeCount2;
    QObject *child;

    void fire(const QString &name)
    {
        child = new QObject(this);
        child->setObjectName(name);
        QMetaObject::connectSlotsByName(this);
        delete child; child = 0;
    }

private slots:
    void on_child1_destroyed(QObject *obj)
    {
        ++invokeCount1;
        if (!obj || obj != child)
            qWarning() << "on_child1_destroyed invoked with wrong child object";
    }
    W_SLOT(on_child1_destroyed,(QObject*))
    void on_child1_destroyed() { ++invokeCount2; }
    W_SLOT(on_child1_destroyed,())
};

W_OBJECT_IMPL(CTestObjectOverloads)

#define FUNCTION(x) "QMetaObject::" x ": "

void tst_QMetaObject::connectSlotsByName()
{
    CTestObject obj;
    QCOMPARE(obj.invokeCount1, 0);
    QCOMPARE(obj.invokeCount2, 0);

    QTest::ignoreMessage(QtWarningMsg, FUNCTION("connectSlotsByName") "No matching signal for on_child1_destroyed(QObject*)");
    QTest::ignoreMessage(QtWarningMsg, FUNCTION("connectSlotsByName") "No matching signal for on_child2_destroyed()");
    obj.fire("bubu");
    QCOMPARE(obj.invokeCount1, 0);
    QCOMPARE(obj.invokeCount2, 0);

    QTest::ignoreMessage(QtWarningMsg, FUNCTION("connectSlotsByName") "No matching signal for on_child2_destroyed()");
    obj.fire("child1");
    QCOMPARE(obj.invokeCount1, 1);
    QCOMPARE(obj.invokeCount2, 0);

    QTest::ignoreMessage(QtWarningMsg, FUNCTION("connectSlotsByName") "No matching signal for on_child1_destroyed(QObject*)");
    obj.fire("child2");
    QCOMPARE(obj.invokeCount1, 1);
    QCOMPARE(obj.invokeCount2, 1);

    QTest::ignoreMessage(QtWarningMsg, FUNCTION("connectSlotsByName") "No matching signal for on_child2_destroyed()");
    obj.fire("child1");
    QCOMPARE(obj.invokeCount1, 2);
    QCOMPARE(obj.invokeCount2, 1);

    // now test with real overloads
    CTestObjectOverloads obj2;
    obj2.fire("child1");
    QCOMPARE(obj2.invokeCount1, 1);
    QCOMPARE(obj2.invokeCount2, 1);
}

struct MyUnregisteredType { };

static int countedStructObjectsCount = 0;
struct CountedStruct
{
    CountedStruct() { ++countedStructObjectsCount; }
    CountedStruct(const CountedStruct &) { ++countedStructObjectsCount; }
    CountedStruct &operator=(const CountedStruct &) { return *this; }
    ~CountedStruct() { --countedStructObjectsCount; }
};

#ifndef QT_NO_EXCEPTIONS
class ObjectException : public std::exception { };
#endif

W_REGISTER_ARGTYPE(QString&)
W_REGISTER_ARGTYPE(QThread*)
W_REGISTER_ARGTYPE(MyUnregisteredType)
W_REGISTER_ARGTYPE(CountedStruct)

class QtTestObject: public QObject
{
    friend class tst_QMetaObject;
    W_OBJECT(QtTestObject)

public:
    QtTestObject();
    QtTestObject(const QString &s) : slotResult(s) {}
    QtTestObject(QObject *parent);
    W_CONSTRUCTOR(QObject*)

public slots:
    void sl0();
    W_SLOT(sl0)
    QString sl1(QString s1);
    W_SLOT(sl1)
    void sl2(QString s1, QString s2);
    W_SLOT(sl2)
    void sl3(QString s1, QString s2, QString s3);
    W_SLOT(sl3)
    void sl4(QString s1, QString s2, QString s3, const QString s4);
    W_SLOT(sl4)
    void sl5(QString s1, QString s2, QString s3, QString s4, const QString &s5);
    W_SLOT(sl5)
    void sl6(QString s1, QString s2, QString s3, QString s4, const QString s5, QString s6);
    W_SLOT(sl6)
    void sl7(QString s1, QString s2, QString s3, QString s4, QString s5, QString s6, QString s7);
    W_SLOT(sl7)
    void sl8(QString s1, QString s2, QString s3, QString s4, QString s5, QString s6, QString s7,
            QString s8);
    W_SLOT(sl8)
    void sl9(QString s1, QString s2, QString s3, QString s4, QString s5, QString s6, QString s7,
            QString s8, QString s9);
    W_SLOT(sl9)
    void sl10(QString s1, QString s2, QString s3, QString s4, QString s5, QString s6, QString s7,
            QString s8, QString s9, QString s10);
    W_SLOT(sl10)
    QObject *sl11();
    W_SLOT(sl11)
    const char *sl12();
    W_SLOT(sl12)
    QList<QString> sl13(QList<QString> l1);
    W_SLOT(sl13)
    qint64 sl14();
    W_SLOT(sl14)
    void testSender();
    W_SLOT(testSender)

    void testReference(QString &str);
    W_SLOT(testReference)
    void testLongLong(qint64 ll1, quint64 ll2);
    W_SLOT(testLongLong)
    void moveToThread(QThread *t)
    { QObject::moveToThread(t); }
    W_SLOT(moveToThread)

    void slotWithUnregisteredParameterType(MyUnregisteredType);
    W_SLOT(slotWithUnregisteredParameterType)

    CountedStruct throwingSlot(const CountedStruct &, CountedStruct s2) {
#ifndef QT_NO_EXCEPTIONS
        throw ObjectException();
#endif
        return s2;
    }
    W_SLOT(throwingSlot)

    void slotWithRegistrableArgument(QtTestObject *o1, QPointer<QtTestObject> o2,
                                     QSharedPointer<QtTestObject> o3, QWeakPointer<QtTestObject> o4,
                                     QVector<QtTestObject *> o5, QList<QtTestObject *> o6)
    {
        slotResult = QLatin1String("slotWithRegistrableArgument:") + o1->slotResult + o2->slotResult
            + o3->slotResult + o4.data()->slotResult + QString::number(o5.size())
            + QString::number(o6.size());
    }

public:
    static void staticFunction0();
    static qint64 staticFunction1();

signals:
    void sig0()
    W_SIGNAL(sig0)
    QString sig1(QString s1)
    W_SIGNAL(sig1,s1)
    void sig10(QString s1, QString s2, QString s3, QString s4, QString s5, QString s6, QString s7,
               QString s8, QString s9, QString s10)
    W_SIGNAL(sig10,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10)

protected:
    QtTestObject(QVariant) {}
private:
    QtTestObject(QVariant, QVariant) {}

public:
    QString slotResult;
    static QString staticResult;
};

W_OBJECT_IMPL(QtTestObject)

QString QtTestObject::staticResult;

QtTestObject::QtTestObject()
{
    connect(this, SIGNAL(sig0()), this, SLOT(sl0()));
    connect(this, SIGNAL(sig1(QString)), this, SLOT(sl1(QString)));
}

QtTestObject::QtTestObject(QObject *parent)
    : QObject(parent)
{
}

void QtTestObject::sl0() { slotResult = "sl0"; }
QString QtTestObject::sl1(QString s1) { slotResult = "sl1:" + s1; return "yessir"; }
void QtTestObject::sl2(QString s1, QString s2) { slotResult = "sl2:" + s1 + s2; }
void QtTestObject::sl3(QString s1, QString s2, QString s3)
{ slotResult = "sl3:" + s1 + s2 + s3; }
void QtTestObject::sl4(QString s1, QString s2, QString s3, const QString s4)
{ slotResult = "sl4:" + s1 + s2 + s3 + s4; }
void QtTestObject::sl5(QString s1, QString s2, QString s3, QString s4, const QString &s5)
{ slotResult = "sl5:" + s1 + s2 + s3 + s4 + s5; }
void QtTestObject::sl6(QString s1, QString s2, QString s3, QString s4,
                      const QString s5, QString s6)
{ slotResult = "sl6:" + s1 + s2 + s3 + s4 + s5 + s6; }
void QtTestObject::sl7(QString s1, QString s2, QString s3, QString s4, QString s5,
                      QString s6, QString s7)
{ slotResult = "sl7:" + s1 + s2 + s3 + s4 + s5 + s6 + s7; }
void QtTestObject::sl8(QString s1, QString s2, QString s3, QString s4, QString s5,
                      QString s6, QString s7, QString s8)
{ slotResult = "sl8:" + s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8; }
void QtTestObject::sl9(QString s1, QString s2, QString s3, QString s4, QString s5,
                      QString s6, QString s7, QString s8, QString s9)
{ slotResult = "sl9:" + s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8 + s9; }
void QtTestObject::sl10(QString s1, QString s2, QString s3, QString s4, QString s5,
                      QString s6, QString s7, QString s8, QString s9, QString s10)
{ slotResult = "sl10:" + s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8 + s9 + s10; }
QObject *QtTestObject::sl11()
{ slotResult = "sl11"; return this; }
const char *QtTestObject::sl12()
{ slotResult = "sl12"; return "foo"; }
QList<QString> QtTestObject::sl13(QList<QString> l1)
{ slotResult = "sl13"; return l1; }
qint64 QtTestObject::sl14()
{ slotResult = "sl14"; return Q_INT64_C(123456789)*123456789; }

void QtTestObject::testReference(QString &str)
{ slotResult = "testReference:" + str; str = "gotcha"; }

void QtTestObject::testLongLong(qint64 ll1, quint64 ll2)
{ slotResult = "testLongLong:" + QString::number(ll1) + "," + QString::number(ll2); }

void QtTestObject::testSender()
{
    slotResult = QString::asprintf("%p", sender());
}

void QtTestObject::slotWithUnregisteredParameterType(MyUnregisteredType)
{ slotResult = "slotWithUnregisteredReturnType"; }

void QtTestObject::staticFunction0()
{
    staticResult = "staticFunction0";
}

qint64 QtTestObject::staticFunction1()
{ staticResult = "staticFunction1"; return Q_INT64_C(123456789)*123456789; }

void tst_QMetaObject::invokeMetaMember()
{
    QtTestObject obj;

    QString t1("1"); QString t2("2"); QString t3("3"); QString t4("4"); QString t5("5");
    QString t6("6"); QString t7("7"); QString t8("8"); QString t9("9"); QString t10("X");

    // Test nullptr
    char *nullCharArray = nullptr;
    const char *nullConstCharArray = nullptr;
    QVERIFY(!QMetaObject::invokeMethod(nullptr, nullCharArray));
    QVERIFY(!QMetaObject::invokeMethod(nullptr, nullConstCharArray));
    QVERIFY(!QMetaObject::invokeMethod(nullptr, "sl0"));
    QVERIFY(!QMetaObject::invokeMethod(&obj, nullCharArray));
    QVERIFY(!QMetaObject::invokeMethod(&obj, nullConstCharArray));
    QVERIFY(!QMetaObject::invokeMethod(&obj, nullCharArray, Qt::AutoConnection));
    QVERIFY(!QMetaObject::invokeMethod(&obj, nullConstCharArray, Qt::AutoConnection));
    QVERIFY(!QMetaObject::invokeMethod(&obj, nullCharArray, Qt::AutoConnection, QGenericReturnArgument()));
    QVERIFY(!QMetaObject::invokeMethod(&obj, nullConstCharArray, Qt::AutoConnection, QGenericReturnArgument()));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl0"));
    QCOMPARE(obj.slotResult, QString("sl0"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl1", Q_ARG(QString, t1)));
    QCOMPARE(obj.slotResult, QString("sl1:1"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl2", Q_ARG(const QString, t1), Q_ARG(QString, t2)));
    QCOMPARE(obj.slotResult, QString("sl2:12"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl3", Q_ARG(QString, t1), Q_ARG(QString, t2), Q_ARG(QString, t3)));
    QCOMPARE(obj.slotResult, QString("sl3:123"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl4", Q_ARG(QString, t1), Q_ARG(QString, t2), Q_ARG(QString, t3),
                       Q_ARG(QString, t4)));
    QCOMPARE(obj.slotResult, QString("sl4:1234"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl5", Q_ARG(QString, t1), Q_ARG(QString, t2), Q_ARG(QString, t3),
                       Q_ARG(QString, t4), Q_ARG(QString, "5")));
    QCOMPARE(obj.slotResult, QString("sl5:12345"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl6", Q_ARG(QString, t1), Q_ARG(QString, t2), Q_ARG(QString, t3),
                       Q_ARG(QString, t4), Q_ARG(QString, t5), Q_ARG(QString, t6)));
    QCOMPARE(obj.slotResult, QString("sl6:123456"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl7", Q_ARG(QString, t1), Q_ARG(QString, t2), Q_ARG(QString, t3),
                       Q_ARG(QString, t4), Q_ARG(QString, t5), Q_ARG(QString, t6),
                       Q_ARG(QString, t7)));
    QCOMPARE(obj.slotResult, QString("sl7:1234567"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl8", Q_ARG(QString, t1), Q_ARG(QString, t2), Q_ARG(QString, t3),
                       Q_ARG(QString, t4), Q_ARG(QString, t5), Q_ARG(QString, t6),
                       Q_ARG(QString, t7), Q_ARG(QString, t8)));
    QCOMPARE(obj.slotResult, QString("sl8:12345678"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl9", Q_ARG(QString, t1), Q_ARG(QString, t2), Q_ARG(QString, t3),
                       Q_ARG(QString, t4), Q_ARG(QString, t5), Q_ARG(QString, t6),
                       Q_ARG(QString, t7), Q_ARG(QString, t8), Q_ARG(QString, t9)));
    QCOMPARE(obj.slotResult, QString("sl9:123456789"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl11"));
    QCOMPARE(obj.slotResult, QString("sl11"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "testSender"));
    QCOMPARE(obj.slotResult, QString("0x0"));

    QString refStr("whatever");
    QVERIFY(QMetaObject::invokeMethod(&obj, "testReference", QGenericArgument("QString&", &refStr)));
    QCOMPARE(obj.slotResult, QString("testReference:whatever"));
    QCOMPARE(refStr, QString("gotcha"));

    qint64 ll1 = -1;
    quint64 ll2 = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj,
                                      "testLongLong",
                                      Q_ARG(qint64, ll1),
                                      Q_ARG(quint64, ll2)));
    QCOMPARE(obj.slotResult, QString("testLongLong:-1,0"));

    QString exp;
    QVERIFY(QMetaObject::invokeMethod(&obj, "sl1", Q_RETURN_ARG(QString, exp), Q_ARG(QString, "bubu")));
    QCOMPARE(exp, QString("yessir"));
    QCOMPARE(obj.slotResult, QString("sl1:bubu"));

    QObject *ptr = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj, "sl11", Q_RETURN_ARG(QObject*,ptr)));
    QCOMPARE(ptr, (QObject *)&obj);
    QCOMPARE(obj.slotResult, QString("sl11"));
    // try again with a space:
    ptr = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj, "sl11", Q_RETURN_ARG(QObject * , ptr)));
    QCOMPARE(ptr, (QObject *)&obj);
    QCOMPARE(obj.slotResult, QString("sl11"));

    const char *ptr2 = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj, "sl12", Q_RETURN_ARG(const char*, ptr2)));
    QVERIFY(ptr2 != 0);
    QCOMPARE(obj.slotResult, QString("sl12"));
    // try again with a space:
    ptr2 = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj, "sl12", Q_RETURN_ARG(char const * , ptr2)));
    QVERIFY(ptr2 != 0);
    QCOMPARE(obj.slotResult, QString("sl12"));

    // test w/ template args
    QList<QString> returnValue, argument;
    argument << QString("one") << QString("two") << QString("three");
    QVERIFY(QMetaObject::invokeMethod(&obj, "sl13",
                                      Q_RETURN_ARG(QList<QString>, returnValue),
                                      Q_ARG(QList<QString>, argument)));
    QCOMPARE(returnValue, argument);
    QCOMPARE(obj.slotResult, QString("sl13"));

    // return qint64
    qint64 return64;
    QVERIFY(QMetaObject::invokeMethod(&obj, "sl14",
                                      Q_RETURN_ARG(qint64, return64)));
    QCOMPARE(return64, Q_INT64_C(123456789)*123456789);
    QCOMPARE(obj.slotResult, QString("sl14"));

    //test signals
    QVERIFY(QMetaObject::invokeMethod(&obj, "sig0"));
    QCOMPARE(obj.slotResult, QString("sl0"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sig1", Q_ARG(QString, "baba")));
    QCOMPARE(obj.slotResult, QString("sl1:baba"));

    exp.clear();
    QVERIFY(QMetaObject::invokeMethod(&obj, "sig1", Q_RETURN_ARG(QString, exp), Q_ARG(QString, "hehe")));
    QCOMPARE(exp, QString("yessir"));
    QCOMPARE(obj.slotResult, QString("sl1:hehe"));

    QTest::ignoreMessage(QtWarningMsg, "QMetaObject::invokeMethod: No such method QtTestObject::doesNotExist()");
    QVERIFY(!QMetaObject::invokeMethod(&obj, "doesNotExist"));
    QTest::ignoreMessage(QtWarningMsg, "QMetaObject::invokeMethod: No such method QtTestObject::sl1(QString)(QString)");
    QVERIFY(!QMetaObject::invokeMethod(&obj, "sl1(QString)", Q_ARG(QString, "arg")));
    QTest::ignoreMessage(QtWarningMsg, "QMetaObject::invokeMethod: No such method QtTestObject::sl3(QString)\n"
                         "Candidates are:\n    sl3(QString,QString,QString)");
    QVERIFY(!QMetaObject::invokeMethod(&obj, "sl3", Q_ARG(QString, "arg")));
    QTest::ignoreMessage(QtWarningMsg, "QMetaObject::invokeMethod: No such method QtTestObject::sl1(QString,QString,QString)\n"
                         "Candidates are:\n    sl1(QString)");
    QVERIFY(!QMetaObject::invokeMethod(&obj, "sl1", Q_ARG(QString, "arg"), Q_ARG(QString, "arg"), Q_ARG(QString, "arg")));

    //should not have changed since last test.
    QCOMPARE(exp, QString("yessir"));
    QCOMPARE(obj.slotResult, QString("sl1:hehe"));
}

void testFunction(){}


void tst_QMetaObject::invokePointer()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 1)
    QtTestObject obj;
    QtTestObject *const nullTestObject = nullptr;

    QString t1("1");

    // Test member functions
    QVERIFY(!QMetaObject::invokeMethod(nullTestObject, &QtTestObject::sl0));
    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::sl0));
    QCOMPARE(obj.slotResult, QString("sl0"));

    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::testSender));
    QCOMPARE(obj.slotResult, QString("0x0"));

    qint64 return64 = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::sl14, &return64));
    QCOMPARE(return64, Q_INT64_C(123456789)*123456789);
    QCOMPARE(obj.slotResult, QString("sl14"));

    // signals
    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::sig0));
    QCOMPARE(obj.slotResult, QString("sl0"));

    // Test function pointers
    QVERIFY(!QMetaObject::invokeMethod(0, &testFunction));
    QVERIFY(QMetaObject::invokeMethod(&obj, &testFunction));

    QVERIFY(!QMetaObject::invokeMethod(0, &QtTestObject::staticFunction0));
    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::staticFunction0));
    QCOMPARE(QtTestObject::staticResult, QString("staticFunction0"));

    return64 = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::staticFunction1, &return64));
    QCOMPARE(return64, Q_INT64_C(123456789)*123456789);
    QCOMPARE(QtTestObject::staticResult, QString("staticFunction1"));

    // Test lambdas
    QCOMPARE(countedStructObjectsCount, 0);
    {
        CountedStruct str;
        QVERIFY(QMetaObject::invokeMethod(&obj, [str, &t1, &obj]() { obj.sl1(t1); }));
        QCOMPARE(obj.slotResult, QString("sl1:1"));
    }
    QCOMPARE(countedStructObjectsCount, 0);
    {
        CountedStruct str;
        QString exp;
        QVERIFY(QMetaObject::invokeMethod(
            &obj, [str, &obj]() -> QString { return obj.sl1("bubu"); }, &exp));
        QCOMPARE(exp, QString("yessir"));
        QCOMPARE(obj.slotResult, QString("sl1:bubu"));
    }
    QCOMPARE(countedStructObjectsCount, 0);
#if defined(__cpp_init_captures) && QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    {
        CountedStruct str;
        std::unique_ptr<int> ptr( new int );
        QVERIFY(QMetaObject::invokeMethod(&obj, [str, &t1, &obj, p = std::move(ptr)]() { obj.sl1(t1); }));
        QCOMPARE(obj.slotResult, QString("sl1:1"));
    }
    QCOMPARE(countedStructObjectsCount, 0);
#endif
#endif
}

void tst_QMetaObject::invokeQueuedMetaMember()
{
    QtTestObject obj;

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl0", Qt::QueuedConnection));
    QVERIFY(obj.slotResult.isEmpty());
    qApp->processEvents(QEventLoop::AllEvents);
    QCOMPARE(obj.slotResult, QString("sl0"));
    obj.slotResult = QString();

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl1", Qt::QueuedConnection, Q_ARG(QString, QString("hallo"))));
    QVERIFY(obj.slotResult.isEmpty());
    qApp->processEvents(QEventLoop::AllEvents);
    QCOMPARE(obj.slotResult, QString("sl1:hallo"));
    obj.slotResult = QString();

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl9", Qt::QueuedConnection, Q_ARG(QString, "1"), Q_ARG(QString, "2"),
                       Q_ARG(QString, "3"), Q_ARG(QString, "4"), Q_ARG(QString, "5"),
                       Q_ARG(QString, "6"), Q_ARG(QString, "7"), Q_ARG(QString, "8"),
                       Q_ARG(QString, "9")));
    QVERIFY(obj.slotResult.isEmpty());
    qApp->processEvents(QEventLoop::AllEvents);
    QCOMPARE(obj.slotResult, QString("sl9:123456789"));

    // signals

    obj.slotResult.clear();
    QVERIFY(QMetaObject::invokeMethod(&obj, "sig0", Qt::QueuedConnection));
    QVERIFY(obj.slotResult.isEmpty());
    qApp->processEvents(QEventLoop::AllEvents);
    QCOMPARE(obj.slotResult, QString("sl0"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sig1", Qt::QueuedConnection, Q_ARG(QString, "gogo")));
    qApp->processEvents(QEventLoop::AllEvents);
    QCOMPARE(obj.slotResult, QString("sl1:gogo"));

    QString exp;
    QTest::ignoreMessage(QtWarningMsg, "QMetaMethod::invoke: Unable to invoke methods with return values in queued connections");
    QVERIFY(!QMetaObject::invokeMethod(&obj, "sig1", Qt::QueuedConnection, Q_RETURN_ARG(QString, exp),
                              Q_ARG(QString, "nono")));

    qint64 ll1 = -1;
    quint64 ll2 = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj,
                                      "testLongLong",
                                      Qt::QueuedConnection,
                                      Q_ARG(qint64, ll1),
                                      Q_ARG(quint64, ll2)));
    qApp->processEvents(QEventLoop::AllEvents);
    QCOMPARE(obj.slotResult, QString("testLongLong:-1,0"));

    obj.slotResult.clear();
    {
        MyUnregisteredType t;
        QTest::ignoreMessage(QtWarningMsg, "QMetaMethod::invoke: Unable to handle unregistered datatype 'MyUnregisteredType'");
        QVERIFY(!QMetaObject::invokeMethod(&obj, "slotWithUnregisteredParameterType", Qt::QueuedConnection, Q_ARG(MyUnregisteredType, t)));
        QVERIFY(obj.slotResult.isEmpty());
    }
}

void tst_QMetaObject::invokeQueuedPointer()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 1)
    QtTestObject obj;

    // Test member function
    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::sl0, Qt::QueuedConnection));
    QVERIFY(obj.slotResult.isEmpty());
    qApp->processEvents(QEventLoop::AllEvents);
    QCOMPARE(obj.slotResult, QString("sl0"));

    // signals
    obj.slotResult.clear();
    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::sig0, Qt::QueuedConnection));
    QVERIFY(obj.slotResult.isEmpty());
    qApp->processEvents(QEventLoop::AllEvents);
    QCOMPARE(obj.slotResult, QString("sl0"));

    // Test function pointers
    QtTestObject::staticResult.clear();
    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::staticFunction0, Qt::QueuedConnection));
    QVERIFY(QtTestObject::staticResult.isEmpty());
    qApp->processEvents(QEventLoop::AllEvents);
    QCOMPARE(QtTestObject::staticResult, QString("staticFunction0"));

    // Test lambda
    QCOMPARE(countedStructObjectsCount, 0);
    {
        CountedStruct str;
        obj.slotResult.clear();
        QVERIFY(
            QMetaObject::invokeMethod(&obj, [str, &obj]() { obj.sl0(); }, Qt::QueuedConnection));
        QVERIFY(obj.slotResult.isEmpty());
        qApp->processEvents(QEventLoop::AllEvents);
        QCOMPARE(obj.slotResult, QString("sl0"));
    }
    QCOMPARE(countedStructObjectsCount, 0);
    {
        CountedStruct str;
        qint32 var = 0;
        QTest::ignoreMessage(QtWarningMsg,
                             "QMetaObject::invokeMethod: Unable to invoke methods with return "
                             "values in queued connections");
        QVERIFY(!QMetaObject::invokeMethod(&obj, [str]() -> qint32 { return 1; },
                                           Qt::QueuedConnection, &var));
        QCOMPARE(var, 0);
    }
    QCOMPARE(countedStructObjectsCount, 0);
#endif
}


void tst_QMetaObject::invokeBlockingQueuedMetaMember()
{
    QThread t;
    t.start();
    QtTestObject obj;
    obj.moveToThread(&t);

    QString t1("1"); QString t2("2"); QString t3("3"); QString t4("4"); QString t5("5");
    QString t6("6"); QString t7("7"); QString t8("8"); QString t9("9"); QString t10("X");

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl1", Qt::BlockingQueuedConnection, Q_ARG(QString, t1)));
    QCOMPARE(obj.slotResult, QString("sl1:1"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl2", Qt::BlockingQueuedConnection, Q_ARG(const QString, t1), Q_ARG(QString, t2)));
    QCOMPARE(obj.slotResult, QString("sl2:12"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl3", Qt::BlockingQueuedConnection, Q_ARG(QString, t1), Q_ARG(QString, t2), Q_ARG(QString, t3)));
    QCOMPARE(obj.slotResult, QString("sl3:123"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl4", Qt::BlockingQueuedConnection, Q_ARG(QString, t1), Q_ARG(QString, t2),
                                      Q_ARG(QString, t3), Q_ARG(QString, t4)));
    QCOMPARE(obj.slotResult, QString("sl4:1234"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl5", Qt::BlockingQueuedConnection, Q_ARG(QString, t1), Q_ARG(QString, t2),
                                      Q_ARG(QString, t3), Q_ARG(QString, t4), Q_ARG(QString, "5")));
    QCOMPARE(obj.slotResult, QString("sl5:12345"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl6", Qt::BlockingQueuedConnection, Q_ARG(QString, t1), Q_ARG(QString, t2),
                                      Q_ARG(QString, t3), Q_ARG(QString, t4), Q_ARG(QString, t5), Q_ARG(QString, t6)));
    QCOMPARE(obj.slotResult, QString("sl6:123456"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl7", Qt::BlockingQueuedConnection, Q_ARG(QString, t1), Q_ARG(QString, t2),
                                      Q_ARG(QString, t3), Q_ARG(QString, t4), Q_ARG(QString, t5), Q_ARG(QString, t6),
                                      Q_ARG(QString, t7)));
    QCOMPARE(obj.slotResult, QString("sl7:1234567"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl8", Qt::BlockingQueuedConnection, Q_ARG(QString, t1), Q_ARG(QString, t2),
                                      Q_ARG(QString, t3), Q_ARG(QString, t4), Q_ARG(QString, t5), Q_ARG(QString, t6),
                                      Q_ARG(QString, t7), Q_ARG(QString, t8)));
    QCOMPARE(obj.slotResult, QString("sl8:12345678"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl9", Qt::BlockingQueuedConnection, Q_ARG(QString, t1), Q_ARG(QString, t2),
                                      Q_ARG(QString, t3), Q_ARG(QString, t4), Q_ARG(QString, t5), Q_ARG(QString, t6),
                                      Q_ARG(QString, t7), Q_ARG(QString, t8), Q_ARG(QString, t9)));
    QCOMPARE(obj.slotResult, QString("sl9:123456789"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sl11", Qt::BlockingQueuedConnection));
    QCOMPARE(obj.slotResult, QString("sl11"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "testSender", Qt::BlockingQueuedConnection));
    QCOMPARE(obj.slotResult, QString("0x0"));

    QString refStr("whatever");
    QVERIFY(QMetaObject::invokeMethod(&obj, "testReference", Qt::BlockingQueuedConnection, QGenericArgument("QString&", &refStr)));
    QCOMPARE(obj.slotResult, QString("testReference:whatever"));
    QCOMPARE(refStr, QString("gotcha"));

    qint64 ll1 = -1;
    quint64 ll2 = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj,
                                      "testLongLong",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(qint64, ll1),
                                      Q_ARG(quint64, ll2)));
    QCOMPARE(obj.slotResult, QString("testLongLong:-1,0"));

    QString exp;
    QVERIFY(QMetaObject::invokeMethod(&obj, "sl1", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QString, exp), Q_ARG(QString, "bubu")));
    QCOMPARE(exp, QString("yessir"));
    QCOMPARE(obj.slotResult, QString("sl1:bubu"));

    QObject *ptr = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj, "sl11", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QObject*,ptr)));
    QCOMPARE(ptr, (QObject *)&obj);
    QCOMPARE(obj.slotResult, QString("sl11"));
    // try again with a space:
    ptr = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj, "sl11", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QObject * , ptr)));
    QCOMPARE(ptr, (QObject *)&obj);
    QCOMPARE(obj.slotResult, QString("sl11"));

    const char *ptr2 = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj, "sl12", Qt::BlockingQueuedConnection, Q_RETURN_ARG(const char*, ptr2)));
    QVERIFY(ptr2 != 0);
    QCOMPARE(obj.slotResult, QString("sl12"));
    // try again with a space:
    ptr2 = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj, "sl12", Qt::BlockingQueuedConnection, Q_RETURN_ARG(char const * , ptr2)));
    QVERIFY(ptr2 != 0);
    QCOMPARE(obj.slotResult, QString("sl12"));

    // test w/ template args
    QList<QString> returnValue, argument;
    argument << QString("one") << QString("two") << QString("three");
    QVERIFY(QMetaObject::invokeMethod(&obj, "sl13", Qt::BlockingQueuedConnection,
                                      Q_RETURN_ARG(QList<QString>, returnValue),
                                      Q_ARG(QList<QString>, argument)));
    QCOMPARE(returnValue, argument);
    QCOMPARE(obj.slotResult, QString("sl13"));

    //test signals
    QVERIFY(QMetaObject::invokeMethod(&obj, "sig0", Qt::BlockingQueuedConnection));
    QCOMPARE(obj.slotResult, QString("sl0"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "sig1", Qt::BlockingQueuedConnection, Q_ARG(QString, "baba")));
    QCOMPARE(obj.slotResult, QString("sl1:baba"));

    exp.clear();
    QVERIFY(QMetaObject::invokeMethod(&obj, "sig1", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QString, exp), Q_ARG(QString, "hehe")));
    QCOMPARE(exp, QString("yessir"));
    QCOMPARE(obj.slotResult, QString("sl1:hehe"));

    QTest::ignoreMessage(QtWarningMsg, "QMetaObject::invokeMethod: No such method QtTestObject::doesNotExist()");
    QVERIFY(!QMetaObject::invokeMethod(&obj, "doesNotExist", Qt::BlockingQueuedConnection));
    QTest::ignoreMessage(QtWarningMsg, "QMetaObject::invokeMethod: No such method QtTestObject::sl1(QString)(QString)");
    QVERIFY(!QMetaObject::invokeMethod(&obj, "sl1(QString)", Qt::BlockingQueuedConnection, Q_ARG(QString, "arg")));
    QTest::ignoreMessage(QtWarningMsg, "QMetaObject::invokeMethod: No such method QtTestObject::sl3(QString)\n"
                         "Candidates are:\n    sl3(QString,QString,QString)");
    QVERIFY(!QMetaObject::invokeMethod(&obj, "sl3", Qt::BlockingQueuedConnection, Q_ARG(QString, "arg")));
    QTest::ignoreMessage(QtWarningMsg, "QMetaObject::invokeMethod: No such method QtTestObject::sl1(QString,QString,QString)\n"
                         "Candidates are:\n    sl1(QString)");
    QVERIFY(!QMetaObject::invokeMethod(&obj, "sl1", Qt::BlockingQueuedConnection, Q_ARG(QString, "arg"), Q_ARG(QString, "arg"), Q_ARG(QString, "arg")));

    //should not have changed since last test.
    QCOMPARE(exp, QString("yessir"));
    QCOMPARE(obj.slotResult, QString("sl1:hehe"));

    QVERIFY(QMetaObject::invokeMethod(&obj, "moveToThread", Qt::BlockingQueuedConnection, Q_ARG(QThread*, QThread::currentThread())));
    t.quit();
    QVERIFY(t.wait());

}

void tst_QMetaObject::invokeBlockingQueuedPointer()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 1)
    QtTestObject *const nullTestObject = nullptr;

    QThread t;
    t.start();
    QtTestObject obj;
    obj.moveToThread(&t);

    QString t1("1");

    // Test member functions
    QVERIFY(!QMetaObject::invokeMethod(nullTestObject, &QtTestObject::sl0, Qt::BlockingQueuedConnection));
    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::sl0, Qt::BlockingQueuedConnection));
    QCOMPARE(obj.slotResult, QString("sl0"));

    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::testSender, Qt::BlockingQueuedConnection));
    QCOMPARE(obj.slotResult, QString("0x0"));

    // return qint64
    qint64 return64 = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::sl14, Qt::BlockingQueuedConnection,
                                      &return64));
    QCOMPARE(return64, Q_INT64_C(123456789)*123456789);
    QCOMPARE(obj.slotResult, QString("sl14"));

    //test signals
    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::sig0, Qt::BlockingQueuedConnection));
    QCOMPARE(obj.slotResult, QString("sl0"));

    // Test function pointers
    QVERIFY(!QMetaObject::invokeMethod(0, &testFunction, Qt::BlockingQueuedConnection));
    QVERIFY(QMetaObject::invokeMethod(&obj, &testFunction, Qt::BlockingQueuedConnection));

    QVERIFY(!QMetaObject::invokeMethod(0, &QtTestObject::staticFunction0, Qt::BlockingQueuedConnection));
    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::staticFunction0, Qt::BlockingQueuedConnection));
    QCOMPARE(QtTestObject::staticResult, QString("staticFunction0"));

    return64 = 0;
    QVERIFY(QMetaObject::invokeMethod(&obj, &QtTestObject::staticFunction1, Qt::BlockingQueuedConnection, &return64));
    QCOMPARE(return64, Q_INT64_C(123456789)*123456789);
    QCOMPARE(QtTestObject::staticResult, QString("staticFunction1"));

    // Test lambdas
    QCOMPARE(countedStructObjectsCount, 0);
    {
        CountedStruct str;
        QVERIFY(QMetaObject::invokeMethod(&obj, [str, &obj, &t1]() { obj.sl1(t1); },
                                          Qt::BlockingQueuedConnection));
        QCOMPARE(obj.slotResult, QString("sl1:1"));
    }
    {
        CountedStruct str;
        QString exp;
        QVERIFY(QMetaObject::invokeMethod(&obj,
                                          [&obj, str]() -> QString { return obj.sl1("bubu"); },
                                          Qt::BlockingQueuedConnection, &exp));
        QCOMPARE(exp, QString("yessir"));
        QCOMPARE(obj.slotResult, QString("sl1:bubu"));
    }
#if defined(__cpp_init_captures) && QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    {
        std::unique_ptr<int> ptr(new int);
        QVERIFY(QMetaObject::invokeMethod(&obj,
                                          [&obj, p = std::move(ptr)]() { return obj.sl1("hehe"); },
                                          Qt::BlockingQueuedConnection));
        QCOMPARE(obj.slotResult, QString("sl1:hehe"));
    }
#endif
    QVERIFY(QMetaObject::invokeMethod(&obj, [&](){obj.moveToThread(QThread::currentThread());}, Qt::BlockingQueuedConnection));
    t.quit();
    QVERIFY(t.wait());
    QCOMPARE(countedStructObjectsCount, 0);
#endif
}


void tst_QMetaObject::qtMetaObjectInheritance()
{
    QVERIFY(!QObject::staticMetaObject.superClass());
    QCOMPARE(QSortFilterProxyModel::staticMetaObject.indexOfEnumerator("Qt::CaseSensitivity"), -1);
    QCOMPARE(QSortFilterProxyModel::staticMetaObject.indexOfEnumerator("CaseSensitivity"), -1);
    int indexOfSortCaseSensitivity = QSortFilterProxyModel::staticMetaObject.indexOfProperty("sortCaseSensitivity");
    QVERIFY(indexOfSortCaseSensitivity != -1);
    QMetaProperty sortCaseSensitivity = QSortFilterProxyModel::staticMetaObject.property(indexOfSortCaseSensitivity);
    QVERIFY(sortCaseSensitivity.isValid());
    QCOMPARE(sortCaseSensitivity.enumerator().name(), "CaseSensitivity");
}

struct MyType
{
    int i1, i2, i3;
};
W_REGISTER_ARGTYPE(MyType)

typedef QString CustomString;


class QtTestCustomObject: public QObject
{
    W_OBJECT(QtTestCustomObject)
    friend class tst_QMetaObject;
public:
    QtTestCustomObject(): QObject(), sum(0) {}

public slots:
    void sl1(MyType myType);
    W_SLOT(sl1)

signals:
    void sig_custom(const CustomString &string)
    W_SIGNAL(sig_custom,string)

public:
    int sum;
};

W_OBJECT_IMPL(QtTestCustomObject)

void QtTestCustomObject::sl1(MyType myType)
{
    sum = myType.i1 + myType.i2 + myType.i3;
}

void tst_QMetaObject::invokeCustomTypes()
{
    QtTestCustomObject obj;
    MyType tp = {1, 1, 1};

    QCOMPARE(obj.sum, 0);
    QVERIFY(QMetaObject::invokeMethod(&obj, "sl1", Q_ARG(MyType, tp)));
    QCOMPARE(obj.sum, 3);
}

namespace NamespaceWithConstructibleClass
{

class ConstructibleClass : public QObject
{
    W_OBJECT(ConstructibleClass)
public:
    ConstructibleClass(QObject *parent = 0)
        : QObject(parent) {}
    W_CONSTRUCTOR(QObject*)
};

}

W_OBJECT_IMPL(NamespaceWithConstructibleClass::ConstructibleClass)

void tst_QMetaObject::invokeMetaConstructor()
{
    const QMetaObject *mo = &QtTestObject::staticMetaObject;
    {
        QObject *obj = mo->newInstance();
        QVERIFY(!obj);
    }
    {
        QtTestObject obj;
        QObject *obj2 = mo->newInstance(Q_ARG(QObject*, &obj));
        QVERIFY(obj2 != 0);
        QCOMPARE(obj2->parent(), (QObject*)&obj);
        QVERIFY(qobject_cast<QtTestObject*>(obj2) != 0);
    }
    // class in namespace
    const QMetaObject *nsmo = &NamespaceWithConstructibleClass::ConstructibleClass::staticMetaObject;
    {
        QtTestObject obj;
        QObject *obj2 = nsmo->newInstance(Q_ARG(QObject*, &obj));
        QVERIFY(obj2 != 0);
        QCOMPARE(obj2->parent(), (QObject*)&obj);
        QVERIFY(qobject_cast<NamespaceWithConstructibleClass::ConstructibleClass*>(obj2) != 0);
    }
    // gadget shouldn't return a valid pointer
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    {
        QCOMPARE(MyGadget::staticMetaObject.constructorCount(), 1);
        QTest::ignoreMessage(QtWarningMsg, "QMetaObject::newInstance: type MyGadget does not inherit QObject");
        QVERIFY(!MyGadget::staticMetaObject.newInstance());
    }
#endif
}

void tst_QMetaObject::invokeTypedefTypes()
{
    qRegisterMetaType<CustomString>("CustomString");
    QtTestCustomObject obj;
    QSignalSpy spy(&obj, &QtTestCustomObject::sig_custom);
    QVERIFY(spy.isValid());

    QCOMPARE(spy.count(), 0);
    CustomString arg("hello");
    QVERIFY(QMetaObject::invokeMethod(&obj, "sig_custom", Q_ARG(CustomString, arg)));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(arg));
}

void tst_QMetaObject::invokeException()
{
#ifndef QT_NO_EXCEPTIONS
    QtTestObject obj;
    QCOMPARE(countedStructObjectsCount, 0);
    try {
        CountedStruct s;
        QVERIFY(QMetaObject::invokeMethod(&obj, "throwingSlot", Q_RETURN_ARG(CountedStruct, s),
                                          Q_ARG(CountedStruct, s), Q_ARG(CountedStruct, s)));
        QFAIL("Did not throw");
    } catch(ObjectException &) {}
    QCOMPARE(countedStructObjectsCount, 0);
#else
    QSKIP("Needs exceptions");
#endif
}

void tst_QMetaObject::invokeQueuedAutoRegister()
{
    QtTestObject obj;

    auto shared = QSharedPointer<QtTestObject>::create(QStringLiteral("myShared-"));

    QVERIFY(QMetaObject::invokeMethod(
        &obj, "slotWithRegistrableArgument", Qt::QueuedConnection,
        Q_ARG(QtTestObject *, shared.data()), Q_ARG(QPointer<QtTestObject>, shared.data()),
        Q_ARG(QSharedPointer<QtTestObject>, shared), Q_ARG(QWeakPointer<QtTestObject>, shared),
        Q_ARG(QVector<QtTestObject *>, QVector<QtTestObject *>()),
        Q_ARG(QList<QtTestObject *>, QList<QtTestObject *>())));
    QVERIFY(obj.slotResult.isEmpty());
    qApp->processEvents(QEventLoop::AllEvents);
    QCOMPARE(obj.slotResult,
             QString("slotWithRegistrableArgument:myShared-myShared-myShared-myShared-00"));
}

void tst_QMetaObject::normalizedSignature_data()
{
    QTest::addColumn<QString>("signature");
    QTest::addColumn<QString>("result");

    QTest::newRow("function") << "void foo()" << "void foo()";
    QTest::newRow("spaces") << " void  foo( ) " << "void foo()";
    QTest::newRow("void") << "void foo(void)" << "void foo()";
    QTest::newRow("void spaces") << "void foo( void )" << "void foo()";
    QTest::newRow("void*") << "void foo(void*)" << "void foo(void*)";
    QTest::newRow("void* spaces") << "void foo( void * )" << "void foo(void*)";
    QTest::newRow("function ptr") << "void foo(void(*)(void))" << "void foo(void(*)())";
    QTest::newRow("function ptr spaces") << "void foo( void ( * ) ( void ))" << "void foo(void(*)())";
    QTest::newRow("function ptr void*") << "void foo(void(*)(void*))" << "void foo(void(*)(void*))";
    QTest::newRow("function ptr void* spaces") << "void foo( void ( * ) ( void * ))" << "void foo(void(*)(void*))";
    QTest::newRow("template args") << " void  foo( QMap<a, a>, QList<b>) "
                                   << "void foo(QMap<a,a>,QList<b>)";
    QTest::newRow("void template args") << " void  foo( Foo<void>, Bar<void> ) "
                                   << "void foo(Foo<void>,Bar<void>)";
    QTest::newRow("void* template args") << " void  foo( Foo<void*>, Bar<void *> ) "
                                   << "void foo(Foo<void*>,Bar<void*>)";
    QTest::newRow("rettype") << "QList<int, int> foo()" << "QList<int,int>foo()";
    QTest::newRow("rettype void template") << "Foo<void> foo()" << "Foo<void>foo()";
    QTest::newRow("const rettype") << "const QString *foo()" << "const QString*foo()";
    QTest::newRow("const ref") << "const QString &foo()" << "const QString&foo()";
    QTest::newRow("reference") << "QString &foo()" << "QString&foo()";
    QTest::newRow("const1") << "void foo(QString const *)" << "void foo(const QString*)";
    QTest::newRow("const2") << "void foo(QString * const)" << "void foo(QString*const)";
    QTest::newRow("const3") << "void foo(QString const &)" << "void foo(QString)";
    QTest::newRow("const4") << "void foo(const int)" << "void foo(int)";
    QTest::newRow("const5") << "void foo(const int, int const, const int &, int const &)"
                            << "void foo(int,int,int,int)";
    QTest::newRow("const6") << "void foo(QList<const int>)" << "void foo(QList<const int>)";
    QTest::newRow("const7") << "void foo(QList<const int*>)" << "void foo(QList<const int*>)";
    QTest::newRow("const8") << "void foo(QList<int const*>)" << "void foo(QList<const int*>)";
    QTest::newRow("const9") << "void foo(const Foo<Bar>)" << "void foo(Foo<Bar>)";
    QTest::newRow("const10") << "void foo(Foo<Bar>const)" << "void foo(Foo<Bar>)";
    QTest::newRow("const11") << "void foo(Foo<Bar> *const)" << "void foo(Foo<Bar>*const)";
    QTest::newRow("const12") << "void foo(Foo<Bar>const*const *const)" << "void foo(Foo<Bar>*const*const)";
    QTest::newRow("const13") << "void foo(const Foo<Bar>&)" << "void foo(Foo<Bar>)";
    QTest::newRow("const14") << "void foo(Foo<Bar>const&)" << "void foo(Foo<Bar>)";

    QTest::newRow("invalid1") << "a( b" << "a(b";
}

void tst_QMetaObject::normalizedSignature()
{
    QFETCH(QString, signature);
    QFETCH(QString, result);

    QCOMPARE(QMetaObject::normalizedSignature(signature.toLatin1()), result.toLatin1());
}

void tst_QMetaObject::normalizedType_data()
{
    QTest::addColumn<QString>("type");
    QTest::addColumn<QString>("result");

    QTest::newRow("simple") << "int" << "int";
    QTest::newRow("white") << "  int  " << "int";
    QTest::newRow("const1") << "int const *" << "const int*";
    QTest::newRow("const2") << "const int *" << "const int*";
    QTest::newRow("template1") << "QList<int const *>" << "QList<const int*>";
    QTest::newRow("template2") << "QList<const int *>" << "QList<const int*>";
    QTest::newRow("template3") << "QMap<QString, int>" << "QMap<QString,int>";
    QTest::newRow("template4") << "const QMap<QString, int> &" << "QMap<QString,int>";
    QTest::newRow("template5") << "QList< ::Foo::Bar>" << "QList< ::Foo::Bar>";
    QTest::newRow("template6") << "QList<::Foo::Bar>" << "QList<::Foo::Bar>";
    QTest::newRow("template7") << "QList<QList<int> >" << "QList<QList<int> >";
    QTest::newRow("template8") << "QMap<const int, const short*>" << "QMap<const int,const short*>";
    QTest::newRow("template9") << "QPair<const QPair<int, int const *> , QPair<QHash<int, const char*>  >  >" << "QPair<const QPair<int,const int*>,QPair<QHash<int,const char*> > >";
    QTest::newRow("value1") << "const QString &" << "QString";
    QTest::newRow("value2") << "QString const &" << "QString";
    QTest::newRow("constInName1") << "constconst" << "constconst";
    QTest::newRow("constInName2") << "constconst*" << "constconst*";
    QTest::newRow("constInName3") << "const constconst&" << "constconst";
    QTest::newRow("constInName4") << "constconst const*const" << "constconst*const";
    QTest::newRow("class") << "const class foo&" << "foo";
    QTest::newRow("struct") << "const struct foo*" << "const foo*";
    QTest::newRow("struct2") << "struct foo const*" << "const foo*";
    QTest::newRow("enum") << "enum foo" << "foo";
    QTest::newRow("void") << "void" << "void";
}

void tst_QMetaObject::normalizedType()
{
    QFETCH(QString, type);
    QFETCH(QString, result);

    QCOMPARE(QMetaObject::normalizedType(type.toLatin1()), result.toLatin1());
}

void tst_QMetaObject::customPropertyType()
{
    QMetaProperty prop = metaObject()->property(metaObject()->indexOfProperty("value3"));

    QCOMPARE(prop.type(), QVariant::UserType);
    QCOMPARE(prop.userType(), 0);

    qRegisterMetaType<MyStruct>("MyStruct");
    QCOMPARE(prop.userType(), QMetaType::type("MyStruct"));

    prop = metaObject()->property(metaObject()->indexOfProperty("value4"));
    QCOMPARE(prop.type(), QVariant::List);

    prop = metaObject()->property(metaObject()->indexOfProperty("value5"));
    QCOMPARE(prop.type(), QVariant::List);
}

void tst_QMetaObject::checkScope_data()
{
    QTest::addColumn<QObject *>("object");
    QTest::addColumn<QByteArray>("name");

    static MyNamespace::MyClass obj1;
    static MyNamespace::MyClass2 obj2;

    QTest::newRow("MyClass") << static_cast<QObject*>(&obj1) << QByteArray("MyClass");
    QTest::newRow("MyClass2") << static_cast<QObject*>(&obj2) << QByteArray("MyClass2");

}


void tst_QMetaObject::checkScope()
{
    QFETCH(QObject *, object);
    QFETCH(QByteArray, name);
    QObject  &obj = *object;
    bool ok;

    const QMetaObject *mo = obj.metaObject();
    QMetaEnum me = mo->enumerator(mo->indexOfEnumerator("MyEnum"));
    QVERIFY(me.isValid());
    QVERIFY(!me.isFlag());
    QCOMPARE(QByteArray(me.scope()), QByteArray("MyNamespace::" + name));
    QCOMPARE(me.keyToValue("MyNamespace::" + name + "::MyEnum2", &ok), 1);
    QCOMPARE(ok, true);
    QCOMPARE(me.keyToValue(name + "::MyEnum2", &ok), -1);
    QCOMPARE(ok, false);
    QCOMPARE(me.keyToValue("MyNamespace::MyEnum2", &ok), -1);
    QCOMPARE(ok, false);
    QCOMPARE(me.keyToValue("MyEnum2", &ok), 1);
    QCOMPARE(ok, true);
    QCOMPARE(me.keyToValue("MyEnum", &ok), -1);
    QCOMPARE(ok, false);
    QCOMPARE(QLatin1String(me.valueToKey(1)), QLatin1String("MyEnum2"));

    QMetaEnum me2 = mo->enumerator(mo->indexOfEnumerator("MyAnotherEnum"));
    QVERIFY(me2.isValid());
    QVERIFY(!me2.isFlag());
    QCOMPARE(me2.keyToValue("MyAnotherEnum1", &ok), 1);
    QCOMPARE(ok, true);
    QCOMPARE(me2.keyToValue("MyAnotherEnum2", &ok), 2);
    QCOMPARE(ok, true);
    QCOMPARE(me2.keyToValue("MyAnotherEnum3", &ok), -1);
    QCOMPARE(ok, true);
    QCOMPARE(me2.keyToValue("MyAnotherEnum", &ok), -1);
    QCOMPARE(ok, false);

    QMetaEnum mf = mo->enumerator(mo->indexOfEnumerator("MyFlags"));
    QVERIFY(mf.isValid());
    QVERIFY(mf.isFlag());
    QCOMPARE(QByteArray(mf.scope()), QByteArray("MyNamespace::" + name));
    QCOMPARE(mf.keysToValue("MyNamespace::" + name + "::MyFlag2", &ok), 2);
    QCOMPARE(ok, true);
    QCOMPARE(mf.keysToValue(name + "::MyFlag2", &ok), -1);
    QCOMPARE(ok, false);
    QCOMPARE(mf.keysToValue("MyNamespace::MyFlag2", &ok), -1);
    QCOMPARE(ok, false);
    QCOMPARE(mf.keysToValue("MyFlag2", &ok), 2);
    QCOMPARE(ok, true);
    QCOMPARE(mf.keysToValue("MyFlag", &ok), -1);
    QCOMPARE(ok, false);
    QCOMPARE(QLatin1String(mf.valueToKey(2)), QLatin1String("MyFlag2"));
    QCOMPARE(mf.keysToValue("MyNamespace::" + name + "::MyFlag1|MyNamespace::" + name + "::MyFlag2", &ok), 3);
    QCOMPARE(ok, true);
    QCOMPARE(mf.keysToValue(name + "::MyFlag1|" + name + "::MyFlag2", &ok), -1);
    QCOMPARE(ok, false);
    QCOMPARE(mf.keysToValue("MyNamespace::MyFlag1|MyNamespace::MyFlag2", &ok), -1);
    QCOMPARE(ok, false);
    QCOMPARE(mf.keysToValue("MyFlag1|MyFlag2", &ok), 3);
    QCOMPARE(ok, true);
    QCOMPARE(mf.keysToValue("MyFlag2|MyFlag2", &ok), 2);
    QCOMPARE(ok, true);
    QCOMPARE(mf.keysToValue("MyFlag1|MyNamespace::" + name + "::MyFlag2", &ok), 3);
    QCOMPARE(ok, true);
    QCOMPARE(mf.keysToValue("MyNamespace::" + name + "::MyFlag2|MyNamespace::" + name + "::MyFlag2", &ok), 2);
    QCOMPARE(ok, true);
    QCOMPARE(QLatin1String(mf.valueToKeys(3)), QLatin1String("MyFlag1|MyFlag2"));
}

void tst_QMetaObject::propertyNotify()
{
    const QMetaObject *mo = metaObject();

    QMetaProperty prop = mo->property(mo->indexOfProperty("value6"));
    QVERIFY(prop.isValid());
    QVERIFY(prop.hasNotifySignal());
    QMetaMethod signal = prop.notifySignal();
    QCOMPARE(signal.methodSignature(), QByteArray("value6Changed()"));

    prop = mo->property(mo->indexOfProperty("value7"));
    QVERIFY(prop.isValid());
    QVERIFY(prop.hasNotifySignal());
    signal = prop.notifySignal();
    QCOMPARE(signal.methodSignature(), QByteArray("value7Changed(QString)"));

    prop = mo->property(mo->indexOfProperty("value8"));
    QVERIFY(prop.isValid());
    QVERIFY(!prop.hasNotifySignal());
    signal = prop.notifySignal();
    QCOMPARE(signal.methodSignature(), QByteArray());

    prop = mo->property(mo->indexOfProperty("value"));
    QVERIFY(prop.isValid());
    QVERIFY(!prop.hasNotifySignal());
    signal = prop.notifySignal();
    QCOMPARE(signal.methodSignature(), QByteArray());
}

void tst_QMetaObject::propertyConstant()
{
    const QMetaObject *mo = metaObject();

    QMetaProperty prop = mo->property(mo->indexOfProperty("value8"));
    QVERIFY(prop.isValid());
    QVERIFY(!prop.isConstant());

    prop = mo->property(mo->indexOfProperty("value9"));
    QVERIFY(prop.isValid());
    QVERIFY(prop.isConstant());
}

void tst_QMetaObject::propertyFinal()
{
    const QMetaObject *mo = metaObject();

    QMetaProperty prop = mo->property(mo->indexOfProperty("value10"));
    QVERIFY(prop.isValid());
    QVERIFY(prop.isFinal());

    prop = mo->property(mo->indexOfProperty("value9"));
    QVERIFY(prop.isValid());
    QVERIFY(!prop.isFinal());
}

class ClassInfoTestObjectA : public QObject
{
    W_OBJECT(ClassInfoTestObjectA)
    W_CLASSINFO("Author", "Christopher Pike")
};

W_OBJECT_IMPL(ClassInfoTestObjectA)

class ClassInfoTestObjectB : public ClassInfoTestObjectA
{
    W_OBJECT(ClassInfoTestObjectB)
};

W_OBJECT_IMPL(ClassInfoTestObjectB)

void tst_QMetaObject::classInfo()
{
    ClassInfoTestObjectB b;
    int index = b.metaObject()->indexOfClassInfo("Author");
    QCOMPARE(index, 0);
    QVERIFY(index <= b.metaObject()->classInfoOffset());
    QCOMPARE(QLatin1String(b.metaObject()->classInfo(index).value()), QLatin1String("Christopher Pike"));
}

void tst_QMetaObject::metaMethod()
{
    QString str("foo");
    QString ret("bar");
    QMetaMethod method;
    QVERIFY(!method.invoke(this));
    QVERIFY(!method.invoke(this, Q_ARG(QString, str)));
    QVERIFY(!method.invoke(this, Q_RETURN_ARG(QString, ret), Q_ARG(QString, str)));
    QCOMPARE(str, QString("foo"));
    QCOMPARE(ret, QString("bar"));

    QtTestObject obj;
    QString t1("1"); QString t2("2"); QString t3("3"); QString t4("4"); QString t5("5");
    QString t6("6"); QString t7("7"); QString t8("8"); QString t9("9"); QString t10("X");

    int index = QtTestObject::staticMetaObject.indexOfMethod("sl5(QString,QString,QString,QString,QString)");
    QVERIFY(index > 0);
    method = QtTestObject::staticMetaObject.method(index);
    //wrong args
    QVERIFY(!method.invoke(&obj, Q_ARG(QString, "1"), Q_ARG(QString, "2"), Q_ARG(QString, "3"), Q_ARG(QString, "4")));
    //QVERIFY(!method.invoke(&obj, Q_ARG(QString, "1"), Q_ARG(QString, "2"), Q_ARG(QString, "3"), Q_ARG(QString, "4"), Q_ARG(QString, "5"), Q_ARG(QString, "6")));
    //QVERIFY(!method.invoke(&obj, Q_ARG(QString, "1"), Q_ARG(QString, "2"), Q_ARG(QString, "3"), Q_ARG(QString, "4"), Q_ARG(int, 5)));
    QVERIFY(!method.invoke(&obj, Q_RETURN_ARG(QString, ret), Q_ARG(QString, "1"), Q_ARG(QString, "2"), Q_ARG(QString, "3"), Q_ARG(QString, "4"), Q_ARG(QString, "5")));

    //wrong object
    //QVERIFY(!method.invoke(this, Q_ARG(QString, "1"), Q_ARG(QString, "2"), Q_ARG(QString, "3"), Q_ARG(QString, "4"), Q_ARG(QString, "5")));
    QVERIFY(!method.invoke(0, Q_ARG(QString, "1"), Q_ARG(QString, "2"), Q_ARG(QString, "3"), Q_ARG(QString, "4"), Q_ARG(QString, "5")));
    QCOMPARE(ret, QString("bar"));
    QCOMPARE(obj.slotResult, QString());

    QVERIFY(method.invoke(&obj, Q_ARG(QString, "1"), Q_ARG(QString, "2"), Q_ARG(QString, "3"), Q_ARG(QString, "4"), Q_ARG(QString, "5")));
    QCOMPARE(obj.slotResult, QString("sl5:12345"));

    index = QtTestObject::staticMetaObject.indexOfMethod("sl13(QList<QString>)");
    QVERIFY(index > 0);
    QMetaMethod sl13 = QtTestObject::staticMetaObject.method(index);
    QList<QString> returnValue, argument;
    argument << QString("one") << QString("two") << QString("three");
    //wrong object
    //QVERIFY(!sl13.invoke(this, Q_RETURN_ARG(QList<QString>, returnValue), Q_ARG(QList<QString>, argument)));
    QVERIFY(!sl13.invoke(0,  Q_RETURN_ARG(QList<QString>, returnValue), Q_ARG(QList<QString>, argument)));
    QCOMPARE(returnValue, QList<QString>());

    QVERIFY(sl13.invoke(&obj, Q_RETURN_ARG(QList<QString>, returnValue), Q_ARG(QList<QString>, argument)));
    QCOMPARE(returnValue, argument);
    QCOMPARE(obj.slotResult, QString("sl13"));
}

void tst_QMetaObject::indexOfMethod_data()
{
    QTest::addColumn<QObject *>("object");
    QTest::addColumn<QByteArray>("name");
    QTest::addColumn<bool>("isSignal");
    QTest::newRow("indexOfMethod_data") << (QObject*)this << QByteArray("indexOfMethod_data()") << false;
    QTest::newRow("deleteLater") << (QObject*)this << QByteArray("deleteLater()") << false;
    QTest::newRow("value6changed") << (QObject*)this << QByteArray("value6Changed()") << true;
    QTest::newRow("value7changed") << (QObject*)this << QByteArray("value7Changed(QString)") << true;
    QTest::newRow("destroyed") << (QObject*)this << QByteArray("destroyed()") << true;
    QTest::newRow("destroyed2") << (QObject*)this << QByteArray("destroyed(QObject*)") << true;
}

void tst_QMetaObject::indexOfMethod()
{
    QFETCH(QObject *, object);
    QFETCH(QByteArray, name);
    QFETCH(bool, isSignal);
    int idx = object->metaObject()->indexOfMethod(name);
    QVERIFY(idx >= 0);
    QCOMPARE(object->metaObject()->method(idx).methodSignature(), name);
    QCOMPARE(object->metaObject()->indexOfSlot(name), isSignal ? -1 : idx);
    QCOMPARE(object->metaObject()->indexOfSignal(name), !isSignal ? -1 : idx);
}

void tst_QMetaObject::indexOfMethodPMF()
{
#define INDEXOFMETHODPMF_HELPER(ObjectType, Name, Arguments)  { \
        int idx = -1; \
        void (ObjectType::*signal)Arguments = &ObjectType::Name; \
        void *signal_p = &signal; \
        void *args[] = { &idx, signal_p, 0}; \
        ObjectType::qt_static_metacall(0, QMetaObject::IndexOfMethod, 0, args); \
        QCOMPARE(ObjectType::staticMetaObject.indexOfMethod(QMetaObject::normalizedSignature(#Name #Arguments)), \
                 ObjectType::staticMetaObject.methodOffset() + idx); \
    }

    INDEXOFMETHODPMF_HELPER(tst_QMetaObject, value7Changed, (const QString&))
    INDEXOFMETHODPMF_HELPER(QtTestObject, sig0, ())
    INDEXOFMETHODPMF_HELPER(QtTestObject, sig10, (QString,QString,QString,QString,QString,QString,QString,QString,QString,QString))
    INDEXOFMETHODPMF_HELPER(QtTestCustomObject, sig_custom, (const CustomString &))
}

#ifdef QT_BUILD_INTERNAL
namespace SignalTestHelper
{
// These functions use the public QMetaObject/QMetaMethod API to implement
// the functionality of the internal API, and are used to check the results.

static int signalCount(const QMetaObject *mo)
{
    int n = 0;
    for (int i = 0; i < mo->methodCount(); ++i) {
        QMetaMethod mm = mo->method(i);
        if (mm.methodType() == QMetaMethod::Signal)
            ++n;
    }
    return n;
}

static int signalOffset(const QMetaObject *mo)
{
    return mo->superClass() ? signalCount(mo->superClass()) : 0;
}

static QMetaMethod signal(const QMetaObject *mo, int index)
{
    int k = 0;
    for (int i = 0; i < mo->methodCount(); ++i) {
        QMetaMethod mm = mo->method(i);
        if (mm.methodType() != QMetaMethod::Signal)
            continue;
        if (k == index)
            return mm;
        ++k;
    }
    return QMetaMethod();
}

static int signalIndex(const QMetaMethod &mm)
{
    int k = mm.methodIndex();
    const QMetaObject *mo = mm.enclosingMetaObject();
    for (int i = 0; i < mm.methodIndex(); ++i) {
        if (mo->method(i).methodType() != QMetaMethod::Signal)
            --k;
    }
    return k;
}

} // namespace SignalTestHelper

void tst_QMetaObject::signalOffset_data()
{
    QTest::addColumn<const QMetaObject *>("metaObject");

    QTest::newRow("QObject") << &QObject::staticMetaObject;
    QTest::newRow("tst_QMetaObject") << &tst_QMetaObject::staticMetaObject;
    QTest::newRow("QtTestObject") << &QtTestObject::staticMetaObject;
}

void tst_QMetaObject::signalOffset()
{
    QFETCH(const QMetaObject *, metaObject);
    QCOMPARE(QMetaObjectPrivate::signalOffset(metaObject),
             SignalTestHelper::signalOffset(metaObject));
}

void tst_QMetaObject::signalCount_data()
{
    signalOffset_data();
}

void tst_QMetaObject::signalCount()
{
    QFETCH(const QMetaObject *, metaObject);
    QCOMPARE(QMetaObjectPrivate::absoluteSignalCount(metaObject),
             SignalTestHelper::signalCount(metaObject));
}

void tst_QMetaObject::signal_data()
{
    QTest::addColumn<const QMetaObject *>("metaObject");
    QTest::addColumn<int>("index");

    struct SignalTestDataHelper
    {
        static void addSignals(const QMetaObject *mo)
        {
            int count = SignalTestHelper::signalCount(mo);
            for (int i = 0; i < count; ++i) {
                QMetaMethod mm = SignalTestHelper::signal(mo, i);
                QByteArray tag(mo->className());
                tag.append("::");
                tag.append(mm.methodSignature());
                QTest::newRow(tag.constData()) << mo << i;
            }
        }
    };

    SignalTestDataHelper::addSignals(&QObject::staticMetaObject);
    SignalTestDataHelper::addSignals(&tst_QMetaObject::staticMetaObject);
    SignalTestDataHelper::addSignals(&QtTestObject::staticMetaObject);
}

void tst_QMetaObject::signal()
{
    QFETCH(const QMetaObject *, metaObject);
    QFETCH(int, index);

    QCOMPARE(QMetaObjectPrivate::signal(metaObject, index),
             SignalTestHelper::signal(metaObject, index));
}

void tst_QMetaObject::signalIndex_data()
{
    signal_data();
}

void tst_QMetaObject::signalIndex()
{
    QFETCH(const QMetaObject *, metaObject);
    QFETCH(int, index);

    QMetaMethod mm = SignalTestHelper::signal(metaObject, index);
    QCOMPARE(QMetaObjectPrivate::signalIndex(mm),
             SignalTestHelper::signalIndex(mm));
}
#endif

void tst_QMetaObject::enumDebugStream_data()
{
    QTest::addColumn<int>("verbosity");
    QTest::addColumn<QString>("normalEnumMsg");
    QTest::addColumn<QString>("scopedEnumMsg");
    QTest::addColumn<QString>("globalEnumMsg");
    QTest::addColumn<QString>("normalFlagMsg");
    QTest::addColumn<QString>("normalFlagsMsg");
    QTest::addColumn<QString>("scopedFlagMsg");
    QTest::addColumn<QString>("scopedFlagsMsg");
    QTest::addColumn<QString>("flagAsEnumMsg");

    QTest::newRow("verbosity=0") << 0
        << "hello MyEnum2 world"
        << "hello MyScopedEnum::Enum3 scoped world"
        << "WindowTitleHint Window Desktop WindowSystemMenuHint"
        << "hello MyFlag1 world"
        << "MyFlag1 MyFlag2|MyFlag3"
        << "MyScopedFlag(MyFlag2)"
        << "MyScopedFlag(MyFlag2|MyFlag3)"
        << "MyFlag1";

    QTest::newRow("verbosity=1") << 1
        << "hello MyEnum::MyEnum2 world"
        << "hello MyScopedEnum::Enum3 scoped world"
        << "WindowType::WindowTitleHint WindowType::Window WindowType::Desktop WindowType::WindowSystemMenuHint"
        << "hello MyFlag(MyFlag1) world"
        << "MyFlag(MyFlag1) MyFlag(MyFlag2|MyFlag3)"
        << "MyScopedFlag(MyFlag2)"
        << "MyScopedFlag(MyFlag2|MyFlag3)"
        << "MyFlag::MyFlag1";

    QTest::newRow("verbosity=2") << 2
        << "hello MyNamespace::MyClass::MyEnum2 world"
        << "hello MyNamespace::MyClass::MyScopedEnum::Enum3 scoped world"
        << "Qt::WindowTitleHint Qt::Window Qt::Desktop Qt::WindowSystemMenuHint"
        << "hello QFlags<MyNamespace::MyClass::MyFlag>(MyFlag1) world"
        << "QFlags<MyNamespace::MyClass::MyFlag>(MyFlag1) QFlags<MyNamespace::MyClass::MyFlag>(MyFlag2|MyFlag3)"
        << "QFlags<MyNamespace::MyClass::MyScopedFlag>(MyFlag2)"
        << "QFlags<MyNamespace::MyClass::MyScopedFlag>(MyFlag2|MyFlag3)"
        << "MyNamespace::MyClass::MyFlag1";

    QTest::newRow("verbosity=3") << 3
        << "hello MyNamespace::MyClass::MyEnum::MyEnum2 world"
        << "hello MyNamespace::MyClass::MyScopedEnum::Enum3 scoped world"
        << "Qt::WindowType::WindowTitleHint Qt::WindowType::Window Qt::WindowType::Desktop Qt::WindowType::WindowSystemMenuHint"
        << "hello QFlags<MyNamespace::MyClass::MyFlag>(MyFlag1) world"
        << "QFlags<MyNamespace::MyClass::MyFlag>(MyFlag1) QFlags<MyNamespace::MyClass::MyFlag>(MyFlag2|MyFlag3)"
        << "QFlags<MyNamespace::MyClass::MyScopedFlag>(MyFlag2)"
        << "QFlags<MyNamespace::MyClass::MyScopedFlag>(MyFlag2|MyFlag3)"
        << "MyNamespace::MyClass::MyFlag::MyFlag1";
}

void tst_QMetaObject::enumDebugStream()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    QTest::ignoreMessage(QtDebugMsg, "hello MyNamespace::MyClass::MyEnum(MyEnum2) world ");
    MyNamespace::MyClass::MyEnum e = MyNamespace::MyClass::MyEnum2;
    qDebug() << "hello" << e << "world";

    QTest::ignoreMessage(QtDebugMsg, "Qt::WindowType(WindowTitleHint) Qt::WindowType(Window) Qt::WindowType(Desktop) Qt::WindowType(WindowSystemMenuHint)");
    qDebug() << Qt::WindowTitleHint << Qt::Window <<Qt::Desktop << Qt::WindowSystemMenuHint;

    QTest::ignoreMessage(QtDebugMsg, "hello QFlags<MyNamespace::MyClass::MyFlags>(MyFlag1) world");
    MyNamespace::MyClass::MyFlags f1 = MyNamespace::MyClass::MyFlag1;
    qDebug() << "hello" << f1 << "world";

    MyNamespace::MyClass::MyFlags f2 = MyNamespace::MyClass::MyFlag2 | MyNamespace::MyClass::MyFlag3;
    QTest::ignoreMessage(QtDebugMsg, "QFlags<MyNamespace::MyClass::MyFlags>(MyFlag1) QFlags<MyNamespace::MyClass::MyFlags>(MyFlag2|MyFlag3)");
    qDebug() << f1 << f2;

#elif QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    QFETCH(int, verbosity);

    QFETCH(QString, normalEnumMsg);
    QFETCH(QString, scopedEnumMsg);
    QFETCH(QString, globalEnumMsg);

    QFETCH(QString, normalFlagMsg);
    QFETCH(QString, normalFlagsMsg);
    QFETCH(QString, scopedFlagMsg);
    QFETCH(QString, scopedFlagsMsg);
    QFETCH(QString, flagAsEnumMsg);

    // Enums
    QTest::ignoreMessage(QtDebugMsg, qPrintable(normalEnumMsg));
    qDebug().verbosity(verbosity) << "hello" << MyNamespace::MyClass::MyEnum2 << "world";

    QTest::ignoreMessage(QtDebugMsg, qPrintable(scopedEnumMsg));
    qDebug().verbosity(verbosity) << "hello" << MyNamespace::MyClass::MyScopedEnum::Enum3 << "scoped world";

    QTest::ignoreMessage(QtDebugMsg, qPrintable(globalEnumMsg));
    qDebug().verbosity(verbosity) << Qt::WindowTitleHint << Qt::Window << Qt::Desktop << Qt::WindowSystemMenuHint;

    // Flags
    QTest::ignoreMessage(QtDebugMsg, qPrintable(normalFlagMsg));
    MyNamespace::MyClass::MyFlags f1 = MyNamespace::MyClass::MyFlag1;
    qDebug().verbosity(verbosity) << "hello" << f1 << "world";

    MyNamespace::MyClass::MyFlags f2 = MyNamespace::MyClass::MyFlag2 | MyNamespace::MyClass::MyFlag3;
    QTest::ignoreMessage(QtDebugMsg, qPrintable(normalFlagsMsg));
    qDebug().verbosity(verbosity) << f1 << f2;

    QTest::ignoreMessage(QtDebugMsg, qPrintable(scopedFlagMsg));
    MyNamespace::MyClass::MyScopedFlags f3 = MyNamespace::MyClass::MyScopedFlag::MyFlag2;
    qDebug().verbosity(verbosity) << f3;

    QTest::ignoreMessage(QtDebugMsg, qPrintable(scopedFlagsMsg));
    f3 |= MyNamespace::MyClass::MyScopedFlag::MyFlag3;
    qDebug().verbosity(verbosity) << f3;

    // Single flag recognized as enum:
    QTest::ignoreMessage(QtDebugMsg, qPrintable(flagAsEnumMsg));
    MyNamespace::MyClass::MyFlag f4 = MyNamespace::MyClass::MyFlag1;
    qDebug().verbosity(verbosity) << f4;
#endif
}

void tst_QMetaObject::inherits_data()
{
    QTest::addColumn<const QMetaObject *>("derivedMetaObject");
    QTest::addColumn<const QMetaObject *>("baseMetaObject");
    QTest::addColumn<bool>("inheritsResult");

    QTest::newRow("MyClass inherits QObject")
        << &MyNamespace::MyClass::staticMetaObject << &QObject::staticMetaObject << true;
    QTest::newRow("QObject inherits MyClass")
        << &QObject::staticMetaObject << &MyNamespace::MyClass::staticMetaObject << false;
    QTest::newRow("MyClass inherits MyClass")
        << &MyNamespace::MyClass::staticMetaObject << &MyNamespace::MyClass::staticMetaObject << true;
    QTest::newRow("MyClassSubclass inherits QObject")
        << &MyNamespace::MyClassSubclass::staticMetaObject << &QObject::staticMetaObject << true;
    QTest::newRow("MyClassSubclass2 inherits QObject")
        << &MyNamespace::MyClassSubclass2::staticMetaObject << &QObject::staticMetaObject << true;
    QTest::newRow("MyClassSubclass2 inherits MyClass2")
        << &MyNamespace::MyClassSubclass2::staticMetaObject << &MyNamespace::MyClass2Subclass::staticMetaObject << false;
}

void tst_QMetaObject::inherits()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    QFETCH(const QMetaObject *, derivedMetaObject);
    QFETCH(const QMetaObject *, baseMetaObject);
    QFETCH(bool, inheritsResult);

    QCOMPARE(derivedMetaObject->inherits(baseMetaObject), inheritsResult);
#endif
}

void tst_QMetaObject::notifySignalsInParentClass()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    MyNamespace::ClassWithSetterGetterSignalsAddsProperties obj;
    QCOMPARE(obj.metaObject()->property(obj.metaObject()->indexOfProperty("value1")).notifySignal().name(), QByteArray("value1Changed"));
    QCOMPARE(obj.metaObject()->property(obj.metaObject()->indexOfProperty("value2")).notifySignal().name(), QByteArray("value2Changed"));

    MyNamespace::ClassWithChangedSignalNewValue obj2;
    QCOMPARE(obj2.metaObject()->property(obj2.metaObject()->indexOfProperty("value2")).notifySignal().name(), QByteArray("propertiesChanged"));
#if 0 // With verdigris, this is a compilation error when the NOTIFY signal is not a signal
    QTest::ignoreMessage(QtWarningMsg, "QMetaProperty::notifySignal: cannot find the NOTIFY signal thisIsNotASignal in class MyNamespace::ClassWithChangedSignalNewValue for property 'value3'");
    obj2.metaObject()->property(obj2.metaObject()->indexOfProperty("value3")).notifySignal();
#endif
#endif
}

QTEST_MAIN(tst_QMetaObject)
