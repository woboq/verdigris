/****************************************************************************
 *  Copyright (C) 2013-2015 Woboq GmbH
 *  Olivier Goffart <contact at woboq.com>
 *  https://woboq.com/
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program.
 *  If not, see <http://www.gnu.org/licenses/>.
 */
#include <wobjectdefs.h>
#include <QtCore/QObject>
#include <QtCore/QMetaObject>

#include <QtWidgets/qgraphicsitem.h>  // only for compilation check

#include "anothertu.h"

class tst_Basic : public QObject
{
    W_OBJECT(tst_Basic)

    struct SubObject;

private slots:
    void firstTest();
    W_SLOT(firstTest, W_Access::Private)

    void signalSlot_data();
    W_SLOT(signalSlot_data, W_Access::Private)
    void signalSlot();
    W_SLOT(signalSlot, W_Access::Private)

    void property_data();
    W_SLOT(property_data, W_Access::Private)
    void property();
    W_SLOT(property, W_Access::Private)

    void signalArgs();
    W_SLOT(signalArgs, W_Access::Private)

    void overloadForm();
    W_SLOT(overloadForm,(), W_Access::Private)

    void enumBase();
    W_SLOT(enumBase, W_Access::Private)

    void subObject();
    W_SLOT(subObject, W_Access::Private)

    void abstractParent();
    W_SLOT(abstractParent, W_Access::Private)

    void testQNamespace();
    W_SLOT(testQNamespace, W_Access::Private)

    void testAccess();
    W_SLOT(testAccess, W_Access::Private)

    void testAnotherTU();
    W_SLOT(testAnotherTU, W_Access::Private)

    void testFinal();
    W_SLOT(testFinal, W_Access::Private)

    void overloadedAddressOperator();
    W_SLOT(overloadedAddressOperator, W_Access::Private)
};

#include <wobjectimpl.h>

#include <QtTest/QtTest>

W_OBJECT_IMPL(tst_Basic)

void tst_Basic::firstTest()
{
    QCOMPARE(metaObject()->className(), "tst_Basic");
    QCOMPARE(metaObject()->superClass()->className(), "QObject");
}


class BTestObj  : public QObject
{   W_OBJECT(BTestObj)
    W_CONSTRUCTOR()
    QString value;
public:
    void setValue(const QString &s) { value = s; emit valueChanged(s); }
    W_SLOT(setValue)

    void resetValue() { value = QString(); }
    W_SLOT(resetValue)

    QString getValue() const { return value; }

    enum XXX { X1, X2, X3 = 45 };
    W_ENUM(XXX, X1, X2, X3)

    void setValueNoex(const QString &s) noexcept { value = s; emit valueChanged(s); }
    W_SLOT(setValueNoex)
    QString getValueNoex() const noexcept { return value; }

public: /*signals*/
    void valueChanged(const QString &s)
    W_SIGNAL(valueChanged, s)

    void simpleSignal()
    W_SIGNAL(simpleSignal)

    void signalSeveralArgs(int a, const QString &b, char * c = nullptr)
    W_SIGNAL(signalSeveralArgs, a, b, c)

    void anotherSignal()
    W_SIGNAL(anotherSignal)
public:
    W_PROPERTY(QString, value1, &BTestObj::setValue, &BTestObj::getValue )
    QString member;
    W_PROPERTY(QString, member1, &BTestObj::member)
    W_PROPERTY(QString, all, &BTestObj::value, &BTestObj::setValue, &BTestObj::getValue)

    W_PROPERTY(QString, notify1 MEMBER member NOTIFY simpleSignal)
    W_PROPERTY(QString, notify2 MEMBER member NOTIFY anotherSignal)

    W_PROPERTY(QString, valueNoex, &BTestObj::setValueNoex, &BTestObj::getValueNoex)
};


W_OBJECT_IMPL(BTestObj)

void tst_Basic::signalSlot_data()
{
    QTest::addColumn<bool>("old");
    QTest::newRow("old") << true;
    QTest::newRow("new") << false;
}


void tst_Basic::signalSlot()
{
    BTestObj obj1, obj2;
    QFETCH(bool, old);
    if (old) {
        QVERIFY(connect(&obj1, SIGNAL(valueChanged(QString)), &obj2, SLOT(setValue(QString))));
        QVERIFY(connect(&obj1, SIGNAL(simpleSignal()), &obj2, SLOT(resetValue())));
    } else {
        QVERIFY(connect(&obj1, &BTestObj::valueChanged, &obj2, &BTestObj::setValue));
        QVERIFY(connect(&obj1, &BTestObj::simpleSignal, &obj2, &BTestObj::resetValue));
    }
    obj1.setValue("HOLLA");
    QCOMPARE(obj2.getValue(), QString("HOLLA"));
    emit obj1.simpleSignal();
    QCOMPARE(obj2.getValue(), QString());
}


