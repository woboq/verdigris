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

#define W_VERSION 0x0101ff

namespace w_internal {
using std::index_sequence;  // From C++14, make sure to enable the C++14 option in your compiler

/* The default std::make_index_sequence from libstdc++ is recursing O(N) times which is reaching
    recursion limits level for big strings. This implementation has only O(log N) recursion */
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

/* workaround for MSVC bug that can't do decltype(xxx)::foo when xxx is dependent of a template */
template<typename T> using identity_t = T;

/**
 * In this namespace we find the implementation of a template binary tree container
 * It has a similar API than std::tuple, but is stored in a binary way.
 * the libstdc++ std::tuple recurse something like 16*N for a tuple of N elements. (Because the copy
 * constructor uses traits for stuff like noexcept.) Which means we are quickly reaching the
 * implementation limit of recursion. (Cannot have more than  ~16 items in a tuple)
 * Also, a linear tuple happens to lead to very slow compilation times.
 *
 * So a std::tuple<T1, T2, T3, T4> is represented by a
 * binary::tree<Node<Node<Leaf<T1>, Leaf<T2>>, Node<Leaf<T3>, Leaf<T4>>>
 */
namespace binary {

    template <typename T> struct Leaf {
        T data;
        static constexpr int Depth = 0;
        static constexpr int Count = 1;
        static constexpr bool Balanced = true;
        template <int> constexpr T get() { return data; }
    };

    template <class A, class B> struct Node {
        A a;
        B b;
        static constexpr int Count = A::Count + B::Count;
        static constexpr int Depth = A::Depth + 1;
        static constexpr bool Balanced = A::Depth == B::Depth && B::Balanced;
        template <int N, typename = std::enable_if_t<(N < A::Count)>>
        constexpr auto get(int = 0) { return a.template get<N>(); }
        template <int N, typename = std::enable_if_t<(N >= A::Count)>>
        constexpr auto get(uint = 0) { return b.template get<N - A::Count>(); }
    };

    /** Add the node 'N' to the tree 'T'  (helper for tree_append) */
    template <class T, typename N, bool Balanced = T::Balanced > struct Add {
        typedef Node<T, Leaf<N> > Result;
        static constexpr Result add(T t, N n) { return {t, {n} }; }
    };
    template <class A, class B, typename N> struct Add<Node<A, B>, N, false> {
        typedef Node<A, typename Add<B, N>::Result > Result;
        static constexpr Result add(Node<A, B> t, N n) { return {t.a , Add<B, N>::add(t.b, n) }; }
    };

    /** Add the node 'N' to the tree 'T', on the left  (helper for tree_prepend) */
    template <class T, typename N, bool Balanced = T::Balanced > struct AddPre {
        typedef Node<Leaf<N> , T > Result;
        static constexpr Result add(T t, N n) { return {{n}, t}; }
    };
    template <class A, class B, typename N> struct AddPre<Node<A, B>, N, false> {
        typedef Node<typename AddPre<A, N>::Result, B > Result;
        static constexpr Result add(Node<A, B> t, N n) { return {AddPre<A, N>::add(t.a, n) , t.b }; }
    };

    /** helper for tree_tail */
    template<typename A, typename B> struct Tail;
    template<typename A, typename B> struct Tail<Leaf<A>, B> {
        using Result = B;
        static constexpr B tail(Node<Leaf<A>,B> t) { return t.b; }
    };
    template<typename A, typename B, typename C> struct Tail<Node<A,B>, C> {
        using Result = Node<typename Tail<A,B>::Result, C>;
        static constexpr Result tail(Node<Node<A,B>, C> t) { return { Tail<A,B>::tail(t.a) , t.b }; }
    };

    /** An equivalent of std::tuple hold in a binary tree for faster compile time */
    template<typename T = void> struct tree {
        static constexpr int size = T::Count;
        T root;
    };
    template<> struct tree<> { static constexpr int size = 0; };

    /**
     * tree_append(tree, T):  append an element at the end of the tree.
     */
    template<typename T>
    constexpr tree<Leaf<T>> tree_append(tree<>, T n)
    { return {{n}}; }
    template<typename Root, typename T>
    constexpr tree<typename Add<Root,T>::Result> tree_append(tree<Root> t, T n)
    { return {Add<Root,T>::add(t.root,n)}; }

    /**
     * tree_append(tree, T):  prepend an element at the beginning of the tree.
     */
    template<typename T>
    constexpr tree<Leaf<T>> tree_prepend(tree<>, T n)
    { return {{n}}; }
    template<typename Root, typename T>
    constexpr tree<typename AddPre<Root,T>::Result> tree_prepend(tree<Root> t, T n)
    { return {AddPre<Root,T>::add(t.root,n)}; }

    /**
     * get<N>(tree): Returns the element from the tree at index N.
     */
    template<int N, typename Root> constexpr auto get(tree<Root> t)
    { return t.root.template get<N>(); }

