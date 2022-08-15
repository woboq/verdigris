## About

This (header-only) library can be used to create an application using Qt, without the need of the
moc (MetaObject Compiler). It uses a different set of macro than Qt and templated constexpr code to
generate the QMetaObject at compile-time. It is entirely binary compatible with Qt.

Blog post presenting the project: https://woboq.com/blog/verdigris-qt-without-moc.html (2016)

Blog post with some implementation details: https://woboq.com/blog/verdigris-implementation-tricks.html (2018)

Browse code online: https://code.woboq.org/woboq/verdigris

Github Actions: [![Clang Tests](https://github.com/woboq/verdigris/actions/workflows/clang.yml/badge.svg?branch=master)](https://github.com/woboq/verdigris/actions/workflows/clang.yml) [![Gcc Tests](https://github.com/woboq/verdigris/actions/workflows/gcc.yml/badge.svg?branch=master)](https://github.com/woboq/verdigris/actions/workflows/gcc.yml) [![Windows Tests](https://github.com/woboq/verdigris/actions/workflows/windows.yml/badge.svg?branch=master)](https://github.com/woboq/verdigris/actions/workflows/windows.yml)
Appveyor:
[![Appveyor Build Status](https://ci.appveyor.com/api/projects/status/povubj5thvlsu6sy/branch/master?svg=true)](https://ci.appveyor.com/project/ogoffart/verdigris)

## Documentation

For an introduction, see the [tutorial.cpp](https://code.woboq.org/woboq/verdigris/tutorial/tutorial.cpp.html).
See also a more detailed documetation of the macros in the source code.

## Status

Almost all features of Qt are working. The Qt test have been ported.

Features that are not yet working:
 - Q_PLUGIN_METADATA: This would require compiling to the Qt's binary json. Out of scope for now.
 - QML_ELEMENT: This is a Qt6 feature that automatically registers the QObjects for QML. Out of scope for now.
 - BINDABLE: Needs to be backported to the current C++ and Qt requirements.
 - QMetaMethod::tag(): Not yet implemented, could be supported if needed, but is not really needed
                       for anything. (not even tested by Qt's auto test)
 - Q_ENUM: Working, but requires to repeat the name of every enum value. Could be improved.

**New features compared to Qt with moc:**
 - Support for templated QObject.
 - Support for QObject nested in another class.

## How to Use

The library consist of only two headers files. You can either copy these header files in your
project, or adjust the include paths so that the compiler finds them.
You will find the headers in the 'src/' sub-directory.
Also make sure to set your compiler in, at least, C++14 mode. With qmake, you can do that with
`CONFIG += c++14`.

Tested with Qt >= 5.9.
Need a compiler that can do C++14 relaxed constexpr such as GCC 5.1 or Clang 3.5, or MSVC 2017

### Translations

When running `lupdate`, add the argument `-tr-function-alias Q_DECLARE_TR_FUNCTIONS+=W_OBJECT` to
avoid the warning that your class are not using the Q_OBJECT macro.

### Correspondance Table

This table show the correspondence between Qt macro and Verdigris macro:

| Qt macro                                     | Use the Verdigris macro instead                   |
|----------------------------------------------|---------------------------------------------------|
| `Q_OBJECT`                                   | `W_OBJECT(MyClass)` ...  `W_OBJECT_IMPL(MyClass)` |
| `public slots: void mySlot(int x);`          |  `void mySlot(int x); W_SLOT(mySlot)`             |
| `signals: void mySignal(int x);`             |  `void mySignal(int x) W_SIGNAL(mySignal,x)`	     |
| `Q_PROPERTY(int myProperty WRITE setProp READ getProp NOTIFY propChanged)` | `W_PROPERTY(int, myProperty WRITE setProp READ getProp NOTIFY propChanged)` |
| `Q_GADGET`                                   | `W_GADGET(MyClass)` ...  `W_GADGET_IMPL(MyClass)` |
| `Q_INVOKABLE void myFunction(int foo);`      |  `void myFunction(int foo); W_INVOKABLE(myFunction)`
| `Q_INVOKABLE MyClass(int foo);`              |  `MyClass(int foo); W_CONSTRUCTOR(int)`           |
| `Q_CLASSINFO("foo", "bar")`                  | `W_CLASSINFO("foo", "bar")`                       |
| `Q_DECLARE_INTERFACE(MyInterface, "my.interface")` ... `Q_INTERFACE(MyInterface)`     | `Q_DECLARE_INTERFACE(MyInterface, "my.interface")` ... `W_INTERFACE(MyInterface)` |
| `Q_NAMESPACE`                                | `W_NAMESPACE(MyNs)` ...  `W_NAMESPACE_IMPL(MyNs)` |
| Q_ENUM/Q_FLAG/Q_ENUM_NS/Q_FLAG_NS            | W_ENUM/W_FLAG/W_ENUM_NS/W_FLAG_NS                 |

## Who uses Verdigris ?

* [Ossia Score](https://ossia.io/), an interactive intermedia sequencer. ([github](https://github.com/OSSIA/score))
  It uses Verdigris in productions. Verdigris allowed ossia score to use template with their QObject's, and solve some trouble with the build system.


<img align="right" src="https://woboq.com/logos/isotronic.png">

* [ISOTRONIC GmbH](https://isotronic.de/), a provider for visual inspection QA systems for automatic glass vial manufacturing, uses Verdigris in production:
> The transition from standard Qt affected more than 100 source code files and was done in not much more than a day. The reason for the switch was to be able to use a build system that has no interface for Qt's MOC process. In the rare cases of questions or problems the Verdigris team was quick and competent in resolving the issues. After more than 6 months of real-world experience we are still very happy with that decision.

* If you are using Verdigris and want to appear here, please open an issue, or a pull request

## Context

The macros were inspired by [CopperSpice](http://www.copperspice.com/).
The template code was based on previous work:
https://woboq.com/blog/reflection-in-cpp-and-qt-moc.html

Differences with CopperSpice:

1. Contrary to CopperSpice, this is not a fork of Qt, but just an alternative set of macro to define
   Qt objects and signals and slot in a way that is binary compatible with Qt, but does not require
   moc. This is to be used with the normal Qt. CopperSpice being an entire fork of Qt, it has more
   differences with Qt and is not kept up to date with all the new features coming in Qt.
2. The QMetaObject is built at compile time in the read only section (like what moc does).
   CopperSpice builds it at run-time when the library is loaded. So CopperSpice takes more memory
   and load slower.
3. With CopperSpice, you cannot declare your slot within the class definition.
4. CopperSpice uses `__LINE__` in its macro making it impossible to declare several things in one line
   or to declare objects or properties from macros.
5. The usability of some macro was (in our opinion) improved.
6. The additional Q_OBJECT_IMPL


## Licence

Like Qt, this library is under the dual licence LGPLv3 and GPLv2.
Being header-only, this removes many of the obligations of the LGPLv3.

If you have any questions or remark please email  info@woboq.com
