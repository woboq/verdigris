/****************************************************************************
 *  Copyright (C) 2016-2018 Woboq GmbH
 *  Olivier Goffart <ogoffart at woboq.com>
 *  https://woboq.com/
 *
 *  This file is part of Verdigris: a way to use Qt without moc.
 *  https://github.com/woboq/verdigris
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program.
 *  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#ifndef Q_MOC_RUN // don't define anything when moc is run

#include <QtCore/qobjectdefs.h>
#include <QtCore/qmetatype.h>
#include <utility>

#define W_VERSION 0x010200

namespace w_internal {
using std::index_sequence; // From C++14, make sure to enable the C++14 option in your compiler

#ifdef W_USE_CUSTOM_MAKE_INDEX_SEQUENCE
// The default std::make_index_sequence from libstdc++ is recursing O(N) times which is reaching
// recursion limits level for big strings. This implementation has only O(log N) recursion
template<size_t... I1, size_t... I2>
index_sequence<I1... , (I2 +(sizeof...(I1)))...>
make_index_sequence_helper_merge(index_sequence<I1...>, index_sequence<I2...>);

template<std::size_t N> struct make_index_sequence_helper {
    using part1 = typename make_index_sequence_helper<(N+1)/2>::result;
    using part2 = typename make_index_sequence_helper<N/2>::result;
    using result = decltype(make_index_sequence_helper_merge(part1(), part2()));
};
template<> struct make_index_sequence_helper<1> { using result = index_sequence<0>; };
template<> struct make_index_sequence_helper<0> { using result = index_sequence<>; };
template<std::size_t N> using make_index_sequence = typename make_index_sequence_helper<N>::result;
#else
using std::make_index_sequence;
#endif

/// integral_constant with an inheritance based fallback
struct IndexBase {};
template <size_t> struct Index : IndexBase {};
template <size_t I> constexpr auto index = Index<I>{};

template<typename... Args> constexpr void ordered(Args...) {}
template<class T> constexpr void ordered2(std::initializer_list<T>) {}

/// Compute the sum of many integers
template<typename... Args>
constexpr int sums(Args... args) {
#if __cplusplus > 201700L
    return static_cast<int>((args + ... + 0));
#else
    auto r = int{};
    ordered2<int>({(r += args)...});
    return r;
#endif
}
// This indirection is required to work around a MSVC bug. (See https://github.com/woboq/verdigris/issues/6 )
template <int ...args>
constexpr int summed = sums(args...);

/// constexpr friendly string_view
struct StringView {
    const char* b{};
    const char* e{};

    constexpr auto size() const -> qptrdiff { return e - b; }
    constexpr auto begin() const { return b; }
    constexpr auto end() const { return e; }
};
// adding this constructor requires more constructors which generates overhead
template<size_t N>
constexpr auto viewLiteral(const char (&d)[N]) -> StringView {
    return {&d[0], &d[N-1]};
}

/// raw arrays cannot be returned from functions so we wrap it
template<size_t N = 0>
struct StringViewArray {
    StringView data[(N > 0 ? N : 1)]{};

    constexpr auto operator[] (size_t i) const { return data[i]; }
};


template<size_t SN>
constexpr auto countParsedLiterals(const char (&s)[SN]) {
    auto r = size_t{};
    auto i = 0u;
    while (true) {
        while (true) {
            if (i >= SN-1) return r;
            if (s[i] != ' ' && s[i] != ',') break; // start found
            i++;
        }
        r++;
        i++;
        while (true) {
            if (i >= SN-1) return r;
            if (s[i] == ',') break;
            i++;
        }
        i++;
    }
}

template<size_t N, size_t SN>
constexpr auto viewParsedLiterals(const char (&s)[SN]) -> StringViewArray<N> {
    // assumtions:
    // 1. s is generated from a macro
    // 2. each value is seperated by comma
    // 3. exactly N values are generated
    // 4. no second space can follow a space (no tabs or line breaks)
    auto r = StringViewArray<N>{};
    auto ri = size_t{};
    auto i = 0u;
    while (true) {
        while (true) {
            if (i >= SN-1) return r;
            if (s[i] != ' ' && s[i] != ',') break; // start found
            i++;
        }
        r.data[ri].b = &s[i];
        i++;
        while (true) {
            if (i >= SN-1) {
                r.data[ri].e = &s[i];
                return r;  // text - end
            }
            if (s[i] == ' ') {
                if (i+1 >= SN-1) {
                    r.data[ri].e = &s[i];
                    return r; // text - whitespace - end
                }
                if (s[i+1] == ',') {
                    r.data[ri].e = &s[i];
                    i += 2;
                    break; // text - whitespace - comma
                }
                i += 2;
                continue; // text - whitespace - text
            }
            if (s[i] == ',') {
                r.data[ri].e = &s[i];
                i++;
                break; // text - comma
            }
            i++;
        }
        ri++;
    }
}

template<size_t N, size_t SN>
constexpr auto viewScopedLiterals(const char (&s)[SN]) -> StringViewArray<N> {
    auto r = StringViewArray<N>{};
    auto ri = size_t{};
    auto i = 0u;
    while (true) {
        while (true) {
            if (i >= SN-1) return r;
            if (s[i] != ' ' && s[i] != ',' && s[i] != ':') break; // start found
            i++;
        }
        r.data[ri].b = &s[i];
        i++;
        while (true) {
            if (i >= SN-1) {
                r.data[ri].e = &s[i];
                return r;  // text - end
            }
            if (s[i] == ':') {
                i++;
                if (s[i] == ' ') i++;
                r.data[ri].b = &s[i];
                continue;
            }
            if (s[i] == ' ') {
                if (i+1 >= SN-1) {
                    r.data[ri].e = &s[i];
                    return r; // text - whitespace - end
                }
                if (s[i+1] == ',') {
                    r.data[ri].e = &s[i];
                    i += 2;
                    break; // text - whitespace - comma
                }
                i += 2;
                continue; // text - whitespace - text
            }
            if (s[i] == ',') {
                r.data[ri].e = &s[i];
                i++;
                break; // text - comma
            }
            i++;
        }
        ri++;
    }
}

//-----------

// From qmetaobject_p.h
enum class PropertyFlags  {
    Invalid = 0x00000000,
    Readable = 0x00000001,
    Writable = 0x00000002,
    Resettable = 0x00000004,
    EnumOrFlag = 0x00000008,
    StdCppSet = 0x00000100,
    //     Override = 0x00000200,
    Constant = 0x00000400,
    Final = 0x00000800,
    Designable = 0x00001000,
    ResolveDesignable = 0x00002000,
    Scriptable = 0x00004000,
    ResolveScriptable = 0x00008000,
    Stored = 0x00010000,
    ResolveStored = 0x00020000,
    Editable = 0x00040000,
    ResolveEditable = 0x00080000,
    User = 0x00100000,
    ResolveUser = 0x00200000,
    Notify = 0x00400000,
    Revisioned = 0x00800000
};
constexpr uint operator|(uint a, PropertyFlags b) { return a | uint(b); }

template <int N> struct W_MethodFlags { static constexpr int value = N; };
constexpr W_MethodFlags<0> W_EmptyFlag{};

} // namespace w_internal

/// Objects that are used as flags in the W_SLOT macro

// Mirror of QMetaMethod::Access
namespace W_Access {
    // From qmetaobject_p.h MethodFlags
    //    AccessPrivate = 0x00,
    //    AccessProtected = 0x01,
    //    AccessPublic = 0x02,
    //    AccessMask = 0x03, //mask
    constexpr w_internal::W_MethodFlags<0x1000> Private{}; // Note: Private has a higher number to differentiate it from the default
    constexpr w_internal::W_MethodFlags<0x01> Protected{};
    constexpr w_internal::W_MethodFlags<0x02> Public{};
}

// Mirror of QMetaMethod::MethodType
namespace W_MethodType {
    // From qmetaobject_p.h MethodFlags
    //    MethodMethod = 0x00,
    //    MethodSignal = 0x04,
    //    MethodSlot = 0x08,
    //    MethodConstructor = 0x0c,
    //    MethodTypeMask = 0x0c,
    constexpr w_internal::W_MethodFlags<0x00> Method{};
    constexpr w_internal::W_MethodFlags<0x04> Signal{};
    constexpr w_internal::W_MethodFlags<0x08> Slot{};
    constexpr w_internal::W_MethodFlags<0x0c> Constructor{};
}

// From qmetaobject_p.h MethodFlags
//    MethodCompatibility = 0x10,
//    MethodCloned = 0x20,
//    MethodScriptable = 0x40,
//    MethodRevisioned = 0x80
constexpr w_internal::W_MethodFlags<0x10> W_Compat{};
constexpr w_internal::W_MethodFlags<0x40> W_Scriptable{};
constexpr struct {} W_Notify{};
constexpr struct {} W_Reset{};
constexpr std::integral_constant<int, int(w_internal::PropertyFlags::Constant)> W_Constant{};
constexpr std::integral_constant<int, int(w_internal::PropertyFlags::Final)> W_Final{};

namespace w_internal {

/// Holds information about a method
template<typename F, int Flags, typename IC, typename ParamTypes, typename ParamNames = StringViewArray<>>
struct MetaMethodInfo {
    F func;
    StringView name;
    ParamTypes paramTypes;
    ParamNames paramNames;
    static constexpr int argCount = QtPrivate::FunctionPointer<F>::ArgumentCount;
    static constexpr auto argSequence = make_index_sequence<argCount>{};
    static constexpr int flags = Flags;
    using IntegralConstant = IC;
};

// Called from the W_SLOT macro
template<typename F, typename ParamTypes, int... Flags, typename IntegralConstant>
constexpr MetaMethodInfo<F, summed<Flags...> | W_MethodType::Slot.value, IntegralConstant, ParamTypes>
makeMetaSlotInfo(F f, StringView name, IntegralConstant, const ParamTypes &paramTypes, W_MethodFlags<Flags>...)
{ return { f, name, paramTypes, {} }; }

// Called from the W_METHOD macro
template<typename F, typename ParamTypes, int... Flags, typename IntegralConstant>
constexpr MetaMethodInfo<F, summed<Flags...> | W_MethodType::Method.value, IntegralConstant, ParamTypes>
makeMetaMethodInfo(F f, StringView name, IntegralConstant, const ParamTypes &paramTypes, W_MethodFlags<Flags>...)
{ return { f, name, paramTypes, {} }; }

// Called from the W_SIGNAL macro
template<typename F, typename ParamTypes, typename ParamNames, int... Flags, typename IntegralConstant>
constexpr MetaMethodInfo<F, summed<Flags...> | W_MethodType::Signal.value, IntegralConstant,
                            ParamTypes, ParamNames>
makeMetaSignalInfo(F f, StringView name, IntegralConstant, const ParamTypes &paramTypes,
                    const ParamNames &paramNames, W_MethodFlags<Flags>...)
{ return { f, name, paramTypes, paramNames }; }

/// Holds information about a constructor
template<typename... Args>
struct MetaConstructorInfo {
    static constexpr std::size_t argCount = sizeof...(Args);
    static constexpr auto argSequence = make_index_sequence<argCount>{};
    static constexpr int flags = W_MethodType::Constructor.value | W_Access::Public.value;
    using IntegralConstant = void*; // Used to detect the access specifier, but it is always public, so no need for this
    StringView name;
};
// called from the W_CONSTRUCTOR macro
template<typename...  Args>
constexpr MetaConstructorInfo<Args...> makeMetaConstructorInfo(StringView name)
{ return { name }; }

struct Empty{
    constexpr operator bool() const { return false; }
};

/// Holds information about a property
template<typename Type, typename Getter = Empty, typename Setter = Empty,
          typename Member = Empty, typename Notify = Empty, typename Reset = Empty, int Flags = 0>
struct MetaPropertyInfo {
    using PropertyType = Type;
    StringView name;
    StringView typeStr;
    Getter getter;
    Setter setter;
    Member member;
    Notify notify;
    Reset reset;
    static constexpr uint flags = Flags;

    template <typename S> constexpr auto setGetter(const S&s) const {
        return MetaPropertyInfo<Type, S, Setter, Member, Notify, Reset,
                                Flags | PropertyFlags::Readable>
        {name, typeStr, s, setter, member, notify, reset};
    }
    template <typename S> constexpr auto setSetter(const S&s) const {
        return MetaPropertyInfo<Type, Getter, S, Member, Notify, Reset,
                                Flags | PropertyFlags::Writable>
        {name, typeStr, getter, s, member, notify, reset};
    }
    template <typename S> constexpr auto setMember(const S&s) const {
        return MetaPropertyInfo<Type, Getter, Setter, S, Notify, Reset,
                                Flags | PropertyFlags::Writable | PropertyFlags::Readable>
        {name, typeStr, getter, setter, s, notify, reset};
    }
    template <typename S> constexpr auto setNotify(const S&s) const {
        return MetaPropertyInfo<Type, Getter, Setter, Member, S, Reset,
                                Flags | PropertyFlags::Notify>
        { name, typeStr, getter, setter, member, s, reset};
    }
    template <typename S> constexpr auto setReset(const S&s) const {
        return MetaPropertyInfo<Type, Getter, Setter, Member, Notify, S,
                                Flags | PropertyFlags::Resettable>
        { name, typeStr, getter, setter, member, notify, s};
    }
    template <int Flag> constexpr auto addFlag() const {
        return MetaPropertyInfo<Type, Getter, Setter, Member, Notify, Reset,
                                Flags | Flag>
        { name, typeStr, getter, setter, member, notify, reset};
    }
};

/// Parse a property and fill a MetaPropertyInfo (called from W_PROPERTY macro)
// base case
template <typename PropInfo> constexpr auto parseProperty(const PropInfo &p) { return p; }
// setter
template <typename PropInfo, typename Obj, typename Arg, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo &p, Ret (Obj::*s)(Arg), Tail... t)
{ return parseProperty(p.setSetter(s) , t...); }
#if defined(__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510
template <typename PropInfo, typename Obj, typename Arg, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo &p, Ret (Obj::*s)(Arg) noexcept, Tail... t)
{ return parseProperty(p.setSetter(s) , t...); }
#endif
// getter
template <typename PropInfo, typename Obj, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo &p, Ret (Obj::*s)(), Tail... t)
{ return parseProperty(p.setGetter(s), t...); }
template <typename PropInfo, typename Obj, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo &p, Ret (Obj::*s)() const, Tail... t)
{ return parseProperty(p.setGetter(s), t...); }
#if defined(__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510
template <typename PropInfo, typename Obj, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo &p, Ret (Obj::*s)() noexcept, Tail... t)
{ return parseProperty(p.setGetter(s), t...); }
template <typename PropInfo, typename Obj, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo &p, Ret (Obj::*s)() const noexcept, Tail... t)
{ return parseProperty(p.setGetter(s), t...); }
#endif
// member
template <typename PropInfo, typename Obj, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo &p, Ret Obj::*s, Tail... t)
{ return parseProperty(p.setMember(s) ,t...); }
// notify
template <typename PropInfo, typename F, typename... Tail>
constexpr auto parseProperty(const PropInfo &p, decltype(W_Notify), F f, Tail... t)
{ return parseProperty(p.setNotify(f) ,t...); }
// reset
template <typename PropInfo, typename Obj, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo &p, decltype(W_Reset), Ret (Obj::*s)(), Tail... t)
{ return parseProperty(p.setReset(s) ,t...); }
#if defined(__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510
template <typename PropInfo, typename Obj, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo &p, decltype(W_Reset), Ret (Obj::*s)() noexcept, Tail... t)
{ return parseProperty(p.setReset(s) ,t...); }
#endif
// other flags flags
template <typename PropInfo, int Flag, typename... Tail>
constexpr auto parseProperty(const PropInfo &p, std::integral_constant<int, Flag>, Tail... t)
{ return parseProperty(p.template addFlag<Flag>() ,t...); }

template<typename T, typename ... Args>
constexpr auto makeMetaPropertyInfo(StringView name, StringView type, Args... args) {
    auto meta = MetaPropertyInfo<T>{ name, type, {}, {}, {}, {}, {} };
    return parseProperty(meta, args...);
}

template <typename T, typename = void> struct EnumIsScoped {
    enum { Value =  std::is_convertible<T, int>::value ? 0 : 2 };
};
template <typename T> struct EnumIsScoped<QFlags<T>, void> : EnumIsScoped<typename QFlags<T>::enum_type> {};

template<typename T> struct QEnumOrQFlags { using Type = T; };
template<typename T> struct QEnumOrQFlags<QFlags<T>> { using Type = T; };

/// Holds information about an enum
template<bool HasAlias, typename Values_, typename Names, int Flags>
struct MetaEnumInfo {
    StringView name;
    StringView alias;
    Names names;
    using Values = Values_;
    static constexpr uint flags = Flags;
    static constexpr auto count = Values::size();
    static constexpr auto sequence = make_index_sequence<count>{};
    static constexpr auto hasAlias = HasAlias;
};
template<typename Enum, Enum... Value> struct enum_sequence {};
// called from W_ENUM and W_FLAG
template<typename Enum, int Flag, Enum... Values, typename Names>
constexpr auto makeMetaEnumInfo(StringView name, int, enum_sequence<Enum, Values...>, Names names)
        -> MetaEnumInfo<false, index_sequence<size_t(Values)...>, Names, Flag | EnumIsScoped<Enum>::Value> {
    return { name, viewLiteral(""), names };
}
template<typename Enum, int Flag, Enum... Values, typename Names>
constexpr auto makeMetaEnumInfo(StringView name, StringView alias, enum_sequence<Enum, Values...>, Names names)
        -> MetaEnumInfo<true, index_sequence<size_t(Values)...>, Names, Flag | EnumIsScoped<Enum>::Value> {
    return { name, alias, names };
}

// Helper function to remove the scope in a scoped enum name. (so "Foo::Bar" -> Bar)
template<int N> constexpr int removedScopeSize(const char (&name)[N]) {
    // Find the position of the last colon (or 0 if it is not there)
    int p = N - 1;
    while (p > 0 && name[p] != ':')
        p--;
    if (name[p] == ':')
        p++;
    return N - p;
}

/// Helper for the implementation of a signal.
/// Called from the signal implementation within the W_SIGNAL macro.
///
/// 'Func' is the type of the signal. 'Idx' is the signal index (relative).
/// It is implemented as a functor so the operator() has exactly the same amount of argument of the
/// signal so the __VA_ARGS__ works also if there is no arguments (no leading commas)
///
/// There is specialization for const and non-const,  and for void and non-void signals.
///
/// the last argument of the operator() is an int, to workaround the ",0" required in the W_SIGNAL
/// macro to make sure there is at least one argument for the ...
template<typename Func, int Idx> struct SignalImplementation {};
template<typename Obj, typename Ret, typename... Args, int Idx>
struct SignalImplementation<Ret (Obj::*)(Args...), Idx>{
    Obj *this_;
    Ret operator()(Args... args, int) const {
        Ret r{};
        const void * a[]= { std::addressof(r), std::addressof(args)... };
        QMetaObject::activate(this_, &Obj::staticMetaObject, Idx, const_cast<void **>(a));
        return r;
    }
};
template<typename Obj, typename... Args, int Idx>
struct SignalImplementation<void (Obj::*)(Args...), Idx>{
    Obj *this_;
    void operator()(Args... args, int) {
        const void * a[]= { nullptr, std::addressof(args)... };
        QMetaObject::activate(this_, &Obj::staticMetaObject, Idx, const_cast<void **>(a));
    }
};
template<typename Obj, typename Ret, typename... Args, int Idx>
struct SignalImplementation<Ret (Obj::*)(Args...) const, Idx>{
    const Obj *this_;
    Ret operator()(Args... args, int) const {
        Ret r{};
        const void * a[]= { std::addressof(r), std::addressof(args)... };
        QMetaObject::activate(const_cast<Obj*>(this_), &Obj::staticMetaObject, Idx, const_cast<void **>(a));
        return r;
    }
};
template<typename Obj, typename... Args, int Idx>
struct SignalImplementation<void (Obj::*)(Args...) const, Idx>{
    const Obj *this_;
    void operator()(Args... args, int) {
        const void * a[]= { nullptr, std::addressof(args)... };
        QMetaObject::activate(const_cast<Obj*>(this_), &Obj::staticMetaObject, Idx, const_cast<void **>(a));
    }
};

/// Used in the W_OBJECT macro to compute the base type.
/// Used like this:
///  using W_BaseType = std::remove_reference_t<decltype(getParentObjectHelper(&W_ThisType::qt_metacast))>;
/// Since qt_metacast for W_ThisType will be declared later, the pointer to member function will be
/// pointing to the qt_metacast of the base class, so T will be deduced to the base class type.
///
/// Returns a reference so this work if T is an abstract class.
template<typename T> T &getParentObjectHelper(void* (T::*)(const char*));

// helper class that can access the private member of any class with W_OBJECT
struct FriendHelper;

inline namespace w_ShouldBeInQt {
// qOverload is already in Qt 5.7, but we need it with older version.
// Note that as of Qt 5.11, it is still not enabled with MSVC as Qt relies on feature macro.
template <typename... Args>
struct QNonConstOverload
{
    template <typename R, typename T>
    constexpr auto operator()(R (T::*ptr)(Args...)) const
    { return ptr; }
};
template <typename... Args>
struct QConstOverload
{
    template <typename R, typename T>
    constexpr auto operator()(R (T::*ptr)(Args...) const) const
    { return ptr; }
};
template <typename... Args>
struct QOverload : QConstOverload<Args...>, QNonConstOverload<Args...>
{
    using QConstOverload<Args...>::operator();
    using QNonConstOverload<Args...>::operator();

    template <typename R>
    constexpr auto operator()(R (*ptr)(Args...)) const
    { return ptr; }
};
template <typename... Args> constexpr QOverload<Args...> qOverload = {};

#ifndef QT_ANNOTATE_CLASS // Was added in Qt 5.6.1
#define QT_ANNOTATE_CLASS(...)
#endif
} // namespace w_ShouldBeInQt

} // namespace w_internal

#if defined(Q_CC_MSVC) && !defined(Q_CC_CLANG)
  #if _MSC_VER >= 1928
    #if defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL
      // MSVC in /Zc:preprocessor- mode (default)
      #define W_CONFORMING_PREPROCESSOR 0
    #else
      // MSVC in /Zc:preprocessor mode
      #define W_CONFORMING_PREPROCESSOR 1
    #endif
  #else
    // Older versions of MSVC without the /Zc:preprocessor flag
    #define W_CONFORMING_PREPROCESSOR 0
  #endif
#else
  // Other compilers, generally conforming
  #define W_CONFORMING_PREPROCESSOR 1
#endif

#if !W_CONFORMING_PREPROCESSOR
// Workaround for MSVC: expension rules are different so we need some extra macro.
#define W_MACRO_MSVC_EXPAND(...) __VA_ARGS__
#define W_MACRO_MSVC_DELAY(X,...) W_MACRO_MSVC_EXPAND(X(__VA_ARGS__))
#define W_MACRO_MSVC_EMPTY /##* *##/
#else
#define W_MACRO_MSVC_EXPAND(...) __VA_ARGS__
#define W_MACRO_MSVC_DELAY(X,...) X(__VA_ARGS__)
#define W_MACRO_MSVC_EMPTY
#endif

// Private macro helpers for  macro programming
#define W_MACRO_EMPTY
#define W_MACRO_EVAL(...) __VA_ARGS__
#define W_MACRO_DELAY(X,...) W_MACRO_MSVC_EXPAND(X(__VA_ARGS__))
#define W_MACRO_DELAY2(X,...) W_MACRO_MSVC_EXPAND(X(__VA_ARGS__))
#define W_MACRO_TAIL(A, ...) __VA_ARGS__
#define W_MACRO_FIRST(...) W_MACRO_MSVC_EXPAND(W_MACRO_FIRST2(__VA_ARGS__,))
#define W_MACRO_FIRST2(A, ...) A
#define W_MACRO_STRIGNIFY(...) W_MACRO_STRIGNIFY2(__VA_ARGS__)
#define W_MACRO_STRIGNIFY2(...) #__VA_ARGS__
#define W_MACRO_CONCAT(A, B) W_MACRO_CONCAT2(A,B)
#define W_MACRO_CONCAT2(A, B) A##_##B

// strignify and make a StaticStringList out of an array of arguments
#define W_PARAM_TOSTRING(...) W_MACRO_MSVC_EMPTY W_MACRO_MSVC_DELAY(W_PARAM_TOSTRING2,__VA_ARGS__)
#define W_PARAM_TOSTRING2(...) w_internal::viewParsedLiterals<w_internal::countParsedLiterals("" #__VA_ARGS__)>("" #__VA_ARGS__)

#define W_PARAM_TOSTRING_REMOVE_SCOPE(...) W_MACRO_MSVC_EMPTY W_MACRO_MSVC_DELAY(W_PARAM_TOSTRING2_REMOVE_SCOPE,__VA_ARGS__)
#define W_PARAM_TOSTRING2_REMOVE_SCOPE(...) w_internal::viewScopedLiterals<w_internal::countParsedLiterals("" #__VA_ARGS__)>("" #__VA_ARGS__)

// remove the surrounding parentheses
#define W_MACRO_REMOVEPAREN(A) W_MACRO_DELAY(W_MACRO_REMOVEPAREN2, W_MACRO_REMOVEPAREN_HELPER A)
#define W_MACRO_REMOVEPAREN2(...) W_MACRO_DELAY2(W_MACRO_TAIL, W_MACRO_REMOVEPAREN_HELPER_##__VA_ARGS__)
#define W_MACRO_REMOVEPAREN_HELPER(...) _ , __VA_ARGS__
#define W_MACRO_REMOVEPAREN_HELPER_W_MACRO_REMOVEPAREN_HELPER ,

// if __VA_ARGS__ is "(types), foobar"   then return just the types, otherwise return nothing
#define W_OVERLOAD_TYPES(...) W_MACRO_DELAY(W_OVERLOAD_TYPES2,W_OVERLOAD_TYPES_HELPER __VA_ARGS__,)
#define W_OVERLOAD_TYPES2(A,...) W_MACRO_DELAY2(W_OVERLOAD_TYPES3, W_OVERLOAD_TYPES_HELPER_##A ...)
#define W_OVERLOAD_TYPES3(A,...) W_MACRO_EVAL A
#define W_OVERLOAD_TYPES_HELPER(...) YES(__VA_ARGS__)
#define W_OVERLOAD_TYPES_HELPER_W_OVERLOAD_TYPES_HELPER (),
#define W_OVERLOAD_TYPES_HELPER_YES(...) (__VA_ARGS__),

#define W_OVERLOAD_RESOLVE(...) W_MACRO_DELAY(W_OVERLOAD_RESOLVE2,W_OVERLOAD_RESOLVE_HELPER __VA_ARGS__,)
#define W_OVERLOAD_RESOLVE2(A, ...) W_MACRO_DELAY2(W_MACRO_FIRST,W_OVERLOAD_RESOLVE_HELPER_##A)
#define W_OVERLOAD_RESOLVE_HELPER(...) YES(w_internal::qOverload<__VA_ARGS__>)
#define W_OVERLOAD_RESOLVE_HELPER_YES(...) (__VA_ARGS__)
#define W_OVERLOAD_RESOLVE_HELPER_W_OVERLOAD_RESOLVE_HELPER ,


// remove the first argument if it is in parentheses"
#define W_OVERLOAD_REMOVE(...) W_MACRO_DELAY(W_OVERLOAD_REMOVE2, W_OVERLOAD_REMOVE_HELPER __VA_ARGS__)
#define W_OVERLOAD_REMOVE2(...) W_MACRO_DELAY2(W_MACRO_TAIL, W_OVERLOAD_REMOVE_HELPER_##__VA_ARGS__)

#define W_OVERLOAD_REMOVE_HELPER(...) _
#define W_OVERLOAD_REMOVE_HELPER_W_OVERLOAD_REMOVE_HELPER ,

#define W_RETURN(R) -> decltype(R) { return R; }

#ifndef Q_CC_MSVC
//Define a unique integral_constant type for a given function pointer
#define W_INTEGRAL_CONSTANT_HELPER(NAME, ...) std::integral_constant<decltype(W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME)), &W_ThisType::NAME>
#else
// On MSVC 2017 we trying to get a function pointer in the type cause compiler internal error, so use a simple hash function
namespace w_internal {
constexpr auto simple_hash(char const *p) {
    unsigned long long h = *p;
    while (*p++) h = ((h >> 8)*37ull)  ^ *p ^ ((h & 0xff) << 56) ;
    return h;
}
} // namespace w_internal
#define W_INTEGRAL_CONSTANT_HELPER(NAME, ...) std::integral_constant<unsigned long long, w_internal::simple_hash(#NAME #__VA_ARGS__)>
#endif

namespace w_internal {

/// We store state in overloads for this method.
/// This overload is found if no better overload was found.
/// All overloads are found using ADL in the QObject T
template<class State, class TPP>
void w_state(IndexBase, State, TPP);

#if __cplusplus > 201700L
template<size_t L, class State, class TPP, size_t N , size_t M, size_t X = (N+M)/2>
constexpr size_t countBetween() {
    using R = decltype(w_state(index<X>, State{}, TPP{}));
    if constexpr (N==X) {
        return std::is_same_v<void, R> ? N : M;
    }
    else if constexpr (std::is_same_v<void, R>) {
        return countBetween<L, State, TPP, N, X>();
    }
    else {
        return countBetween<L, State, TPP, X, M>();
    }
}
template<size_t L, class State, class TPP, size_t N = 1>
constexpr size_t count() {
    using R = decltype(w_state(index<N>, State{}, TPP{}));
    if constexpr (std::is_same_v<void, R>) {
        return countBetween<L, State, TPP, N/2, N>();
    }
    else {
        return count<L, State, TPP, N*2>();
    }
}

template<size_t L, class State, class TPP>
constexpr auto stateCount = count<L, State, TPP>();
#else
template<size_t L, class State, class TPP
          , size_t N, size_t M, size_t X = (N+M)/2
          , bool noX = std::is_same<void, decltype(w_state(index<X>, State{}, TPP{}))>::value
          , bool up = N==X>
struct CountBetween;

template<size_t L, class State, class TPP, size_t N, size_t M, size_t X>
struct CountBetween<L, State, TPP, N, M, X, true, true> {
    static constexpr auto value = N;
};
template<size_t L, class State, class TPP, size_t N, size_t M, size_t X>
struct CountBetween<L, State, TPP, N, M, X, false, true> {
    static constexpr auto value = M;
};
template<size_t L, class State, class TPP, size_t N, size_t M, size_t X>
struct CountBetween<L, State, TPP, N, M, X, true, false> {
    static constexpr auto value = CountBetween<L, State, TPP, N, X>::value;
};
template<size_t L, class State, class TPP, size_t N, size_t M, size_t X>
struct CountBetween<L, State, TPP, N, M, X, false, false> {
    static constexpr auto value = CountBetween<L, State, TPP, X, M>::value;
};

template<size_t L, class State, class TPP
          , size_t N = 1
          , bool noX = std::is_same<void, decltype(w_state(index<N>, State{}, TPP{}))>::value>
struct Count;

template<size_t L, class State, class TPP, size_t N>
struct Count<L, State, TPP, N, true> {
    static constexpr auto value = CountBetween<L, State, TPP, N/2, N>::value;
};
template<size_t L, class State, class TPP, size_t N>
struct Count<L, State, TPP, N, false> {
    static constexpr auto value = Count<L, State, TPP, N*2>::value;
};

template<size_t L, class State, class TPP>
constexpr auto stateCount = Count<L, State, TPP>::value;
#endif

struct SlotStateTag {};
struct SignalStateTag {};
struct MethodStateTag {};
struct ConstructorStateTag {};
struct PropertyStateTag {};
struct EnumStateTag {};
struct ClassInfoStateTag {};
struct InterfaceStateTag {};

} // namespace w_internal

#define W_OBJECT_COMMON(TYPE) \
        using W_ThisType = TYPE; \
        static constexpr auto W_UnscopedName = w_internal::viewLiteral(#TYPE); /* so we don't repeat it in W_CONSTRUCTOR */ \
        friend struct w_internal::FriendHelper; \
        template<typename W_Flag> static inline constexpr int w_flagAlias(W_Flag) { return 0; } \
    public: \
        struct W_MetaObjectCreatorHelper;