    /**
     * tree_tail(tree):  Returns a tree with the first element removed.
     */
    template<typename A, typename B>
    constexpr tree<typename Tail<A,B>::Result> tree_tail(tree<Node<A, B>> t)
    { return { Tail<A,B>::tail(t.root) }; }
    template<typename N>
    constexpr tree<> tree_tail(tree<Leaf<N>>) { return {}; }
    constexpr tree<> tree_tail(tree<>) { return {}; }

    /**
     * tree_head(tree): same as get<O> but return something invalid in case the tuple is too small.
     */
    template<typename T> constexpr auto tree_head(tree<T> t) { return get<0>(t); }
    constexpr auto tree_head(tree<void>) { struct _{}; return _{}; }
    template<typename T> constexpr auto tree_head(T) { struct _{}; return _{}; }

    template<std::size_t I, typename T> using tree_element_t = decltype(get<I>(std::declval<T>()));

    /**
     * tree_cat(tree1, tree2, ....): concatenate trees (like tuple_cat)
     */
    // FIXME: Should we balance?
    template<class A, class B>
    constexpr tree<Node<A,B>> tree_cat(tree<A> a, tree<B> b) { return { { a.root, b.root } }; }
    template<class A>
    constexpr tree<A> tree_cat(tree<>, tree<A> a) { return a; }
    template<class A>
    constexpr tree<A> tree_cat(tree<A> a, tree<>) { return a; }
    constexpr tree<> tree_cat(tree<>, tree<>) { return {}; }
    template<class A, class B, class C, class...D>
    constexpr auto tree_cat(A a, B b, C c, D... d) { return tree_cat(a, tree_cat(b, c, d...)); }
} // namespace binary


/** Compute the sum of many integers */
constexpr int sums() { return 0; }
template<typename... Args>
constexpr int sums(int i, Args... args) { return i + sums(args...);  }
// This indirection is required to work around a MSVC bug. (See https://github.com/woboq/verdigris/issues/6 )
template <int ...Args>
constexpr int summed = sums(Args...);

/*
 * Helpers to play with static strings
 */

/** A compile time character array of size N  */
template<std::size_t N> using StaticStringArray = const char [N];

/** Represents a string of size N  (N includes the '\0' at the end) */
template<std::size_t N, typename = make_index_sequence<N>> struct StaticString;
template<std::size_t N, std::size_t... I> struct StaticString<N, std::index_sequence<I...>>
{
    StaticStringArray<N> data;
    constexpr StaticString(StaticStringArray<N> &d) : data{ (d[I])... } { }
    static constexpr std::size_t size = N;
    constexpr char operator[](int p) const { return data[p]; }
};
template <std::size_t N> constexpr StaticString<N> makeStaticString(StaticStringArray<N> &d) { return {d}; }

/** A list containing many StaticString with possibly different sizes */
template<typename T = void> using StaticStringList = binary::tree<T>;

/** Make a StaticStringList out of many char array  */
constexpr StaticStringList<> makeStaticStringList() { return {}; }
template<typename... T>
constexpr StaticStringList<> makeStaticStringList(StaticStringArray<1> &, T...)
{ return {}; }
template<std::size_t N, typename... T>
constexpr auto makeStaticStringList(StaticStringArray<N> &h, T&...t)
{ return binary::tree_prepend(makeStaticStringList(t...), StaticString<N>(h)); }
template<typename... T>
constexpr StaticStringList<> makeStaticStringList(StaticString<1>, T...)
{ return {}; }
template<std::size_t N, typename... T>
constexpr auto makeStaticStringList(StaticString<N> h, T...t)
{ return binary::tree_prepend(makeStaticStringList(t...), h); }


/** Add a string in a StaticStringList */
template<std::size_t L, typename T>
constexpr auto addString(const StaticStringList<T> &l, const StaticString<L> & s) {
    return binary::tree_append(l, s);
}


/*-----------*/

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

/** w_number<I> is a helper to implement state */
template<int N = 255> struct w_number : public w_number<N - 1> {
    static constexpr int value = N;
    static constexpr w_number<N-1> prev() { return {}; }
};
template<> struct w_number<0> { static constexpr int value = 0; };

template <int N> struct W_MethodFlags { static constexpr int value = N; };
constexpr W_MethodFlags<0> W_EmptyFlag{};
} // w_internal

/** Objects that can be used as flags in the W_SLOT macro */

// Mirror of QMetaMethod::Access
namespace W_Access {
/* // From qmetaobject_p.h MethodFlags
    AccessPrivate = 0x00,
    AccessProtected = 0x01,
    AccessPublic = 0x02,
    AccessMask = 0x03, //mask
 */
    constexpr w_internal::W_MethodFlags<0x1000> Private{}; // Note: Private has a higher number to differentiate it from the default
    constexpr w_internal::W_MethodFlags<0x01> Protected{};
    constexpr w_internal::W_MethodFlags<0x02> Public{};
}