void tst_Basic::property_data()
{
    QTest::addColumn<QByteArray>("name");
    QTest::newRow("value1") << QByteArrayLiteral("value1");
    QTest::newRow("member1") << QByteArrayLiteral("member1");
    QTest::newRow("all") << QByteArrayLiteral("all");
    QTest::newRow("valueNoex") << QByteArrayLiteral("valueNoex");
}


void tst_Basic::property()
{
    QFETCH(QByteArray, name);

    BTestObj obj;
    QString str = "TRALAL";
    QVERIFY(obj.setProperty(name, str));
    QCOMPARE(obj.property(name), QVariant(str));
}

void tst_Basic::signalArgs()
{
    auto method = QMetaMethod::fromSignal(&BTestObj::signalSeveralArgs);
    QCOMPARE(method.parameterTypes(), (QByteArrayList{ "int", "QString", "char*" }));
    QCOMPARE(method.parameterNames(), (QByteArrayList{ "a", "b", "c" }));
}


struct My {};

class OverloadForm  : public QObject
{   W_OBJECT(OverloadForm)
public:
    int over1(My, int x) { return x; }
    W_SLOT(over1,(My,int))
    int over1() const { return 34; }
    W_INVOKABLE(over1,())
};


W_OBJECT_IMPL(OverloadForm)

void tst_Basic::overloadForm()
{
    OverloadForm obj;
    int result;
    QVERIFY(QMetaObject::invokeMethod(&obj, "over1", Q_RETURN_ARG(int, result), Q_ARG(My, {}), Q_ARG(int, 23)));
    QCOMPARE(result, 23);
    QVERIFY(QMetaObject::invokeMethod(&obj, "over1", Q_RETURN_ARG(int, result)));
    QCOMPARE(result, 34);
}

void tst_Basic::enumBase()
{
    QMetaEnum em = BTestObj::staticMetaObject.enumerator(
        BTestObj::staticMetaObject.indexOfEnumerator("XXX"));
    QVERIFY(em.isValid());
    QCOMPARE(em.keyCount(), 3);
}

struct tst_Basic::SubObject : QObject {
    W_OBJECT(SubObject)
public:
    void mySignal() W_SIGNAL(mySignal)
};

W_OBJECT_IMPL(tst_Basic::SubObject)

void tst_Basic::subObject()
{
    QVERIFY(QMetaMethod::fromSignal(&tst_Basic::SubObject::mySignal).isValid());
}

class AbstractClass : public QObject {
    W_OBJECT(AbstractClass)
    int prop;
    W_PROPERTY(int, prop MEMBER prop)

public:
    void mySignal() W_SIGNAL(mySignal)
    virtual void pureSlot() = 0; W_SLOT(pureSlot)
};
W_OBJECT_IMPL(AbstractClass)

class ConcreateClass : public AbstractClass {
    W_OBJECT(ConcreateClass)
    int prop2;
    W_PROPERTY(int, prop2 MEMBER prop2)

public:
    void mySignal2() W_SIGNAL(mySignal2)
    void pureSlot() override {}
};
W_OBJECT_IMPL(ConcreateClass)

void tst_Basic::abstractParent()
{
    QVERIFY(QMetaMethod::fromSignal(&AbstractClass::mySignal).isValid());
    QVERIFY(QMetaMethod::fromSignal(&ConcreateClass::mySignal2).isValid());
    ConcreateClass cl;
    AbstractClass *ab = &cl;
    QCOMPARE(ab->metaObject()->superClass(), &AbstractClass::staticMetaObject);
    QVERIFY(ab->setProperty("prop", 8989));
    QCOMPARE(ab->property("prop"), QVariant(8989));
}


#ifdef Q_NAMESPACE // since Qt 5.8
namespace TestQNamespace {
    W_NAMESPACE(TestQNamespace)
    enum TestEnum1 {
        Key1 = 11,
        Key2
    };
    W_ENUM_NS(TestEnum1, Key1, Key2)

