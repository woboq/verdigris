
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


/** ******************************************************************************************** **/
/** INTRODUCTION **/

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


/** ******************************************************************************************** **/
/** SLOTS **/
class SlotTutorial : public QObject {
    W_OBJECT(SlotTutorial)

    /**

       W_SLOT( <slot name> [, (<parametters types>) ]  [, <flags>]* )

       The W_SLOT macro needs to be put after the slot declaration.
       The W_SLOT macro can have flags:
       - Specifying the the access:  W_Access::Protected, W_Access::Private
         or W_Access::Public (the default)
       - W_Compat: for deprecated methods (equivalent of Q_MOC_COMPAT

       The W_SLOT macro can optionally have a list of parametter types as second
       argument to disambiguate or declarre types.
     */

    /* Examples: */
protected:
    // declares a protected slot
    void protectedSlot() {}
    W_SLOT(protectedSlot, W_Access::Protected)
private:
    // and a private slot
    void privateSlot() {}
    W_SLOT(protectedSlot, W_Access::Private)

public:
    // overloaded function needs a parametter list as second argument of the macro
    // to disambiguate
    void overload() {}
    W_SLOT(overload, ())

    void overload(int) {}
    W_SLOT(overload, (int))
private:
    void overload(double) {}
    W_SLOT(overload, (double), W_Access::Private)
    void overload(int, int) {}
    W_SLOT(overload, (int, int), W_Access::Private)
    // Note: for curstom type that are not const reference, one must use the normalized signature
};

W_OBJECT_IMPL(SlotTutorial)


/** ******************************************************************************************** **/
/** SIGNALS **/

class SignalTutorial : public QObject {
    W_OBJECT(SignalTutorial)

    /**

       W_SIGNAL_1( <signal signature> )
       W_SIGNAL_2( <signal name> [, (<parametters types>) ] , <parametters> )

       Almost Exactly like CS_SIGNAL, but
        - It is not possible to specify anything else than public signals
        - CS_SIGNAL_OVERLOAD is merged with W_SIGNAL_2
     */

public:
    // example:
    W_SIGNAL_1(void sig1(int a , int b))
    W_SIGNAL_2(sig1, a, b)

    // technicly the W_SIGNAL_1 is not nessesary. I only have it for symetry with copperspice but
    // I guess i should get rid of it.  But W_SIGNAL_2 should always come after the declaration
    // without semi colon at the end of the declaration:
    // So another way to declare a signal:
    void sig2(int a, int b) W_SIGNAL_2(sig2, a, b)

    // For overloaded signals:
    W_SIGNAL_1(void overload(int a, int b))
    W_SIGNAL_2(overload, (int, int), a, b)
};

W_OBJECT_IMPL(SignalTutorial)


/** ******************************************************************************************** **/
/** Gadgets,  invokables, constructor **/


class InvokableTutorial  {
    // Well, just like Qt has Q_GADGET, here we have W_GADGET
    W_GADGET(InvokableTutorial)

public:
    /** W_INVOKABLE is the same as W_SLOT.
     * It can take another flag (W_Scriptable) which corresponds to Q_SCRIPTABLE*/

    void myInvokable() {}
    W_INVOKABLE(myInvokable)


    /** W_CONSTRUCTOR(<parametter types>)
        for Q_INVOKABLE constructor, just pass the parametter types to this macro.
        one can have W_CONSTRUCTOR() for the default constructor even if it is implicit
     */


    InvokableTutorial(int, int) {}
    W_CONSTRUCTOR(int,  int)

    InvokableTutorial(void*, void* =nullptr) {}
    W_CONSTRUCTOR(void*, void*)
    // because of tehe default argument we can also do that:  (only in this macro)
    W_CONSTRUCTOR(void*)
};

// For gadget there is also a different IMPL macro
W_GADGET_IMPL(InvokableTutorial)

/** ******************************************************************************************** **/
/** PROPERTY **/

#include <QtCore/QMap>

class PropertyTutorial  : public QObject {
    W_OBJECT(PropertyTutorial)

public:
    /** W_PROPERTY(<type>, <name> [, <flags>]*)

        In addition, there are the macro READ WRITE MEMBER and so on which have been defined so
        you can just add a comma after the type, just like in a Q_PROPERTY.

        W_PROPERTY need to be put after all the seter member and co have been declared
    */

    QString m_value;
    QString value() const { return m_value; }
    void setValue(const QString &value) {
        m_value = value;
        emit valueChanged();
    }
    W_SIGNAL_1(void valueChanged())
    W_SIGNAL_2(valueChanged)

    // So just like in Qt only with one additional coma
    W_PROPERTY(QString, prop1 READ value WRITE setValue NOTIFY valueChanged)

    // Is equivalent to:
    W_PROPERTY(QString, prop2, &PropertyTutorial::value, &PropertyTutorial::setValue,
               W_Notify, &PropertyTutorial::valueChanged)
    // The setter and getter are matched by signature. add W_Notify before the notify signal