// Mirror of QMetaMethod::MethodType
namespace W_MethodType {
    /*  // From qmetaobject_p.h MethodFlags
        MethodMethod = 0x00,
        MethodSignal = 0x04,
        MethodSlot = 0x08,
        MethodConstructor = 0x0c,
        MethodTypeMask = 0x0c,
    */
    constexpr w_internal::W_MethodFlags<0x00> Method{};
    constexpr w_internal::W_MethodFlags<0x04> Signal{};
    constexpr w_internal::W_MethodFlags<0x08> Slot{};
    constexpr w_internal::W_MethodFlags<0x0c> Constructor{};
}

/*
MethodCompatibility = 0x10,
MethodCloned = 0x20,
MethodScriptable = 0x40,
MethodRevisioned = 0x80
*/
constexpr w_internal::W_MethodFlags<0x10> W_Compat{};
constexpr w_internal::W_MethodFlags<0x40> W_Scriptable{};
constexpr struct {} W_Notify{};
constexpr struct {} W_Reset{};
constexpr std::integral_constant<int, int(w_internal::PropertyFlags::Constant)> W_Constant{};
constexpr std::integral_constant<int, int(w_internal::PropertyFlags::Final)> W_Final{};

namespace w_internal {

/** Holds information about a method */
template<typename F, std::size_t NameLength, int Flags, typename IC, typename ParamTypes, typename ParamNames = StaticStringList<>>
struct MetaMethodInfo {
    F func;
    StaticString<NameLength> name;
    ParamTypes paramTypes;
    ParamNames paramNames;
    static constexpr int argCount = QtPrivate::FunctionPointer<F>::ArgumentCount;
    static constexpr int flags = Flags;
    using IntegralConstant = IC;
};

// Called from the W_SLOT macro
template<typename F, std::size_t N, typename ParamTypes, int... Flags, typename IntegralConstant>
constexpr MetaMethodInfo<F, N, summed<Flags...> | W_MethodType::Slot.value, IntegralConstant, ParamTypes>
makeMetaSlotInfo(F f, StaticStringArray<N> &name, IntegralConstant, const ParamTypes &paramTypes, W_MethodFlags<Flags>...)
{ return { f, {name}, paramTypes, {} }; }

// Called from the W_METHOD macro
template<typename F, std::size_t N, typename ParamTypes, int... Flags, typename IntegralConstant>
constexpr MetaMethodInfo<F, N, summed<Flags...> | W_MethodType::Method.value, IntegralConstant, ParamTypes>
makeMetaMethodInfo(F f, StaticStringArray<N> &name, IntegralConstant, const ParamTypes &paramTypes, W_MethodFlags<Flags>...)
{ return { f, {name}, paramTypes, {} }; }

// Called from the W_SIGNAL macro
template<typename F, std::size_t N, typename ParamTypes, typename ParamNames, int... Flags, typename IntegralConstant>
constexpr MetaMethodInfo<F, N, summed<Flags...> | W_MethodType::Signal.value, IntegralConstant,
                            ParamTypes, ParamNames>
makeMetaSignalInfo(F f, StaticStringArray<N> &name, IntegralConstant, const ParamTypes &paramTypes,
                    const ParamNames &paramNames, W_MethodFlags<Flags>...)
{ return { f, {name}, paramTypes, paramNames }; }

/** Holds information about a constructor */
template<std::size_t NameLength, typename... Args> struct MetaConstructorInfo {
    static constexpr std::size_t argCount = sizeof...(Args);
    static constexpr int flags = W_MethodType::Constructor.value | W_Access::Public.value;
    using IntegralConstant = void*; // Used to detect the access specifier, but it is always public, so no need for this
    StaticString<NameLength> name;
    template<std::size_t N>
    constexpr MetaConstructorInfo<N, Args...> setName(StaticStringArray<N> &n)
    { return { { n } }; }
    template<typename T, std::size_t... I>
    void createInstance(void **_a, std::index_sequence<I...>) const {
        *reinterpret_cast<T**>(_a[0]) =
            new T(*reinterpret_cast<std::remove_reference_t<Args> *>(_a[I+1])...);
    }
};
// called from the W_CONSTRUCTOR macro
template<typename...  Args> constexpr MetaConstructorInfo<1,Args...> makeMetaConstructorInfo()
{ return { {""} }; }

struct Empty{
    constexpr operator bool() const { return false; }
};

/** Holds information about a property */
template<typename Type, std::size_t NameLength, std::size_t TypeLength, typename Getter = Empty,
            typename Setter = Empty, typename Member = Empty,
            typename Notify = Empty, typename Reset = Empty, int Flags = 0>
struct MetaPropertyInfo {
    using PropertyType = Type;
    StaticString<NameLength> name;
    StaticString<TypeLength> typeStr;
    Getter getter;
    Setter setter;
    Member member;
    Notify notify;
    Reset reset;
    static constexpr uint flags = Flags;

