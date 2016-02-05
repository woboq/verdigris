#include <wobjectdefs.h>
#include <QtCore/QObject>
class tst_Templates : public QObject
{
    W_OBJECT(tst_Templates)

private /*slots*/:

    // from https://codereview.qt-project.org/49864/
    void templatesMethod_data(); W_SLOT(templatesMethod_data, W_Access::Private)
    void templatesMethod(); W_SLOT(templatesMethod, W_Access::Private)


};


#include <QtTest/QtTest>


struct MyStruct {};
struct MyStruct2 {};

Q_DECLARE_METATYPE(MyStruct)
Q_DECLARE_METATYPE(MyStruct*)
W_DECLARE_METATYPE(MyStruct)
W_DECLARE_METATYPE(MyStruct*)

template<typename T, int I = 5 + 4, bool = (I > 5), class S = void>
class TestTemplate : public QObject {
    W_OBJECT(TestTemplate)
public:
    void mySlot(const T&) {} W_SLOT(mySlot,(const T&))
//signals:
    void mySignal(const T& t) W_SIGNAL_2(mySignal,(const T&), t)
};

template<typename T>
class TestTemplate2 : public TestTemplate<T>
{
    W_OBJECT(TestTemplate2)
public:
    void sl2() {} W_SLOT(sl2)
//signals:
    void si2() W_SIGNAL_2(si2)
};

template<typename O, typename T1, void (O::*)(const T1&), void (O::*F2)(const T1&)>
class FunctionTemplateParameter : public QObject {
    W_OBJECT(FunctionTemplateParameter)
    QString member;
    W_PROPERTY(QString, member MEMBER member)
public:
//signals:
    void hello() W_SIGNAL_2(hello)
};

template<template<typename> class Container1, template <typename TT> class , template <typename, int > class Container3>
class TemplateTemplateParameter : public QObject {
    W_OBJECT(TemplateTemplateParameter)
public:
//signals:
    void hello() W_SIGNAL_2(hello)
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
    void hello(Reducer*r) W_SIGNAL_2(hello,(Reducer*),r)
};

Q_DECLARE_METATYPE(const QMetaObject*);

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


QTEST_MAIN(tst_Templates)

W_DECLARE_METATYPE(ReduceKernel<Functor,Functor,int>*)

#include <wobjectimpl.h>
W_OBJECT_IMPL(tst_Templates)
W_OBJECT_IMPL((TestTemplate<T,I,B,S>), template<typename T, int I, bool B, class S>)
W_OBJECT_IMPL(TestTemplate2<T>, template<typename T>)
W_OBJECT_IMPL((FunctionTemplateParameter<O,T1,F1,F2>), template<typename O, typename T1, void (O::*F1)(const T1&), void (O::*F2)(const T1&)>)
W_OBJECT_IMPL((TemplateTemplateParameter<C1, C2, C3>), template<template<typename> class C1, template <typename> class C2, template <typename, int > class C3>)
W_OBJECT_IMPL((MappedReducedKernel<ReducedResultType, Iterator, MapFunctor, ReduceFunctor, Reducer>), template <typename ReducedResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor, typename Reducer>)