    // By member:
    W_PROPERTY(QString, prop3 MEMBER m_value NOTIFY valueChanged)
    //equivalent to
    W_PROPERTY(QString, prop4, &PropertyTutorial::m_value, W_Notify, &PropertyTutorial::valueChanged)

    // Optionally, you can put parentheses around the type, usefull if it contains a coma
    QMap<int, int> m_map;
    W_PROPERTY((QMap<int,int>), map  MEMBER m_map)
};

W_OBJECT_IMPL(PropertyTutorial)



/** ******************************************************************************************** **/
/** Enums **/


class EnumTutorial  {
    W_GADGET(EnumTutorial)

public:
    /** W_ENUM(<name>, <values>)
      Similar to Q_ENUM, but we also have to manually write all the values.
      Maybe in the future it could be made nicer that declares the enum in the same macro
      but now that's all we have */
    enum MyEnum { Blue, Red, Green, Yellow = 45, Violet = Blue + Green*3 };

    W_ENUM(MyEnum, Blue, Red, Green, Yellow)

    // CS_ENUM is a bit better, but i don't believe this works with complex expression like
    // "Blue + Green*3".

    // W_ENUM is currently limited to 16 enum values.
    // enum class are not yet supported

    // There is a W_FLAG  which is the same as Q_FLAG
};

W_GADGET_IMPL(EnumTutorial)

/** ******************************************************************************************** **/
/** TYPE REGISTRATION **/

/* Here is where the thing gets a bit more awkward:
   The types of signals and slots need to be registered so that we can generate the function signature

   To use a type as a return type or signal slot, it needs to:
    - be a builtin QMetaType; or
    - be registered with W_REGISTER_METATYPE; or
    - use the overload syntax, but not with const reference.
*/

struct CustomType1 {};
struct CustomType2 {};
struct CustomType3 {};

/** W_DECLARE_METATYPE(TYPE)
   register  TYPE so it can be used as a parameter of a signal/slot or return value
   One must use the normalized signature
   Note: This do not implies Q_DECLARE_METATYPE, and Q_DECLARE_METATYPE does not implies this
  */
W_DECLARE_METATYPE(CustomType1)
W_DECLARE_METATYPE(CustomType1*)
W_DECLARE_METATYPE(CustomType2)

class FooBar : public QObject {
    W_OBJECT(FooBar)
public:
    void slot1(CustomType1 a, CustomType2 b) {}
    W_SLOT(slot1) // OK, all arguments register with W_DECLARE_METATYPE

    void slot2(CustomType1 *a, CustomType2 *b) {}
    W_SLOT(slot2, (CustomType1*,CustomType2*)) // Need to use the overload syntax because
                                               // CustomType2* is not registered

    typedef int MyInt;
    typedef CustomType1 MyCustomType1;

    void slot3(FooBar::MyInt a, FooBar::MyCustomType1 b) {}
    W_SLOT(slot3, (FooBar::MyInt,FooBar::MyCustomType1)) // Need to use the overload syntax to use
                                                         // different type name (typedefs)

};

W_OBJECT_IMPL(FooBar)

/** ******************************************************************************************** **/
/** TEMPLATES **/

#include <QtCore/QDebug>

// we can have templated class:
template<typename T>
class MyTemplate : public QObject {
    W_OBJECT(MyTemplate)
public:
    // Template class can have slots and signals that depends on the parametter:
    void slot(T t) { qDebug() << "templated slot" << t; }
    W_SLOT(slot)

    W_SIGNAL_1(void signal(T t))
    W_SIGNAL_2(signal, t)
};

//The syntax of W_OBJECT_IMPL changes a bit: as a second parametter you need to specify the template
//prefix:
W_OBJECT_IMPL(MyTemplate<T>, template <typename T>)

// When you have several template arguments:
template<typename A, typename B> class MyTemplate2 : public QObject {
    W_OBJECT(MyTemplate2)
};
// The first argument of W_OBJECT_IMPL need to be within parentheses:
W_OBJECT_IMPL((MyTemplate2<A,B>), template<typename A, typename B>)


void templ() {
    // This shows that it is possible;
    bool ok = true;
    MyTemplate<QString> obj;
    // old syntax
    ok = ok && QObject::connect(&obj, SIGNAL(signal(QString)), &obj, SLOT(slot(QString)));
    // new syntax
    ok = ok && QObject::connect(&obj, &MyTemplate<QString>::signal, &obj, &MyTemplate<QString>::slot);
    Q_ASSERT(ok);
    emit obj.signal("Hallo"); // Will show the qDebug twice
}

/** ******************************************************************************************** **/
// This show nested class:
struct MyStruct {
    class Nested : public QObject {
        W_OBJECT(Nested)
    public:
        int foobar() {}
        W_INVOKABLE(foobar)
    };
};
W_OBJECT_IMPL(MyStruct::Nested)

/** ******************************************************************************************** **/


int main() {
    MyObject o;
    aaa(&o);
    templ();
}