    template <typename S> constexpr auto setGetter(const S&s) const {
        return MetaPropertyInfo<Type, NameLength, TypeLength, S, Setter, Member, Notify, Reset,
                                Flags | PropertyFlags::Readable>
        {name, typeStr, s, setter, member, notify, reset};
    }
    template <typename S> constexpr auto setSetter(const S&s) const {
        return MetaPropertyInfo<Type, NameLength, TypeLength, Getter, S, Member, Notify, Reset,
                                Flags | PropertyFlags::Writable>
        {name, typeStr, getter, s, member, notify, reset};
    }
    template <typename S> constexpr auto setMember(const S&s) const {
        return MetaPropertyInfo<Type, NameLength, TypeLength, Getter, Setter, S, Notify, Reset,
                                Flags | PropertyFlags::Writable | PropertyFlags::Readable>
        {name, typeStr, getter, setter, s, notify, reset};
    }
    template <typename S> constexpr auto setNotify(const S&s) const {
        return MetaPropertyInfo<Type, NameLength, TypeLength, Getter, Setter, Member, S, Reset,
                                Flags | PropertyFlags::Notify>
        { name, typeStr, getter, setter, member, s, reset};
    }
    template <typename S> constexpr auto setReset(const S&s) const {
        return MetaPropertyInfo<Type, NameLength, TypeLength, Getter, Setter, Member, Notify, S,
                                Flags | PropertyFlags::Resettable>
        { name, typeStr, getter, setter, member, notify, s};
    }
    template <int Flag> constexpr auto addFlag() const {
        return MetaPropertyInfo<Type, NameLength, TypeLength, Getter, Setter, Member, Notify, Reset,
                                Flags | Flag>
        { name, typeStr, getter, setter, member, notify, reset};
    }
};

/** Parse a property and fill a MetaPropertyInfo (called from W_PRPERTY macro) */
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

template<typename T, std::size_t N1, std::size_t N2, typename ... Args>
constexpr auto makeMetaPropertyInfo(StaticStringArray<N1> &name, StaticStringArray<N2> &type, Args... args) {
    MetaPropertyInfo<T, N1, N2> meta
    { {name}, {type}, {}, {}, {}, {}, {} };
    return parseProperty(meta, args...);
}

template <typename T, typename = void> struct EnumIsScoped {
    enum { Value =  std::is_convertible<T, int>::value ? 0 : 2 };
};
template <typename T> struct EnumIsScoped<QFlags<T>, void> : EnumIsScoped<typename QFlags<T>::enum_type> {};

/** Holds information about an enum */
template<std::size_t NameLength, std::size_t AliasLength, typename Values_, typename Names, int Flags>
struct MetaEnumInfo {
    StaticString<NameLength> name;
    StaticString<AliasLength> alias;
    Names names;
    using Values = Values_;
    static constexpr uint flags = Flags;
    static constexpr auto count = Values::size();
    static constexpr auto hasAlias = AliasLength > 1;
};
template<typename Enum, Enum... Value> struct enum_sequence {};
// called from W_ENUM and W_FLAG
template<typename Enum, int Flag, std::size_t NameLength, std::size_t AliasLength, Enum... Values, typename Names>
constexpr MetaEnumInfo<NameLength, AliasLength, std::index_sequence<size_t(Values)...> , Names,
                       Flag | EnumIsScoped<Enum>::Value> makeMetaEnumInfo(
                StaticStringArray<NameLength> &name, StaticString<AliasLength> alias,
                enum_sequence<Enum, Values...>, Names names) {
    return { {name}, alias, names };
}

// Helper function to remove the scope in a scoped enum name. (so "Foo::Bar" -> Bar)
template<int N> constexpr int removedScopeSize(StaticStringArray<N> &name) {
    // Find the position of the last colon (or 0 if it is not there)
    int p = N - 1;
    while (p > 0 && name[p] != ':')
        p--;
    if (name[p] == ':')
        p++;
    return N - p;
}

template<int R, int N> constexpr StaticString<R> removeScope(StaticStringArray<N> &name) {
    char result[R] = {};
    for (int i = 0; i < R; ++i) {
        result[i] = name[N - R + i];
    }
    return { result };
}

// STRing REMove SCOPE:  strignify while removing the scope
#define W_MACRO_STRREMSCOPE(...) w_internal::removeScope<w_internal::removedScopeSize("" #__VA_ARGS__)>("" #__VA_ARGS__)


/**
 * Helper for the implementation of a signal.
 * Called from the signal implementation within the W_SIGNAL macro.
 *
 * 'Func' is the type of the signal. 'Idx' is the signal index (relative).
 * It is implemented as a functor so the operator() has exactly the same amount of argument of the
 * signal so the __VA_ARGS__ works also if there is no arguments (no leading commas)
 *
 * There is specialization for const and non-const,  and for void and non-void signals.
 *
 * the last argument of the operator() is an int, to workaround the ",0" required in the W_SIGNAL
 * macro to make sure there is at least one argument for the ...
 */
template<typename Func, int Idx> struct SignalImplementation {};
template<typename Obj, typename Ret, typename... Args, int Idx>
struct SignalImplementation<Ret (Obj::*)(Args...), Idx>{
    Obj *this_;
    Ret operator()(Args... args, int) const {
        Ret r{};
        const void * a[]= { &r, (&args)... };
        QMetaObject::activate(this_, &Obj::staticMetaObject, Idx, const_cast<void **>(a));
        return r;
    }
};
template<typename Obj, typename... Args, int Idx>
struct SignalImplementation<void (Obj::*)(Args...), Idx>{
    Obj *this_;
    void operator()(Args... args, int) {
        const void *a[]= { nullptr, (&args)... };
        QMetaObject::activate(this_, &Obj::staticMetaObject, Idx, const_cast<void **>(a));
    }
};
template<typename Obj, typename Ret, typename... Args, int Idx>
struct SignalImplementation<Ret (Obj::*)(Args...) const, Idx>{
    const Obj *this_;
    Ret operator()(Args... args, int) const {
        Ret r{};
        const void * a[]= { &r, (&args)... };
        QMetaObject::activate(const_cast<Obj*>(this_), &Obj::staticMetaObject, Idx, const_cast<void **>(a));
        return r;
    }
};
template<typename Obj, typename... Args, int Idx>
struct SignalImplementation<void (Obj::*)(Args...) const, Idx>{
    const Obj *this_;
    void operator()(Args... args, int) {
        const void *a[]= { nullptr, (&args)... };
        QMetaObject::activate(const_cast<Obj*>(this_), &Obj::staticMetaObject, Idx, const_cast<void **>(a));
    }
};

/**
 * Used in the W_OBJECT macro to compute the base type.
 * Used like this:
 *  using W_BaseType = std::remove_reference_t<decltype(getParentObjectHelper(&W_ThisType::qt_metacast))>;
 * Since qt_metacast for W_ThisType will be declared later, the pointer to member function will be
 * pointing to the qt_metacast of the base class, so T will be deduced to the base class type.
 *
 * Returns a reference so this work if T is an abstract class.
 */
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
}

} // w_internal