#define W_STATE_APPEND(STATE, ...) \
    friend constexpr auto w_state(w_internal::Index<w_internal::stateCount<__COUNTER__, w_internal::STATE##Tag, W_ThisType**>>, \
            w_internal::STATE##Tag, W_ThisType**) W_RETURN((__VA_ARGS__))
#define W_STATE_APPEND_NS(STATE, ...) \
    static constexpr auto w_state(w_internal::Index<w_internal::stateCount<__COUNTER__, w_internal::STATE##Tag, W_ThisType**>>, \
            w_internal::STATE##Tag, W_ThisType**) W_RETURN((__VA_ARGS__))

// public macros

/// \macro W_OBJECT(TYPE)
/// Like the Q_OBJECT macro, this declare that the object might have signals, slots or properties.
/// Must contains the class name as a parameter and need to be put before any other W_ macro in the class.
#define W_OBJECT(TYPE) \
    W_OBJECT_COMMON(TYPE) \
    public: \
        using W_BaseType = std::remove_reference_t<decltype(\
            w_internal::getParentObjectHelper(&W_ThisType::qt_metacast))>; \
    Q_OBJECT \
    QT_ANNOTATE_CLASS(qt_fake, "")

/// \macro W_GADGET(TYPE)
/// Like the Q_GADGET macro, this declare that the object might have properties.
/// Must contains the class name as a parameter and need to be put before any other W_ macro in the class.
#define W_GADGET(TYPE) \
    W_OBJECT_COMMON(TYPE) \
    Q_GADGET \
    QT_ANNOTATE_CLASS(qt_fake, "")

/// \macro W_NAMESPACE(NAMESPACE)
/// Like the Q_GADGET macro, this declare that the object might have properties.
/// Must contains the class name as a parameter and need to be put before any other W_ macro in the class.
#define W_NAMESPACE(NAMESPACE) \
    Q_NAMESPACE \
    struct W_MetaObjectCreatorHelper; \
    struct W_ThisType { \
        using W_MetaObjectCreatorHelper = NAMESPACE::W_MetaObjectCreatorHelper; \
        static constexpr auto qt_static_metacall = nullptr; \
    }; \
    static constexpr auto W_UnscopedName = w_internal::viewLiteral(#NAMESPACE); \
    template<typename W_Flag> Q_DECL_UNUSED static inline constexpr int w_flagAlias(W_Flag) { Q_UNUSED(W_UnscopedName) return 0; } \
    QT_ANNOTATE_CLASS(qt_fake, "")

/// \macro W_SLOT( <slot name> [, (<parameters types>) ]  [, <flags>]* )
///
/// The W_SLOT macro needs to be put after the slot declaration.
///
/// The W_SLOT macro can optionally have a list of parameter types as second argument to disambiguate
/// overloads or use types that are not registered with W_REGISTER_ARGTYPE. The list of parameter
/// need to be within parentheses (even if there is 0 or 1 argument).
///
/// The W_SLOT macro can have flags:
/// - Specifying the the access:  W_Access::Protected, W_Access::Private
///   or W_Access::Public. (By default, it is auto-detected from the location of this macro.)
/// - W_Compat: for deprecated methods (equivalent of Q_MOC_COMPAT)
#define W_SLOT(...) W_MACRO_MSVC_EXPAND(W_SLOT2(__VA_ARGS__, w_internal::W_EmptyFlag))
#define W_SLOT2(NAME, ...) \
    W_STATE_APPEND(SlotState, w_internal::makeMetaSlotInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), w_internal::viewLiteral(#NAME),  \
            W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)(), \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), \
            W_OVERLOAD_REMOVE(__VA_ARGS__))) \
    static inline void w_GetAccessSpecifierHelper(W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)) {}

