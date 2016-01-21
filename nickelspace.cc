#include "nickelspace.h"
#include <QtCore/QObject>
#include <QtCore/QDebug>


class TestObject : public QObject {
public:
    W_OBJECT(TestObject)
public:

    ~TestObject();



};

TestObject::~TestObject()
{
}

W_OBJECT_IMPL(TestObject)



//    CS_OBJECT(KeyPressEater)


int main() {
    TestObject obj;
    qDebug() << obj.metaObject()->className() << obj.metaObject()->superClass()->className();
    //qDebug() << TestObject::parentMetaObject::value->className();

}
