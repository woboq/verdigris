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

#include <QtWidgets/qgraphicsitem.h>  // only for compilation check

class tst_Basic : public QObject
{
    W_OBJECT(tst_Basic)

    struct SubObject;

private /*slots*/:
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
    void mySignal() W_SIGNAL(mySignal);
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
    void mySignal() W_SIGNAL(mySignal);
    virtual void pureSlot() = 0; W_SLOT(pureSlot)
};
W_OBJECT_IMPL(AbstractClass)

class ConcreateClass : public AbstractClass {
    W_OBJECT(ConcreateClass)
    int prop2;
    W_PROPERTY(int, prop2 MEMBER prop2)

public:
    void mySignal2() W_SIGNAL(mySignal2);
    void pureSlot() override {};
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

QTEST_MAIN(tst_Basic)
