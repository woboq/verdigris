#include "nickelspace.h"
#include <QtCore/QObject>
#include <QtCore/QDebug>


class TestObject : public QObject {
public:
    W_OBJECT
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
    qDebug() << obj.metaObject()->className() << TestObject::ParentMetaObjectGetter::value->className();
    //qDebug() << TestObject::parentMetaObject::value->className();

}
