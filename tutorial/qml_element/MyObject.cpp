#include "MyObject.h"

/* Here is what would go in the C++ .cpp file: */
#include <wobjectimpl.h>

// And now this is the macro you need to instantiate the meta object.
// It's an additional macro that basically does the same as the code generated by moc.
// W_OBJECT_IMPL(MyObject)

MyObject::MyObject() {}

int MyObject::demoProp() const
{
  return m_demoProp;
}

void MyObject::setDemoProp(int newDemoProp)
{
  if (newDemoProp == m_demoProp) return;
  m_demoProp = newDemoProp;
  emit demoPropChanged();
}

W_QML_ELEMENT_IMPL(MyObject)