/// \macro W_INVOKABLE( <slot name> [, (<parameters types>) ]  [, <flags>]* )
/// Exactly like W_SLOT but for Q_INVOKABLE methods.
#define W_INVOKABLE(...) W_MACRO_MSVC_EXPAND(W_INVOKABLE2(__VA_ARGS__, w_internal::W_EmptyFlag))
#define W_INVOKABLE2(NAME, ...) \
    W_STATE_APPEND(MethodState, w_internal::makeMetaMethodInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), w_internal::viewLiteral(#NAME),  \
            W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)(), \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), \
            W_OVERLOAD_REMOVE(__VA_ARGS__))) \
    static inline void w_GetAccessSpecifierHelper(W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)) {}

/// <signal signature>
/// \macro W_SIGNAL(<signal name> [, (<parameter types>) ] , <parameter names> )
///
/// Unlike W_SLOT, W_SIGNAL must be placed directly after the signal signature declaration.
/// There should not be a semi colon between the signal signature and the macro
///
/// Like W_SLOT, there can be the types of the parametter as a second argument, within parentheses.
/// You must then follow with the parameter names
///
/// Note about exported classes: since the signal is inline, GCC won't export it when compiling
/// with -fvisibility-inlines-hidden (which is the default), so connecting using pointer to member
/// functions won't work accross library boundaries. You need to explicitly export the signal with
/// your MYLIB_EXPORT macro in front of the signal declaration.
#define W_SIGNAL(...) W_MACRO_MSVC_EXPAND(W_SIGNAL2(__VA_ARGS__ , 0))
#define W_SIGNAL2(NAME, ...) \
    { /* W_SIGNAL need to be placed directly after the signal declaration, without semicolon. */\
        using w_SignalType = decltype(W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME)); \
        return w_internal::SignalImplementation<w_SignalType, W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__)>{this}(W_OVERLOAD_REMOVE(__VA_ARGS__)); \
    } \
    static constexpr int W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__) = \
        w_internal::stateCount<__COUNTER__, w_internal::SignalStateTag, W_ThisType**>; \
    friend constexpr auto w_state(w_internal::Index<W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__)>, w_internal::SignalStateTag, W_ThisType**) \
        W_RETURN(w_internal::makeMetaSignalInfo( \
                W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), w_internal::viewLiteral(#NAME), \
                W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)(), \
                W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), W_PARAM_TOSTRING(W_OVERLOAD_REMOVE(__VA_ARGS__)))) \
    static inline void w_GetAccessSpecifierHelper(W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)) {}

