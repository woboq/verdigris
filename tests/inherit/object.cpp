#include "object.h"

#include "wobjectimpl.h"

W_OBJECT_IMPL(Object)

Object::Object(QObject *parent)
    : QObject(parent)
{}

void Object::emit1()
{
    emit handleEvent1();
}

void Object::emit2()
{
    emit handleEvent2();
}

void Object::emit3()
{
    emit handleEvent3();
}