    enum TestFlag1 {
        None = 0,
        Flag1 = 1,
        Flag2 = 2,
        Any = Flag1 | Flag2
    };
    W_FLAG_NS(TestFlag1, None, Flag1, Flag2, Any)

    W_CLASSINFO_NS("Foo", "Bar")

}
W_NAMESPACE_IMPL(TestQNamespace)

static void checkEnum(const QMetaEnum &enumerator, const QByteArray &name, const QVector<QPair<QByteArray, int >> &keys)
{
    QCOMPARE(name, QByteArray{enumerator.name()});
    QCOMPARE(keys.size(), enumerator.keyCount());
    for (int i = 0; i < enumerator.keyCount(); ++i) {
        QCOMPARE(keys[i].first, QByteArray{enumerator.key(i)});
        QCOMPARE(keys[i].second, enumerator.value(i));
    }
}
#endif

void tst_Basic::testQNamespace()
{
#ifdef Q_NAMESPACE
    QCOMPARE(TestQNamespace::staticMetaObject.enumeratorCount(), 2);
    checkEnum(TestQNamespace::staticMetaObject.enumerator(0), "TestEnum1",
                {{"Key1", 11}, {"Key2", 12}});
    checkEnum(TestQNamespace::staticMetaObject.enumerator(1), "TestFlag1",
                {{"None", 0}, {"Flag1", 1}, {"Flag2", 2}, {"Any", 1 | 2}});

    QMetaEnum meta = QMetaEnum::fromType<TestQNamespace::TestEnum1>();
    QVERIFY(meta.isValid());
    QCOMPARE(meta.name(), "TestEnum1");
    QCOMPARE(meta.enclosingMetaObject(), &TestQNamespace::staticMetaObject);
    QCOMPARE(meta.keyCount(), 2);

    QCOMPARE(TestQNamespace::staticMetaObject.classInfoCount(), 1);
    QMetaClassInfo info =  TestQNamespace::staticMetaObject.classInfo(0);
    QCOMPARE(info.name(), "Foo");
    QCOMPARE(info.value(), "Bar");
    QCOMPARE(info.enclosingMetaObject(), &TestQNamespace::staticMetaObject);

    QCOMPARE(TestQNamespace::staticMetaObject.className(), "TestQNamespace");

    QCOMPARE(AnotherTU::staticMetaObject.className(), "AnotherTU");
    QCOMPARE(AnotherTU::staticMetaObject.enumeratorCount(), 1);
    QCOMPARE(AnotherTU::staticMetaObject.enumerator(0).name(), "ATTestEnum");
    QCOMPARE(QMetaEnum::fromType<AnotherTU::ATTestEnum>().name(), "ATTestEnum");

#if defined (__cpp_inline_variables) && Q_CC_MSVC > 192000000
    QCOMPARE(Inline_NS::staticMetaObject.className(), "Inline_NS");
    QCOMPARE(Inline_NS::staticMetaObject.enumeratorCount(), 1);
    QCOMPARE(Inline_NS::staticMetaObject.enumerator(0).name(), "InlineEnum");
    QCOMPARE(QMetaEnum::fromType<Inline_NS::InlineEnum>().name(), "InlineEnum");
#endif
#endif
}


struct TestAccess : QObject  {
    W_OBJECT(TestAccess)

public:
    void publicSlot(){} W_SLOT(publicSlot)
    void publicMethod(){} W_INVOKABLE(publicMethod)
    void forcePrivateSlot(){} W_SLOT(forcePrivateSlot, W_Access::Private)
    void forcePrivateMethod(){} W_INVOKABLE(forcePrivateMethod, W_Access::Private)
protected:
    void protectedSlot(){} W_SLOT(protectedSlot)
    void protectedMethod(){} W_INVOKABLE(protectedMethod)
private:
    void privateSlot(){} W_SLOT(privateSlot)
    void privateMethod(){} W_INVOKABLE(privateMethod)
    void forcePublicSlot(){} W_SLOT(forcePublicSlot, W_Access::Public)
    void forcePublicMethod(){} W_INVOKABLE(forcePublicMethod, W_Access::Public)
};
W_OBJECT_IMPL(TestAccess)

struct TestAccess2 final {
    W_GADGET(TestAccess2)
private:
    void slot(int){} W_SLOT(slot, (int))
public:
    void slot(double){} W_SLOT(slot, (double))

};
W_GADGET_IMPL(TestAccess2)