#ifdef Q_CC_MSVC
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
#define W_PARAM_TOSTRING(...) W_MACRO_MSVC_EMPTY W_MACRO_MSVC_DELAY(W_PARAM_TOSTRING2,__VA_ARGS__ ,,,,,,,,,,,,,,,,)
#define W_PARAM_TOSTRING2(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,...) \
    w_internal::makeStaticStringList(#A1,#A2,#A3,#A4,#A5,#A6,#A7,#A8,#A9,#A10,#A11,#A12,#A13,#A14,#A15,#A16)

#define W_PARAM_TOSTRING_REMOVE_SCOPE(...) W_MACRO_MSVC_EMPTY W_MACRO_MSVC_DELAY(W_PARAM_TOSTRING2_REMOVE_SCOPE,__VA_ARGS__ ,,,,,,,,,,,,,,,,)
#define W_PARAM_TOSTRING2_REMOVE_SCOPE(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,...) \
    w_internal::makeStaticStringList(W_MACRO_STRREMSCOPE(A1), W_MACRO_STRREMSCOPE(A2), W_MACRO_STRREMSCOPE(A3), \
                                     W_MACRO_STRREMSCOPE(A4), W_MACRO_STRREMSCOPE(A5), W_MACRO_STRREMSCOPE(A6), \
                                     W_MACRO_STRREMSCOPE(A7), W_MACRO_STRREMSCOPE(A8), W_MACRO_STRREMSCOPE(A9), \
                                     W_MACRO_STRREMSCOPE(A10), W_MACRO_STRREMSCOPE(A11), W_MACRO_STRREMSCOPE(A12), \
                                     W_MACRO_STRREMSCOPE(A13), W_MACRO_STRREMSCOPE(A14), W_MACRO_STRREMSCOPE(A15), \
                                     W_MACRO_STRREMSCOPE(A16))


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
}
#define W_INTEGRAL_CONSTANT_HELPER(NAME, ...) std::integral_constant<unsigned long long, w_internal::simple_hash(#NAME #__VA_ARGS__)>
#endif

#define W_OBJECT_COMMON(TYPE) \
        using W_ThisType = TYPE; \
        static constexpr auto &W_UnscopedName = #TYPE; /* so we don't repeat it in W_CONSTRUCTOR */ \
        friend struct w_internal::FriendHelper; \
        friend constexpr w_internal::binary::tree<> w_SlotState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
        friend constexpr w_internal::binary::tree<> w_SignalState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
        friend constexpr w_internal::binary::tree<> w_MethodState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
        friend constexpr w_internal::binary::tree<> w_ConstructorState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
        friend constexpr w_internal::binary::tree<> w_PropertyState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
        friend constexpr w_internal::binary::tree<> w_EnumState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
        friend constexpr w_internal::binary::tree<> w_ClassInfoState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
        friend constexpr w_internal::binary::tree<> w_InterfaceState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
        template<typename W_Flag> static inline constexpr w_internal::StaticString<1> w_flagAlias(W_Flag) { return {""}; } \
    public: \
        struct W_MetaObjectCreatorHelper;

#if defined Q_CC_GNU && !defined Q_CC_CLANG
// workaround gcc bug  (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=69836)
#define W_STATE_APPEND(STATE, ...) \
    static constexpr int W_MACRO_CONCAT(W_WORKAROUND_, __LINE__) = \
        decltype(STATE(w_internal::w_number<>{}, static_cast<W_ThisType**>(nullptr)))::size+1; \
    friend constexpr auto STATE(w_internal::w_number<W_MACRO_CONCAT(W_WORKAROUND_, __LINE__)> w_counter, W_ThisType **w_this) \
        W_RETURN(w_internal::binary::tree_append(STATE(w_counter.prev(), w_this), __VA_ARGS__))
