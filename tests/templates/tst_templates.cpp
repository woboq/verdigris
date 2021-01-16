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
class tst_Templates : public QObject
{
    W_OBJECT(tst_Templates)

private slots:

    // from https://codereview.qt-project.org/49864/
    void templatesMethod_data(); W_SLOT(templatesMethod_data, W_Access::Private)
    void templatesMethod(); W_SLOT(templatesMethod, W_Access::Private)

    // from https://codereview.qt-project.org/49866/
    void connectTemplate();  W_SLOT(connectTemplate, W_Access::Private)

    void gadget(); W_SLOT(gadget, W_Access::Private)

    void smartPointer(); W_SLOT(smartPointer, W_Access::Private)
};


#include <QtTest/QtTest>


struct MyStruct {};
struct MyStruct2 {};

Q_DECLARE_METATYPE(MyStruct)
Q_DECLARE_METATYPE(MyStruct*)
W_REGISTER_ARGTYPE(MyStruct)
W_REGISTER_ARGTYPE(MyStruct*)

template<typename T, int I = 5 + 4, bool = (I > 5), class S = void>
class TestTemplate : public QObject {
    W_OBJECT(TestTemplate)
public:
    void mySlot(const T&) {} W_SLOT(mySlot,(const T&))
//signals:
    void mySignal(const T& t) W_SIGNAL(mySignal,(const T&), t)
};

template<typename T>
class TestTemplate2 : public TestTemplate<T>
{
    W_OBJECT(TestTemplate2)
public:
    void sl2() {} W_SLOT(sl2)
//signals:
    void si2() W_SIGNAL(si2)
};

template<typename O, typename T1, void (O::*)(const T1&), void (O::*F2)(const T1&)>
class FunctionTemplateParameter : public QObject {
    W_OBJECT(FunctionTemplateParameter)
    QString member;
    W_PROPERTY(QString, member MEMBER member)
public:
//signals:
    void hello() W_SIGNAL(hello)
};

template<template<typename> class Container1,
         template <typename TT> class,
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
         template <typename, qsizetype > class Container3>
#else
         template <typename, int > class Container3>
#endif
class TemplateTemplateParameter : public QObject {
    W_OBJECT(TemplateTemplateParameter)
public:
//signals:
    void hello() W_SIGNAL(hello)
};

template<typename, typename, typename> struct ReduceKernel{};
struct Functor { typedef int result_type; };

template <typename ReducedResultType,
          typename Iterator,
          typename MapFunctor,
          typename ReduceFunctor,
          typename Reducer = ReduceKernel<ReduceFunctor,
                                          ReducedResultType,
                                          typename MapFunctor::result_type> >
class MappedReducedKernel : public QObject
{
    W_OBJECT(MappedReducedKernel)
public: // signals:
    void hello(Reducer*r) W_SIGNAL(hello,(Reducer*),r)
};

Q_DECLARE_METATYPE(const QMetaObject*)

