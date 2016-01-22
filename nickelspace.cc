#include "nickelspace.h"
#include <QtCore/QObject>
#include <QtCore/QDebug>
#include <QtCore/qmetaobject.h>


class TestObject : public QObject {
public:
    W_OBJECT(TestObject)
public:

    ~TestObject();


    int fooBar(int x) { return x; }
    W_SLOT_2(fooBar)


    int fooBar2(int x) { return x; }
    W_SLOT_2(fooBar2, W_Access::Private)

    int fooBar3(QString x) { return x.toInt(); }
    W_SLOT_2(fooBar3, W_Access::Private)


};

TestObject::~TestObject()
{
}

W_OBJECT_IMPL(TestObject)



//    CS_OBJECT(KeyPressEater)

class MyObject : public QObject {

    W_OBJECT(MyObject)

public slots:
    void setFoo(int value) { qDebug() << Q_FUNC_INFO << value; };
    W_SLOT_2(setFoo)
    int getFoo() {  return 0; };
    W_SLOT_2(getFoo)
    void setBar() {  }
    W_SLOT_2(setBar)
    QObject *getBar() {  return this; }
    W_SLOT_2(getBar)
    signals:  // would expands to public [[qt::signals]]:

        W_SIGNAL_1(void fooChanged())
        W_SIGNAL_2(fooChanged)


        W_SIGNAL_1(void barChanged(int bar))
        W_SIGNAL_2(barChanged,bar)

public:

    W_PROPERTY(int, foo, &MyObject::getFoo, &MyObject::setFoo)

    QString m_strin;
    W_PROPERTY(QString, prop2, &MyObject::m_strin)

};


W_OBJECT_IMPL(MyObject)


int main() {
    TestObject obj;
    qDebug() << obj.metaObject()->className() << obj.metaObject()->superClass()->className();
    //qDebug() << TestObject::parentMetaObject::value->className();

    for (int i = 0; i <  obj.metaObject()->methodCount(); ++i) {
        qDebug() << i << obj.metaObject()->method(i).methodSignature();
    }


{
    MyObject obj;
    qDebug() << obj.metaObject()->className();
    //    qDebug() << obj.metaObject()->method(5).parameterCount();
    QMetaObject::invokeMethod(&obj, "setFoo", Q_ARG(int, 111));

    //    qDebug() << obj.metaObject()->indexOfMethod("barChanged(int)") - obj.metaObject()->methodOffset();
    QObject::connect(&obj,SIGNAL(barChanged(int)), &obj, SLOT(setFoo(int)));
    QObject::connect(&obj, &MyObject::barChanged, [](auto q){ qDebug() << Q_FUNC_INFO << q;  });
    obj.barChanged(222);

    obj.setProperty("foo", 333);

    obj.setProperty("prop2", "YAY");
    qDebug() << obj.property("prop2");


}


#define MACRO_CAT(CAT, ...) CAT
#define MACRO_TAIL(CAT, ...) __VA_ARGS__

#define X(a) +a+

#define MACRO_FOREACH(FUNC, VAL) MACRO_EVAL(FUNC(MACRO_CAT VAL) MACRO_FOREACH(FUN, (MACRO_TAIL VAL)))
#define MACRO_FOREACH_(F, V) MACRO_FOREACH
#define MACRO_EVAL(...) MACRO_EVAL1(MACRO_EVAL1(MACRO_EVAL1(__VA_ARGS__)))
#define MACRO_EVAL1(...) __VA_ARGS__


    qDebug() << QT_STRINGIFY(MACRO_FOREACH(X, (A, B, C, D)));




}