#else
#define W_STATE_APPEND(STATE, ...) \
    friend constexpr auto STATE(w_internal::w_number<w_internal::identity_t<decltype(STATE( \
            w_internal::w_number<>{}, static_cast<W_ThisType**>(nullptr)))>::size+1> w_counter, \
            W_ThisType **w_this) \
        W_RETURN(w_internal::binary::tree_append(STATE(w_counter.prev(), w_this), __VA_ARGS__))
#endif
#define W_STATE_APPEND_NS(STATE, ...) \
    static constexpr auto STATE(w_internal::w_number<decltype(STATE( \
            w_internal::w_number<>{}, static_cast<W_ThisType**>(nullptr)))::size+1> w_counter, \
            W_ThisType **w_this) \
        W_RETURN(w_internal::binary::tree_append(STATE(w_counter.prev(), w_this), __VA_ARGS__))

//
// public macros

/** \macro W_OBJECT(TYPE)
 * Like the Q_OBJECT macro, this declare that the object might have signals, slots or properties.
 * Must contains the class name as a parameter and need to be put before any other W_ macro in the class.
 */
#define W_OBJECT(TYPE) \
    W_OBJECT_COMMON(TYPE) \
    public: \
        using W_BaseType = std::remove_reference_t<decltype(\
            w_internal::getParentObjectHelper(&W_ThisType::qt_metacast))>; \
    Q_OBJECT \
    QT_ANNOTATE_CLASS(qt_fake, "")

/** \macro W_GADGET(TYPE)
 * Like the Q_GADGET macro, this declare that the object might have properties.
 * Must contains the class name as a parameter and need to be put before any other W_ macro in the class.
 */
#define W_GADGET(TYPE) \
    W_OBJECT_COMMON(TYPE) \
    Q_GADGET \
    QT_ANNOTATE_CLASS(qt_fake, "")

/** \macro W_NAMESPACE(NAMESPACE)
 * Like the Q_GADGET macro, this declare that the object might have properties.
 * Must contains the class name as a parameter and need to be put before any other W_ macro in the class.
 */
#define W_NAMESPACE(NAMESPACE) \
    Q_NAMESPACE \
    struct W_MetaObjectCreatorHelper; \
    struct W_ThisType { \
        using W_MetaObjectCreatorHelper = NAMESPACE::W_MetaObjectCreatorHelper; \
        static constexpr auto qt_static_metacall = nullptr; \
    }; \
    constexpr auto &W_UnscopedName = #NAMESPACE; \
    static constexpr w_internal::binary::tree<> w_SlotState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
    static constexpr w_internal::binary::tree<> w_SignalState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
    static constexpr w_internal::binary::tree<> w_MethodState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
    static constexpr w_internal::binary::tree<> w_ConstructorState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
    static constexpr w_internal::binary::tree<> w_PropertyState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
    static constexpr w_internal::binary::tree<> w_EnumState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
    static constexpr w_internal::binary::tree<> w_ClassInfoState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
    static constexpr w_internal::binary::tree<> w_InterfaceState(w_internal::w_number<0>, W_ThisType**) { return {}; } \
    template<typename W_Flag> static inline constexpr w_internal::StaticString<1> w_flagAlias(W_Flag) { return {""}; } \
    QT_ANNOTATE_CLASS(qt_fake, "")

/**
 * W_SLOT( <slot name> [, (<parameters types>) ]  [, <flags>]* )
 *
 * The W_SLOT macro needs to be put after the slot declaration.
 *
 * The W_SLOT macro can optionally have a list of parameter types as second argument to disambiguate
 * overloads or use types that are not registered with W_REGISTER_ARGTYPE. The list of parameter
 * need to be within parentheses (even if there is 0 or 1 argument).
 *
 * The W_SLOT macro can have flags:
 * - Specifying the the access:  W_Access::Protected, W_Access::Private
 *   or W_Access::Public. (By default, it is auto-detected from the location of this macro.)
 * - W_Compat: for deprecated methods (equivalent of Q_MOC_COMPAT)
 */
#define W_SLOT(...) W_MACRO_MSVC_EXPAND(W_SLOT2(__VA_ARGS__, w_internal::W_EmptyFlag))
#define W_SLOT2(NAME, ...) \
    W_STATE_APPEND(w_SlotState, w_internal::makeMetaSlotInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME,  \
            W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)(), \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), \
            W_OVERLOAD_REMOVE(__VA_ARGS__))) \
    static inline void w_GetAccessSpecifierHelper(W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)) {}

/**
 * W_INVOKABLE( <slot name> [, (<parameters types>) ]  [, <flags>]* )
 * Exactly like W_SLOT but for Q_INVOKABLE methods.
 */