void tst_Templates::templatesMethod_data()
{
    QTest::addColumn<const QMetaObject *>("metaObject");
    QTest::addColumn<QByteArray>("method");
    QTest::addColumn<bool>("exist");
    typedef QByteArray _;

    QTest::newRow("TestTemplate<QString> mySlot(T)")
        << &TestTemplate<QString>::staticMetaObject << _("mySlot(T)") << false;
    QTest::newRow("TestTemplate<QString> mySlot(QString)")
        << &TestTemplate<QString>::staticMetaObject << _("mySlot(QString)") << true;
    QTest::newRow("TestTemplate<QString> mySlot(MyStruct)")
        << &TestTemplate<QString>::staticMetaObject << _("mySlot(MyStruct)") << false;
    QTest::newRow("TestTemplate<QString> mySignal(T)")
        << &TestTemplate<QString>::staticMetaObject << _("mySignal(T)") << false;
    QTest::newRow("TestTemplate<QString> mySignal(QString)")
        << &TestTemplate<QString>::staticMetaObject << _("mySignal(QString)") << true;
    QTest::newRow("TestTemplate<QString> mySignal(MyStruct)")
        << &TestTemplate<QString>::staticMetaObject << _("mySignal(MyStruct)") << false;

    QTest::newRow("TestTemplate<MyStruct> mySlot(T)")
        << &TestTemplate<MyStruct>::staticMetaObject << _("mySlot(T)") << false;
    QTest::newRow("TestTemplate<MyStruct> mySlot(QString)")
        << &TestTemplate<MyStruct>::staticMetaObject << _("mySlot(QString)") << false;
    QTest::newRow("TestTemplate<MyStruct> mySlot(MyStruct)")
        << &TestTemplate<MyStruct>::staticMetaObject << _("mySlot(MyStruct)") << true;
    QTest::newRow("TestTemplate<MyStruct> mySignal(T)")
        << &TestTemplate<MyStruct>::staticMetaObject << _("mySignal(T)") << false;
    QTest::newRow("TestTemplate<MyStruct> mySignal(QString)")
        << &TestTemplate<MyStruct>::staticMetaObject << _("mySignal(QString)") << false;
    QTest::newRow("TestTemplate<MyStruct> mySignal(MyStruct)")
        << &TestTemplate<MyStruct>::staticMetaObject << _("mySignal(MyStruct)") << true;

    QTest::newRow("TestTemplate2<QString> mySlot(T)")
        << &TestTemplate2<QString>::staticMetaObject << _("mySlot(T)") << false;
        QTest::newRow("TestTemplate2<QString> mySlot(QString)")
        << &TestTemplate2<QString>::staticMetaObject << _("mySlot(QString)") << true;

    QTest::newRow("FunctionTemplateParameter")
        << &FunctionTemplateParameter<TestTemplate<QString>, QString, &TestTemplate<QString>::mySlot, &TestTemplate<QString>::mySignal>::staticMetaObject
        << _("hello()") << true;

    QTest::newRow("TemplateTemplateParameter")
        << &TemplateTemplateParameter<QList, QVector, QVarLengthArray>::staticMetaObject
        << _("hello()") << true;

    QTest::newRow("MappedReducedKernel")
        << &MappedReducedKernel<Functor, Functor, Functor, Functor>::staticMetaObject
        << _("hello(Reducer*)") << true;

}


void tst_Templates::templatesMethod()
{
    QFETCH(const QMetaObject *, metaObject);
    QFETCH(QByteArray, method);
    QFETCH(bool, exist);

    int index = metaObject->indexOfMethod(method);
    QCOMPARE(index != -1, exist);
}

template <typename T> class TemplateObject : public QObject  {
    W_OBJECT(TemplateObject)
public: // signals
    void signalTemplate(const T &t) W_SIGNAL(signalTemplate, t)
    void signalString(const QString & str) W_SIGNAL(signalString, str)
public:
    void slotTemplate(const T &t) { result = QVariant::fromValue<T>(t); count++; }
    W_SLOT(slotTemplate)
    void slotVariant(const QVariant &t) { result = t; count += 100; }
    W_SLOT(slotVariant)

public:
    TemplateObject() : count(0) { }
    int count;
    QVariant result;
};

void tst_Templates::connectTemplate()
{
    TemplateObject<int> oi;
    TemplateObject<QString> os;

    QVERIFY(QObject::connect(&oi, &TemplateObject<int>::signalTemplate, &os, &TemplateObject<QString>::slotVariant));
    oi.signalTemplate(25);
    QCOMPARE(os.count, 100);
    QCOMPARE(os.result, QVariant(25));
    os.count = 0;

    QVERIFY(QObject::connect(&oi, &TemplateObject<int>::signalString, &os, &TemplateObject<QString>::slotTemplate));
    oi.signalString("hello");
    QCOMPARE(os.count, 1);
    QCOMPARE(os.result, QVariant("hello"));
    os.count = 0;

    QVERIFY(QObject::connect(&os, &TemplateObject<QString>::signalTemplate, &oi, &TemplateObject<int>::slotVariant));
    os.signalTemplate("world");
    QCOMPARE(oi.count, 100);
    QCOMPARE(oi.result, QVariant("world"));
}


