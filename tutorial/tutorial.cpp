
/*
 This file present this "fork" of coperspice.

 What is copperspice:
  Copperspice is a fork of Qt 4 which has been done mainly to get rid of moc. In order to get rid
  of moc, they changed the Qt macro to be less optimal dropped many compilers to require C++11
  (even more than what is still supported in Qt 5.7). And they made a complete, incompatible fork
  of Qt.

 This library:
  It is not a fork of copperspice, but rather a re-implementation of their macro in a way that is
  binary compatible with Qt.
  That is: you can write your application without needing moc, and still use it with Qt.

 How:
  Copperspice generate the metaobjects at runtime. But moc generates it at compile time. I choose
  to do the same using constexpr to generate a QMetaObject at compile time.
  I am using C++14 for simplicity because it is much more easy to handle constexpr in C++14 altough
  I think everything should be possible in C++11.  The code is originaly taken from my previous work
  https://woboq.com/blog/reflection-in-cpp-and-qt-moc.html
  But in there i was trying to do so with the same kind of annotation that Qt does (keeping source
  compatibility). When using uglier macro (copperspice style) we can do it without the moc.


 Name of this library: ###
  currently, all the macro and identifier starts with W_ or w_.
  the 'W' stands for Woboq.
  I did not find a real name and once one is found, things should be moved in a namespace and macro
  renamed.

 Code:
 */


/**************************************************************************************************/

// In a header file you would include the wobjectdefs.h header
#include <wobjectdefs.h>
//### The name 'wobjectdefs' is taken from the name qobjectdefs. But perhaps it should have a better name?

// And because you will inherit from QObject you also need to QObject header
#include <QObject>

//### Should QObject be included by wobjectdefs?  If you inherit from anything else you need another
//    header. And also you can have Q_GADGET.


// Now declare your class:
class MyObject : public QObject
{
    /* The W_OBJECT macro is equivalent to the Q_OBJECT macro. The difference is that it must
       contains the class name as a parametter and need to be put before any other W_ macro in the
       class. So it's the same as the CS_OBJECT macro from copperspice */
    W_OBJECT(MyObject)

public /* slots */:
    
    //Here we declare a slot:
    void mySlot(const QString &name) { qDebug("hello %s", qPrintable(name));  }
    /* If you're going to use the new connection syntax, no need to do anything else for slots.
       But if you want to use the other connection syntax, or QML, you need to register the slot
       just like so: */
    W_SLOT(mySlot)
    /* The W_SLOT has optional argument we will see them later. But notice how much simpler it
       is than the two CS_SLOT_ macro.  Aslo, copperspice slot cannot be ceclared inline in the
       class definition. */

public /* signals */:

    // Now a signal
    W_SIGNAL_1(void mySignal(const QString &name))
    W_SIGNAL_2(mySignal, name)

    /* Slightly more complicated than a slot. This is actually just like copperspice */

};


/* Here is what would go in the C++ file */

// This header could also go in a header. so there would have been only one header to include
// But it is better to keep them separate
#include <wobjectimpl.h>

// And now this is the macro you need to instentiate the meta object
// It's an additional macro compared to Qt and moc but it is required
W_OBJECT_IMPL(MyObject)

// That's it. our MyObject is a QObject that can be used in QML or connected
void aaa(MyObject *obj1) {
    bool ok = true;
    // new syntax
    ok = ok && QObject::connect(obj1, &MyObject::mySignal, obj1, &MyObject::mySlot);
    // old syntax
    ok = ok && QObject::connect(obj1, SIGNAL(mySignal(QString)), obj1, SLOT(mySlot(QString)));
    Q_ASSERT(ok);
}



int main() {
    MyObject o;
    aaa(&o);
}