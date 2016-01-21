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
        //void fooChanged();
        //void barChanged(int);

public:

  //  Q_PROPERTY2(int, foo, &MyObject::getFoo, &MyObject::setFoo)

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
    QObject::connect(&obj,SIGNAL(barChanged(int)), &obj, SIGNAL(setFoo(int)));  // (setFoo is using SIGNAL because all method are signal in this prototype)
//    QObject::connect(&obj, &MyObject::barChanged, [](auto q){ qDebug() << Q_FUNC_INFO << q;  });
//    obj.barChanged(222);

    obj.setProperty("foo", 333);
}

}