template<typename T> class TemplateGadget {
    W_GADGET(TemplateGadget)
public:
    T data;

    T readData() { return data; }
    W_INVOKABLE(readData)
    void writeData(const T &d) { data = d; }
    W_INVOKABLE(writeData)
    W_PROPERTY(T, data READ readData WRITE writeData)

    QVariant readVariant() { return QVariant::fromValue(data); }
    W_INVOKABLE(readVariant)
    void writeVariant(const QVariant &d) { data = qvariant_cast<T>(d); }
    W_INVOKABLE(writeVariant)
    W_PROPERTY(QVariant, variant READ readVariant WRITE writeVariant)
};

template<typename T> void testGadget(const T &val1)
{
    TemplateGadget<T> xx;
    const QMetaObject *mo = &xx.staticMetaObject;
    {
        auto id = mo->indexOfProperty("data");
        QVERIFY(id >= 0);
        auto prop = mo->property(id);
        prop.writeOnGadget(&xx, QVariant::fromValue(val1));
        QCOMPARE(xx.data, val1);
        QVariant v = prop.readOnGadget(&xx);
        QCOMPARE(v.value<T>(), val1);
    }
    {
        auto id = mo->indexOfProperty("variant");
        QVERIFY(id >= 0);
        auto prop = mo->property(id);
        prop.writeOnGadget(&xx, QVariant::fromValue(val1));
        QCOMPARE(xx.data, val1);
        id = mo->indexOfMethod("readVariant()");
        QVERIFY(id >= 0);
        auto meth = mo->method(id);
        QVariant v;
        meth.invokeOnGadget(&xx, Q_RETURN_ARG(QVariant, v));
        QCOMPARE(v.value<T>(), val1);
    }

}

void tst_Templates::gadget()
{
    testGadget(34);
    testGadget(QString("hello"));
    testGadget(QByteArray("toto"));
}

#include <wobjectcpp.h>

namespace test {

template<typename T>
class RegisterTemplate : public QObject {
    W_OBJECT(RegisterTemplate)
};
using IntTest = RegisterTemplate<int>;

constexpr auto w_explicitObjectName(IntTest*) {
    return w_cpp::viewLiteral("IntTest");
}

} // namespace test

void tst_Templates::smartPointer() {
    auto smart_ptr = QSharedPointer<test::IntTest>{};
    // note: this will fail if no w_explicitObjectName was registered
    auto var = QVariant::fromValue(smart_ptr);
}


QTEST_MAIN(tst_Templates)

W_REGISTER_ARGTYPE(ReduceKernel<Functor,Functor,int>*)

#include <wobjectimpl.h>
W_OBJECT_IMPL(tst_Templates)
W_OBJECT_IMPL((TestTemplate<T,I,B,S>), template<typename T, int I, bool B, class S>)
W_OBJECT_IMPL(TestTemplate2<T>, template<typename T>)
W_OBJECT_IMPL((FunctionTemplateParameter<O,T1,F1,F2>), template<typename O, typename T1, void (O::*F1)(const T1&), void (O::*F2)(const T1&)>)
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
W_OBJECT_IMPL((TemplateTemplateParameter<C1, C2, C3>), template<template<typename> class C1, template <typename> class C2, template <typename, qsizetype > class C3>)
#else
W_OBJECT_IMPL((TemplateTemplateParameter<C1, C2, C3>), template<template<typename> class C1, template <typename> class C2, template <typename, int > class C3>)
#endif
W_OBJECT_IMPL((MappedReducedKernel<ReducedResultType, Iterator, MapFunctor, ReduceFunctor, Reducer>), template <typename ReducedResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor, typename Reducer>)
W_OBJECT_IMPL(TemplateObject<T>, template<typename T>)
W_GADGET_IMPL(TemplateGadget<T>, template<typename T>)
W_OBJECT_IMPL((test::RegisterTemplate<T>), template<typename T>)