/// \macro W_SIGNAL_COMPAT
/// Same as W_SIGNAL, but set the W_Compat flag
#define W_SIGNAL_COMPAT(...) W_MACRO_MSVC_EXPAND(W_SIGNAL_COMPAT2(__VA_ARGS__, 0))
#define W_SIGNAL_COMPAT2(NAME, ...) \
    { \
        using w_SignalType = decltype(W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME)); \
        return w_internal::SignalImplementation<w_SignalType, W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__)>{this}(W_OVERLOAD_REMOVE(__VA_ARGS__)); \
    } \
    static constexpr int W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__) = \
        w_internal::stateCount<__COUNTER__, w_internal::SignalStateTag, W_ThisType**>; \
    friend constexpr auto w_state(w_internal::Index<W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__)>, w_internal::SignalStateTag, W_ThisType**) \
        W_RETURN(w_internal::makeMetaSignalInfo( \
                W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), w_internal::viewLiteral(#NAME), \
                W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)(), \
                W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), W_PARAM_TOSTRING(W_OVERLOAD_REMOVE(__VA_ARGS__)), W_Compat)) \
    static inline void w_GetAccessSpecifierHelper(W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)) {}

/// \macro W_CONSTRUCTOR(<parameter types>)
/// Declares that this class can be constructed with this list of argument.
/// Equivalent to Q_INVOKABLE constructor.
/// One can have W_CONSTRUCTOR() for the default constructor even if it is implicit.
#define W_CONSTRUCTOR(...) \
    W_STATE_APPEND(ConstructorState, \
                   w_internal::makeMetaConstructorInfo<__VA_ARGS__>(W_UnscopedName))