void tst_Basic::testAccess()
{
    auto mo = &TestAccess::staticMetaObject;
    QCOMPARE(mo->method(mo->indexOfMethod("publicSlot()")).access(), QMetaMethod::Public);
    QCOMPARE(mo->method(mo->indexOfMethod("publicMethod()")).access(), QMetaMethod::Public);
    QCOMPARE(mo->method(mo->indexOfMethod("protectedSlot()")).access(), QMetaMethod::Protected);
    QCOMPARE(mo->method(mo->indexOfMethod("protectedMethod()")).access(), QMetaMethod::Protected);
    QCOMPARE(mo->method(mo->indexOfMethod("privateSlot()")).access(), QMetaMethod::Private);
    QCOMPARE(mo->method(mo->indexOfMethod("privateMethod()")).access(), QMetaMethod::Private);
    QCOMPARE(mo->method(mo->indexOfMethod("forcePublicSlot()")).access(), QMetaMethod::Public);
    QCOMPARE(mo->method(mo->indexOfMethod("forcePublicMethod()")).access(), QMetaMethod::Public);
    QCOMPARE(mo->method(mo->indexOfMethod("forcePrivateSlot()")).access(), QMetaMethod::Private);
    QCOMPARE(mo->method(mo->indexOfMethod("forcePrivateMethod()")).access(), QMetaMethod::Private);

    auto mo2 = &TestAccess2::staticMetaObject;
    QCOMPARE(mo2->method(mo2->indexOfMethod("slot(int)")).access(), QMetaMethod::Private);
    QCOMPARE(mo2->method(mo2->indexOfMethod("slot(double)")).access(), QMetaMethod::Public);

}

void tst_Basic::testAnotherTU()
{
    auto mo = &AnotherTU::Gaga::staticMetaObject;
    QCOMPARE(mo->className(), "AnotherTU::Gaga");

#if defined (__cpp_inline_variables) && Q_CC_MSVC > 192000000
    auto mo2 = &AnotherTU::InlineGadget::staticMetaObject;
    QCOMPARE(mo2->className(), "AnotherTU::InlineGadget");
    auto mo3 = &AnotherTU::InlineObject::staticMetaObject;
    QCOMPARE(mo3->className(), "AnotherTU::InlineObject");
#endif
}

struct TestFinalObject final : public QObject {
    W_OBJECT(TestFinalObject)
public:
    int foo() { return 0;}
    void setFoo(int) { }
    void fooChanged(int v) W_SIGNAL(fooChanged, v)
    W_PROPERTY(int, foo READ foo WRITE setFoo NOTIFY fooChanged)
};
W_OBJECT_IMPL(TestFinalObject)

struct TestFinalGadget final {
    W_GADGET(TestFinalGadget)
public:
    int foo() { return 0;}
    void setFoo(int) { }
    W_PROPERTY(int, foo READ foo WRITE setFoo)
};
W_GADGET_IMPL(TestFinalGadget)

void tst_Basic::testFinal()
{
    {
        auto mo = &TestFinalObject::staticMetaObject;
        QVERIFY(mo->property(mo->propertyOffset()).isFinal());
    }
    {
        auto mo = &TestFinalGadget::staticMetaObject;
        QVERIFY(mo->property(mo->propertyOffset()).isFinal());
    }
    {
        // Don't put final by default
        auto mo = &ConcreateClass::staticMetaObject;
        QVERIFY(!mo->property(mo->propertyOffset()).isFinal());
    }
}

class OverloadedAddressOperator : public QObject
{
   W_OBJECT(OverloadedAddressOperator)
public:
   void* operator&() { return nullptr; }
signals:
   void self(OverloadedAddressOperator &s) W_SIGNAL(self, s)
public slots:
    void assertSelf(OverloadedAddressOperator &o)
    {
        QCOMPARE(std::addressof(o), this);
        testResult = (std::addressof(o) == this);
    }
    W_SLOT(assertSelf)
public:
    bool testResult = false;
};

W_REGISTER_ARGTYPE(OverloadedAddressOperator&)

W_OBJECT_IMPL(OverloadedAddressOperator)

void tst_Basic::overloadedAddressOperator()
{
    OverloadedAddressOperator o;
    OverloadedAddressOperator *p = std::addressof(o);
    QCOMPARE(&o, static_cast<void *>(nullptr));
    QVERIFY(p);
    QObject::connect(p, &OverloadedAddressOperator::self, p, &OverloadedAddressOperator::assertSelf);
    emit o.self(o);
    QVERIFY(o.testResult);
}

QTEST_MAIN(tst_Basic)
