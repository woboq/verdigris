This (header-only) library can be used to create an application using Qt, without the need of the
moc (MetaObject Compiler). It uses a different set of macro than Qt and templated constexpr code to
generate the QMetaObject at compile-time. It is entirely binary compatible with Qt.

Blog post presenting the project: https://woboq.com/blog/verdigris-qt-without-moc.html (2016)

Blog post with some implementation details: https://woboq.com/blog/verdigris-implementation-tricks.html (2018)

Browse code online: https://code.woboq.org/woboq/verdigris

Travis: [![Travis Build Status](https://travis-ci.org/woboq/verdigris.svg?branch=master)](https://travis-ci.org/woboq/verdigris)
Appveyor: 
[![Appveyor Build Status](https://ci.appveyor.com/api/projects/status/povubj5thvlsu6sy/branch/master?svg=true)](https://ci.appveyor.com/project/ogoffart/verdigris)


## How to Use

The library consist of only two headers files. You can either copy these header files in your
project, or adjust the include paths so that the compiler finds them.
You will find the headers in the 'src/' sub-directory.
Also make sure to set your compiler in, at least, C++14 mode. With qmake, you can do that with
`CONFIG += c++14`.

For the documentation, see the tutorial.
https://code.woboq.org/woboq/verdigris/tutorial/tutorial.cpp.html

For MSVC, you also need to define some C++14 defines that are not builtins so Qt enable C++14 features.
That can be done in qmake like so: `msvc:DEFINES+=__cpp_constexpr=201304 __cpp_variable_templates=201304`

Tested with Qt >= 5.5.
Need a compiler that can do C++14 relaxed constexpr such as GCC 5.1 or Clang 3.5, or MSVC 2017


## Status

Almost all features of Qt are working. The Qt test have been ported.

Features that are not yet working:
 - Q_PLUGIN_METADATA: This would require compiling to the Qt's binary json. Out of scope for now.
 - QMetaMethod::tag(): Not yet implemented, could be supported if needed, but is not really needed
                       for anything. (not even tested by Qt's auto test)
 - Q_ENUM: Working, but requires to repeat the name of every enum value. Could be improved.

New features compared to Qt with moc:
 - Support for templated QObject.
 - Support for QObject nested in another class.


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

If you have any questions or remark please email  contact@woboq.com