#define W_INVOKABLE(...) W_MACRO_MSVC_EXPAND(W_INVOKABLE2(__VA_ARGS__, w_internal::W_EmptyFlag))
#define W_INVOKABLE2(NAME, ...) \
    W_STATE_APPEND(w_MethodState, w_internal::makeMetaMethodInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME,  \
            W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)(), \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), \
            W_OVERLOAD_REMOVE(__VA_ARGS__))) \
    static inline void w_GetAccessSpecifierHelper(W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)) {}

/**
 * <signal signature>
 * W_SIGNAL(<signal name> [, (<parameter types>) ] , <parameter names> )
 *
 * Unlike W_SLOT, W_SIGNAL must be placed directly after the signal signature declaration.
 * There should not be a semi colon between the signal signature and the macro
 *
 * Like W_SLOT, there can be the types of the parametter as a second argument, within parentheses.
 * You must then follow with the parameter names
 *
 * Note about exported classes: since the signal is inline, GCC won't export it when compiling
 * with -fvisibility-inlines-hidden (which is the default), so connecting using pointer to member
 * functions won't work accross library boundaries. You need to explicitly export the signal with
 * your MYLIB_EXPORT macro in front of the signal declaration.
 */
#define W_SIGNAL(...) W_MACRO_MSVC_EXPAND(W_SIGNAL2(__VA_ARGS__ , 0))
#define W_SIGNAL2(NAME, ...) \
    { /* W_SIGNAL need to be placed directly after the signal declaration, without semicolon. */\
        using w_SignalType = decltype(W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME)); \
        return w_internal::SignalImplementation<w_SignalType, W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__)>{this}(W_OVERLOAD_REMOVE(__VA_ARGS__)); \
    } \
    static constexpr int W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__) = \
        decltype(w_SignalState(w_internal::w_number<>{}, static_cast<W_ThisType**>(nullptr)))::size; \
    friend constexpr auto w_SignalState(w_internal::w_number<W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__) + 1> w_counter, W_ThisType **w_this) \
        W_RETURN(w_internal::binary::tree_append(w_SignalState(w_counter.prev(), w_this), \
            w_internal::makeMetaSignalInfo( \
                W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME, \
                W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)(), \
                W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), W_PARAM_TOSTRING(W_OVERLOAD_REMOVE(__VA_ARGS__))))) \
    static inline void w_GetAccessSpecifierHelper(W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)) {}

/** \macro W_SIGNAL_COMPAT
 * Same as W_SIGNAL, but set the W_Compat flag
 */
#define W_SIGNAL_COMPAT(...) W_MACRO_MSVC_EXPAND(W_SIGNAL_COMPAT2(__VA_ARGS__, 0))
#define W_SIGNAL_COMPAT2(NAME, ...) \
    { \
        using w_SignalType = decltype(W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME)); \
        return w_internal::SignalImplementation<w_SignalType, W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__)>{this}(W_OVERLOAD_REMOVE(__VA_ARGS__)); \
    } \
    static constexpr int W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__) = \
        decltype(w_SignalState(w_internal::w_number<>{}, static_cast<W_ThisType**>(nullptr)))::size; \
    friend constexpr auto w_SignalState(w_internal::w_number<W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__) + 1> w_counter, W_ThisType **w_this) \
        W_RETURN(w_internal::binary::tree_append(w_SignalState(w_counter.prev(), w_this), \
            w_internal::makeMetaSignalInfo( \
                W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME, \
                W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)(), \
                W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), W_PARAM_TOSTRING(W_OVERLOAD_REMOVE(__VA_ARGS__)), W_Compat))) \
    static inline void w_GetAccessSpecifierHelper(W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)) {}

/** W_CONSTRUCTOR(<parameter types>)
 * Declares that this class can be constructed with this list of argument.
 * Equivalent to Q_INVOKABLE constructor.
 * One can have W_CONSTRUCTOR() for the default constructor even if it is implicit.
 */
#define W_CONSTRUCTOR(...) \
    W_STATE_APPEND(w_ConstructorState, \
                   w_internal::makeMetaConstructorInfo<__VA_ARGS__>().setName(W_UnscopedName))


/** W_PROPERTY(<type>, <name> [, <flags>]*)
 *
 * Declare a property <name> with the type <type>.
 * The flags can be function pointers that are detected to be setter, getters, notify signal or
 * other flags. Use the macro READ, WRITE, MEMBER, ... for the flag so you can write W_PROPERTY
 * just like in a Q_PROPERTY. The only difference with Q_PROPERTY would be the semicolon before the
 * name.
 * W_PROPERTY need to be put after all the setters, getters, signals and members have been declared.
 *
 * <type> can optionally be put in parentheses, if you have a type containing a comma
 */