/// \macro W_PROPERTY(<type>, <name> [, <flags>]*)
///
/// Declare a property <name> with the type <type>.
/// The flags can be function pointers that are detected to be setter, getters, notify signal or
/// other flags. Use the macro READ, WRITE, MEMBER, ... for the flag so you can write W_PROPERTY
/// just like in a Q_PROPERTY. The only difference with Q_PROPERTY would be the semicolon before the
/// name.
/// W_PROPERTY need to be put after all the setters, getters, signals and members have been declared.
///
/// <type> can optionally be put in parentheses, if you have a type containing a comma
///
/// The macro to be used inside W_PROPERTY can also be prefixed by W_ (W_READ, W_WRITE, ...)
/// If you define the macro W_NO_PROPERTY_MACRO in your build settings, the macro without prefix
/// won't be defined.
#define W_PROPERTY(...) W_MACRO_MSVC_EXPAND(W_PROPERTY2(__VA_ARGS__)) // expands the READ, WRITE, and other sub marcos
#define W_PROPERTY2(TYPE, NAME, ...) \
    W_STATE_APPEND(PropertyState, \
        w_internal::makeMetaPropertyInfo<W_MACRO_REMOVEPAREN(TYPE)>(\
                            w_internal::viewLiteral(#NAME), w_internal::viewLiteral(W_MACRO_STRIGNIFY(W_MACRO_REMOVEPAREN(TYPE))), __VA_ARGS__))

#define W_WRITE , &W_ThisType::
#define W_READ , &W_ThisType::
#define W_NOTIFY , W_Notify, &W_ThisType::
#define W_RESET , W_Reset, &W_ThisType::
#define W_MEMBER , &W_ThisType::
#define W_CONSTANT , W_Constant
#define W_FINAL , W_Final

#ifndef W_NO_PROPERTY_MACRO
#define WRITE     W_WRITE
#define READ      W_READ
#define NOTIFY    W_NOTIFY
#define RESET     W_RESET
#define MEMBER    W_MEMBER
#define CONSTANT  W_CONSTANT
#define FINAL     W_FINAL
#endif

#undef Q_PRIVATE_PROPERTY // the official macro is not a variadic macro, and the coma in READ would break it
#define Q_PRIVATE_PROPERTY(...)

/// \macro W_ENUM(<name>, <values>)
/// Similar to Q_ENUM, but one must also manually write all the values.
#define W_ENUM(NAME, ...) \
    W_STATE_APPEND(EnumState, w_internal::makeMetaEnumInfo<NAME,false>( \
            w_internal::viewLiteral(#NAME), w_flagAlias(NAME{}), \
            w_internal::enum_sequence<NAME,__VA_ARGS__>{}, W_PARAM_TOSTRING_REMOVE_SCOPE(__VA_ARGS__))) \
    Q_ENUM(NAME)


/// \macro W_ENUM_NS(<name>, <values>)
/// Similar to Q_ENUM_NS, like W_ENUM
#define W_ENUM_NS(NAME, ...) \
    W_STATE_APPEND_NS(EnumState, w_internal::makeMetaEnumInfo<NAME,false>( \
            w_internal::viewLiteral(#NAME), w_flagAlias(NAME{}), \
            w_internal::enum_sequence<NAME,__VA_ARGS__>{}, W_PARAM_TOSTRING_REMOVE_SCOPE(__VA_ARGS__))) \
    Q_ENUM_NS(NAME)

/// \macro W_FLAG(<name>, <values>)
/// Similar to Q_FLAG, but one must also manually write all the values.
#define W_FLAG(NAME, ...) \
    W_STATE_APPEND(EnumState, w_internal::makeMetaEnumInfo<w_internal::QEnumOrQFlags<NAME>::Type,true>( \
            w_internal::viewLiteral(#NAME), w_flagAlias(NAME{}), \
            w_internal::enum_sequence<w_internal::QEnumOrQFlags<NAME>::Type,__VA_ARGS__>{}, \
            W_PARAM_TOSTRING_REMOVE_SCOPE(__VA_ARGS__))) \
    Q_FLAG(NAME)

/// \macro W_FLAG_NS(<name>, <values>)
/// Similar to Q_FLAG_NS, like W_FLAG.
#define W_FLAG_NS(NAME, ...) \
    W_STATE_APPEND_NS(EnumState, w_internal::makeMetaEnumInfo<w_internal::QEnumOrQFlags<NAME>::Type,true>( \
            w_internal::viewLiteral(#NAME), w_flagAlias(NAME{}), \
            w_internal::enum_sequence<w_internal::QEnumOrQFlags<NAME>::Type,__VA_ARGS__>{}, \
            W_PARAM_TOSTRING_REMOVE_SCOPE(__VA_ARGS__))) \
    Q_FLAG_NS(NAME)

/// Same as Q_CLASSINFO.  Note, Q_CLASSINFO_NS is required for namespace
#define W_CLASSINFO(A, B) \
    W_STATE_APPEND(ClassInfoState, \
        std::pair<w_internal::StringView, w_internal::StringView>{ w_internal::viewLiteral(A), w_internal::viewLiteral(B) })

/// Same as Q_CLASSINFO, but within a namespace
#define W_CLASSINFO_NS(A, B) \
    W_STATE_APPEND_NS(ClassInfoState, \
        std::pair<w_internal::StringView, w_internal::StringView>{ w_internal::viewLiteral(A), w_internal::viewLiteral(B) })

/// Same as Q_INTERFACE
#define W_INTERFACE(A) \
    W_STATE_APPEND(InterfaceState, static_cast<A*>(nullptr))

/// Same as Q_DECLARE_FLAGS
#define W_DECLARE_FLAGS(Flags, Enum) \
    Q_DECLARE_FLAGS(Flags, Enum) \
    static inline constexpr w_internal::StringView w_flagAlias(Flags) { return w_internal::viewLiteral(#Enum); }

/// \macro W_REGISTER_ARGTYPE(TYPE)
/// Registers TYPE so it can be used as a parameter of a signal/slot or return value.
/// The normalized signature must be used.
/// Note: This does not imply Q_DECLARE_METATYPE, and Q_DECLARE_METATYPE does not imply this
namespace w_internal { template<typename T> struct W_TypeRegistery { enum { registered = false }; }; }
#define W_REGISTER_ARGTYPE(...) namespace w_internal { \
    template<> struct W_TypeRegistery<__VA_ARGS__> { \
    enum { registered = true }; \
    static constexpr auto name = viewLiteral(#__VA_ARGS__); \
  };}
W_REGISTER_ARGTYPE(char*)
W_REGISTER_ARGTYPE(const char*)

#else // Q_MOC_RUN
// just to avoid parse errors when moc is run over things that it should ignore
#define W_SIGNAL(...)        ;
#define W_SIGNAL_COMPAT(...) ;
#define W_PROPERTY(...)
#define W_SLOT(...)
#define W_CLASSINFO(...)
#define W_INTERFACE(...)
#define W_CONSTRUCTOR(...)
#define W_FLAG(...)
#define W_ENUM(...)
#define W_DECLARE_FLAGS(...)
#endif