#define W_PROPERTY(...) W_MACRO_MSVC_EXPAND(W_PROPERTY2(__VA_ARGS__)) // expands the READ, WRITE, and other sub marcos
#define W_PROPERTY2(TYPE, NAME, ...) \
    W_STATE_APPEND(w_PropertyState, \
        w_internal::makeMetaPropertyInfo<W_MACRO_REMOVEPAREN(TYPE)>(\
                            #NAME, W_MACRO_STRIGNIFY(W_MACRO_REMOVEPAREN(TYPE)), __VA_ARGS__))

#define WRITE , &W_ThisType::
#define READ , &W_ThisType::
#define NOTIFY , W_Notify, &W_ThisType::
#define RESET , W_Reset, &W_ThisType::
#define MEMBER , &W_ThisType::
#define CONSTANT , W_Constant
#define FINAL , W_Final

#undef Q_PRIVATE_PROPERTY // the official macro is not a variadic macro, and the coma in READ would break it
#define Q_PRIVATE_PROPERTY(...)

/** W_ENUM(<name>, <values>)
 * Similar to Q_ENUM, but one must also manually write all the values.
 */
#define W_ENUM(NAME, ...) \
    W_STATE_APPEND(w_EnumState, w_internal::makeMetaEnumInfo<NAME,false>( \
            #NAME, w_flagAlias(NAME{}), \
            w_internal::enum_sequence<NAME,__VA_ARGS__>{}, W_PARAM_TOSTRING_REMOVE_SCOPE(__VA_ARGS__))) \
    Q_ENUM(NAME)


/** W_ENUM_NS(<name>, <values>)
 * Similar to Q_ENUM_NS, like W_ENUM
 */
#define W_ENUM_NS(NAME, ...) \
    W_STATE_APPEND_NS(w_EnumState, w_internal::makeMetaEnumInfo<NAME,false>( \
            #NAME, w_flagAlias(NAME{}), \
            w_internal::enum_sequence<NAME,__VA_ARGS__>{}, W_PARAM_TOSTRING_REMOVE_SCOPE(__VA_ARGS__))) \
    Q_ENUM_NS(NAME)

/** W_FLAG(<name>, <values>)
 * Similar to Q_FLAG, but one must also manually write all the values.
 */
namespace w_internal {
template<typename T> struct QEnumOrQFlags { using Type = T; };
template<typename T> struct QEnumOrQFlags<QFlags<T>> { using Type = T; };
}
#define W_FLAG(NAME, ...) \
    W_STATE_APPEND(w_EnumState, w_internal::makeMetaEnumInfo<w_internal::QEnumOrQFlags<NAME>::Type,true>( \
            #NAME, w_flagAlias(NAME{}), \
            w_internal::enum_sequence<w_internal::QEnumOrQFlags<NAME>::Type,__VA_ARGS__>{}, \
            W_PARAM_TOSTRING_REMOVE_SCOPE(__VA_ARGS__))) \
    Q_FLAG(NAME)

/** W_FLAG_NS(<name>, <values>)
 * Similar to Q_FLAG_NS, like W_FLAG.
 */
#define W_FLAG_NS(NAME, ...) \
    W_STATE_APPEND_NS(w_EnumState, w_internal::makeMetaEnumInfo<w_internal::QEnumOrQFlags<NAME>::Type,true>( \
            #NAME, w_flagAlias(NAME{}), \
            w_internal::enum_sequence<w_internal::QEnumOrQFlags<NAME>::Type,__VA_ARGS__>{}, \
            W_PARAM_TOSTRING_REMOVE_SCOPE(__VA_ARGS__))) \
    Q_FLAG_NS(NAME)

/** Same as Q_CLASSINFO.  Note, Q_CLASSINFO_NS is required for namespace */
#define W_CLASSINFO(A, B) \
    W_STATE_APPEND(w_ClassInfoState, \
        std::pair<w_internal::StaticString<sizeof(A)>, w_internal::StaticString<sizeof(B)>>{ {A}, {B} })

/** Same as Q_CLASSINFO, but within a namespace */
#define W_CLASSINFO_NS(A, B) \
    W_STATE_APPEND_NS(w_ClassInfoState, \
        std::pair<w_internal::StaticString<sizeof(A)>, w_internal::StaticString<sizeof(B)>>{ {A}, {B} })


/** Same as Q_INTERFACE */
#define W_INTERFACE(A) \
    W_STATE_APPEND(w_InterfaceState, static_cast<A*>(nullptr))

/** Same as Q_DECLARE_FLAGS */
#define W_DECLARE_FLAGS(Flags, Enum) \
    Q_DECLARE_FLAGS(Flags, Enum) \
    static inline constexpr w_internal::StaticString<sizeof(#Enum)> w_flagAlias(Flags) { return {#Enum}; }

/** \macro W_REGISTER_ARGTYPE(TYPE)
 Registers TYPE so it can be used as a parameter of a signal/slot or return value.
 The normalized signature must be used.
 Note: This does not imply Q_DECLARE_METATYPE, and Q_DECLARE_METATYPE does not imply this
*/
namespace w_internal { template<typename T> struct W_TypeRegistery { enum { registered = false }; }; }
#define W_REGISTER_ARGTYPE(...) namespace w_internal { \
    template<> struct W_TypeRegistery<__VA_ARGS__> { \
    enum { registered = true }; \
    static constexpr auto name = makeStaticString(#__VA_ARGS__); \
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
