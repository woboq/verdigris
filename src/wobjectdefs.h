#pragma once

#include <QtCore/qobjectdefs.h>
#include <QtCore/qmetatype.h>
#include <utility>

namespace simple {
    // FIXME default std::make_index_sequence is recursing O(N) times which is too big for big strings
    using std::index_sequence;
    template<size_t... I1, size_t... I2>
    index_sequence<I1... , (I2 +(sizeof...(I1)))...>
    make_index_sequence_helper_merge(index_sequence<I1...>, index_sequence<I2...>);

    template<int I> struct make_index_sequence_helper {
        using part1 = typename make_index_sequence_helper<(I+1)/2>::result;
        using part2 = typename make_index_sequence_helper<I/2>::result;
        using result = decltype(make_index_sequence_helper_merge(part1(), part2()));
    };
    template<> struct make_index_sequence_helper<1> { using result = index_sequence<0>; };
    template<> struct make_index_sequence_helper<0> { using result = index_sequence<>; };
    template<int I> using make_index_sequence = typename make_index_sequence_helper<I>::result;
}



namespace binary {

    template <typename T> struct Leaf {
        T data;
        static constexpr int Depth = 0;
        static constexpr int Count = 1;
        static constexpr bool Balanced = true;
    };

    template <class A, class B> struct Node {
        A a;
        B b;
        static constexpr int Count = A::Count + B::Count;
        static constexpr int Depth = A::Depth + 1;
        static constexpr bool Balanced = A::Depth == B::Depth && B::Balanced;
    };

    /* Add the node 'N' to thre tree 'T' */
    template <class T, typename N, bool Balanced = T::Balanced > struct Add {
        typedef Node<T, Leaf<N> > Result;
        static constexpr Result add(T t, N n) { return {t, {n} }; }
    };
    template <class A, class B, typename N> struct Add<Node<A, B>, N, false> {
        typedef Node<A, typename Add<B, N>::Result > Result;
        static constexpr Result add(Node<A, B> t, N n) { return {t.a , Add<B, N>::add(t.b, n) }; }
    };

    template <class T, typename N, bool Balanced = T::Balanced > struct AddPre {
        typedef Node<Leaf<N> , T > Result;
        static constexpr Result add(T t, N n) { return {{n}, t}; }
    };
    template <class A, class B, typename N> struct AddPre<Node<A, B>, N, false> {
        typedef Node<typename AddPre<A, N>::Result, B > Result;
        static constexpr Result add(Node<A, B> t, N n) { return {AddPre<A, N>::add(t.a, n) , t.b }; }
    };


    template <class T, int I, typename = void> struct Get;
    template <class N> struct Get<Leaf<N>, 0> {
        static constexpr N get(Leaf<N> t) { return t.data; }
    };
    template <class A, class B, int I> struct Get<Node<A,B>, I, std::enable_if_t<(A::Count <= I)>> {
        static constexpr auto get(Node<A,B> t) { return Get<B,I - A::Count>::get(t.b); }
    };
    template <class A, class B, int I> struct Get<Node<A,B>, I, std::enable_if_t<(A::Count > I)>> {
        static constexpr auto get(Node<A,B> t) { return Get<A,I>::get(t.a); }
    };

    template<typename A, typename B> struct Tail;
    template<typename A, typename B> struct Tail<Leaf<A>, B> {
        using Result = B;
        static constexpr B tail(Node<Leaf<A>,B> t) { return t.b; }
    };
    template<typename A, typename B, typename C> struct Tail<Node<A,B>, C> {
        using Result = Node<typename Tail<A,B>::Result, C>;
        static constexpr Result tail(Node<Node<A,B>, C> t) { return { Tail<A,B>::tail(t.a) , t.b }; }
    };

    template<typename T = void> struct tree {
        static constexpr int size = T::Count;
        T root;
    };
    template<> struct tree<> { static constexpr int size = 0; };

    template<typename T> struct tree_size { static constexpr int value = T::size; };
    //template<typename... Ts> tuple<Ts...> make_tuple(Ts... ts) { return {ts...}; }

    template<typename N>
    constexpr tree<Leaf<N>> tree_append(tree<>, N n)
    { return {{n}}; }
    template<typename Root, typename N>
    constexpr tree<typename Add<Root,N>::Result> tree_append(tree<Root> t, N n)
    { return {Add<Root,N>::add(t.root,n)}; }

    template<typename N>
    constexpr tree<Leaf<N>> tree_prepend(tree<>, N n)
    { return {{n}}; }
    template<typename Root, typename N>
    constexpr tree<typename AddPre<Root,N>::Result> tree_prepend(tree<Root> t, N n)
    { return {AddPre<Root,N>::add(t.root,n)}; }


    template<int N, typename Root> constexpr auto get(tree<Root> t)
    { return Get<Root, N>::get(t.root); }


    /**
     *  tree_tail()  Returns a tuple with the first element removed
     */
    template<typename A, typename B>
    constexpr tree<typename Tail<A,B>::Result> tree_tail(tree<Node<A, B>> t)
    { return { Tail<A,B>::tail(t.root) }; }
    template<typename N>
    constexpr tree<> tree_tail(tree<Leaf<N>>) { return {}; }
    constexpr tree<> tree_tail(tree<>) { return {}; }

    /**
     * tree_head()  same as get<O> but return something in case the tuple is too small
     */
    template<typename T> constexpr auto tree_head(tree<T> t) { return get<0>(t); }
    constexpr auto tree_head(tree<void>) { struct _{}; return _{}; }
    template<typename T> constexpr auto tree_head(T) { struct _{}; return _{}; }

    template<int I, typename T> using tree_element_t = decltype(get<I>(std::declval<T>()));

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
}





// Qt should have that https://codereview.qt-project.org/139583
inline namespace ShouldBeInQt {

template <typename... Args>
struct QNonConstOverload
{
    template <typename R, typename T>
    Q_DECL_CONSTEXPR auto operator()(R (T::*ptr)(Args...)) const
    { return ptr; }
};

template <typename... Args>
struct QConstOverload
{
    template <typename R, typename T>
    Q_DECL_CONSTEXPR auto operator()(R (T::*ptr)(Args...) const) const
    { return ptr; }
};

template <typename... Args>
struct QOverload : QConstOverload<Args...>, QNonConstOverload<Args...>
{
    using QConstOverload<Args...>::operator();
    using QNonConstOverload<Args...>::operator();

    template <typename R>
    Q_DECL_CONSTEXPR auto operator()(R (*ptr)(Args...)) const
    { return ptr; }
};

template <typename... Args> QOverload<Args...> qOverload = {};
template <typename... Args> QConstOverload<Args...> qConstOverload = {};
template <typename... Args> QNonConstOverload<Args...> qNonConstOverload = {};

}

/*-----------------------------------------------------------------------------------------------*/
/* Helpers to play with tuple or strings at compile time                                         */
/*-----------------------------------------------------------------------------------------------*/

template<std::size_t... I, std::size_t...J>
constexpr std::index_sequence<I...,J...> operator+(std::index_sequence<I...>,std::index_sequence<J...>)
{ return {}; }


/**
 * ones()
 * return an index_sequence with N ones
 **/
template<typename> struct ones_helper;
template<std::size_t...I> struct ones_helper<std::index_sequence<I...>>
{ using result = std::index_sequence<(void(I),1)...>; };
template<int N> using ones = typename ones_helper<simple::make_index_sequence<N>>::result;

/* Compute the sum of many integers */
constexpr int sums() { return 0; }
template<typename... Args>
constexpr int sums(int i, Args... args) { return i + sums(args...);  }


/*
 * Helpers to play with static strings
 */

/** A compile time character array of size N  */
template<int N> using StaticStringArray = const char [N];

/** Represents a string of size N  (N includes the 0 at the end) */
template<int N> struct StaticString  {
    StaticStringArray<N> data;
    template <std::size_t... I>
    constexpr StaticString(StaticStringArray<N> &d, std::index_sequence<I...>) : data{ (d[I])... } { }
    constexpr StaticString(StaticStringArray<N> &d) : StaticString(d, simple::make_index_sequence<N>()) {}
    static constexpr int size = N;
    constexpr char operator[](int p) const { return data[p]; }
};
template <int N> constexpr StaticString<N> makeStaticString(StaticStringArray<N> &d) { return {d}; }

/* A tuple containing many  StaticString with possibly different sizes */
template<typename T = void> using StaticStringList = binary::tree<T>;


constexpr binary::tree<> makeParamStringList() { return {}; }
template<typename... T>
constexpr binary::tree<> makeParamStringList(StaticStringArray<1> &, T...)
{ return {}; }
template<int N, typename... T>
constexpr auto makeParamStringList(StaticStringArray<N> &h, T&...t)
{ return binary::tree_prepend(makeParamStringList(t...), StaticString<N>(h)); }

/** Add a string in a StaticStringList */
template<int L, typename T>
constexpr auto addString(const StaticStringList<T> &l, const StaticString<L> & s) {
    return binary::tree_append(l, s);
}


/*-----------*/

template<int N = 255> struct w_number : public w_number<N - 1> {
    static constexpr int value = N;
    static constexpr w_number<N-1> prev() { return {}; }
};
template<> struct w_number<0> { static constexpr int value = 0; };

template <int N> struct W_MethodFlags { static constexpr int value = N; };


// Mirror of QMetaMethod::Access
namespace W_Access {
/* // From qmetaobject_p.h MethodFlags
    AccessPrivate = 0x00,
    AccessProtected = 0x01,
    AccessPublic = 0x02,
    AccessMask = 0x03, //mask
 */
    constexpr W_MethodFlags<0x00> Public{};
    constexpr W_MethodFlags<0x01> Protected{};
    constexpr W_MethodFlags<0x02> Private{}; // Note: Public and Private are reversed so Public can be the default
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
    constexpr W_MethodFlags<0x00> Method{};
    constexpr W_MethodFlags<0x04> Signal{};
    constexpr W_MethodFlags<0x08> Slot{};
    constexpr W_MethodFlags<0x0c> Constructor{};
}

/*
MethodCompatibility = 0x10,
MethodCloned = 0x20,
MethodScriptable = 0x40,
MethodRevisioned = 0x80
*/

constexpr W_MethodFlags<0x10> W_Compat{};
constexpr W_MethodFlags<0x40> W_Scriptable{};


namespace MetaObjectBuilder {
// From qmetaobject_p.h
enum PropertyFlags  {
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
}

constexpr struct {} W_Notify{};
constexpr struct {} W_Reset{};
constexpr std::integral_constant<int, MetaObjectBuilder::Constant> W_Constant{};
constexpr std::integral_constant<int, MetaObjectBuilder::Final> W_Final{};


// workaround to avoid leading coma in macro that can optionaly take a flag
struct W_RemoveLeadingComa { constexpr W_MethodFlags<0> operator+() const { return {}; } };
template <typename T> constexpr T operator+(T &&t, W_RemoveLeadingComa) { return t; }
constexpr W_RemoveLeadingComa W_removeLeadingComa{};

template<typename T> struct W_TypeRegistery { enum { registered = false }; };
#define W_DECLARE_METATYPE(T) template<> struct W_TypeRegistery<T> { \
    enum { registered = true }; \
    static constexpr auto name = makeStaticString(#T); \
};

W_DECLARE_METATYPE(char*)
W_DECLARE_METATYPE(const char*)

/*-----------------------------------------------------------------------------------------------*/
/* The code that generates the QMetaObject                                                          */
/*-----------------------------------------------------------------------------------------------*/
namespace MetaObjectBuilder {

    /** Holds information about a method */
    template<typename F, int NameLength, int Flags, typename ParamTypes, typename ParamNames = StaticStringList<>>
    struct MetaMethodInfo {
        F func;
        StaticString<NameLength> name;
        ParamTypes paramTypes;
        ParamNames paramNames;
        static constexpr int argCount = QtPrivate::FunctionPointer<F>::ArgumentCount;
        static constexpr int flags = Flags;
    };

    template<typename F, int N, typename ParamTypes, int... Flags>
    constexpr MetaMethodInfo<F, N, sums(Flags...) | W_MethodType::Slot.value, ParamTypes>
    makeMetaSlotInfo(F f, StaticStringArray<N> &name, const ParamTypes &paramTypes, W_MethodFlags<Flags>...)
    { return { f, {name}, paramTypes, {} }; }

    template<typename F, int N, typename ParamTypes, int... Flags>
    constexpr MetaMethodInfo<F, N, sums(Flags...) | W_MethodType::Method.value, ParamTypes>
    makeMetaMethodInfo(F f, StaticStringArray<N> &name, const ParamTypes &paramTypes, W_MethodFlags<Flags>...)
    { return { f, {name}, paramTypes, {} }; }

    template<typename F, int N, typename ParamTypes, typename ParamNames, int... Flags>
    constexpr MetaMethodInfo<F, N, sums(Flags...) | W_MethodType::Signal.value | W_Access::Public.value,
                             ParamTypes, ParamNames>
    makeMetaSignalInfo(F f, StaticStringArray<N> &name, const ParamTypes &paramTypes,
                       const ParamNames &paramNames, W_MethodFlags<Flags>...)
    { return { f, {name}, paramTypes, paramNames }; }


    template<int NameLength, typename... Args> struct MetaConstructorInfo {
        static constexpr int argCount = sizeof...(Args);
        static constexpr int flags = W_MethodType::Constructor.value | W_Access::Public.value;
        StaticString<NameLength> name;
        template<int N>
        constexpr MetaConstructorInfo<N, Args...> setName(StaticStringArray<N> &name)
        { return { { name } }; }
        template<typename T, std::size_t... I>
        void createInstance(void **_a, std::index_sequence<I...>) const {
            *reinterpret_cast<T**>(_a[0]) =
                new T(*reinterpret_cast<typename std::remove_reference<Args>::type *>(_a[I+1])...);
        }
    };
    template<typename...  Args> constexpr MetaConstructorInfo<1,Args...> makeMetaConstructorInfo()
    { return { {""} }; }

    /** Holds information about a property */
    template<typename Type, int NameLength, int TypeLength, typename Getter = std::nullptr_t,
             typename Setter = std::nullptr_t, typename Member = std::nullptr_t,
             typename Notify = std::nullptr_t, typename Reset = std::nullptr_t, int Flags = 0>
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

    /** Parse a property and fill a MetaPropertyInfo */
    // base case
    template <typename PropInfo> constexpr auto parseProperty(const PropInfo &p) { return p; }
    // setter
    template <typename PropInfo, typename Obj, typename Arg, typename Ret, typename... Tail>
    constexpr auto parseProperty(const PropInfo &p, Ret (Obj::*s)(Arg), Tail... t)
    { return parseProperty(p.setSetter(s) , t...); }
    // getter
    template <typename PropInfo, typename Obj, typename Ret, typename... Tail>
    constexpr auto parseProperty(const PropInfo &p, Ret (Obj::*s)(), Tail... t)
    { return parseProperty(p.setGetter(s), t...); }
    template <typename PropInfo, typename Obj, typename Ret, typename... Tail>
    constexpr auto parseProperty(const PropInfo &p, Ret (Obj::*s)() const, Tail... t)
    { return parseProperty(p.setGetter(s), t...); }
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
    // other flags flags
    template <typename PropInfo, int Flag, typename... Tail>
    constexpr auto parseProperty(const PropInfo &p, std::integral_constant<int, Flag>, Tail... t)
    { return parseProperty(p.template addFlag<Flag>() ,t...); }

    template<typename T, int N1, int N2, typename ... Args>
    constexpr auto makeMetaPropertyInfo(StaticStringArray<N1> &name, StaticStringArray<N2> &type, Args... args) {
        MetaPropertyInfo<T, N1, N2> meta
        { {name}, {type}, {}, {}, {}, {}, {} };
        return parseProperty(meta, args...);
    }

    template<int NameLength, typename Values_, typename Names, int Flags>
    struct MetaEnumInfo {
        StaticString<NameLength> name;
        Names names;
        using Values = Values_;
        static constexpr uint flags = Flags;
        static constexpr uint count = Values::size();
    };

    template<typename T, int Flag, int NameLength, T... Values, typename Names>
    constexpr MetaEnumInfo<NameLength, std::index_sequence<size_t(Values)...> , Names, Flag> makeMetaEnumInfo(
                    StaticStringArray<NameLength> &name, std::integer_sequence<T, Values...>, Names names) {
        return { {name}, names };
    }

struct FriendHelper1;
}

template<typename Func, int Idx> struct SignalImplementation {};
template<typename Ret, typename Obj, typename... Args, int Idx>
struct SignalImplementation<Ret (Obj::*)(Args...), Idx>{
    Obj *this_;
    Ret operator()(Args... args) const {
        Ret r{};
        const void * a[]= { &r, (&args)... };
        QMetaObject::activate(this_, &Obj::staticMetaObject, Idx, const_cast<void **>(a));
        return r;
    }
};
template<typename Obj, typename... Args, int Idx>
struct SignalImplementation<void (Obj::*)(Args...), Idx>{
    Obj *this_;
    void operator()(Args... args) {
        const void *a[]= { nullptr, (&args)... };
        QMetaObject::activate(this_, &Obj::staticMetaObject, Idx, const_cast<void **>(a));
    }
};
template<typename Ret, typename Obj, typename... Args, int Idx>
struct SignalImplementation<Ret (Obj::*)(Args...) const, Idx>{
    const Obj *this_;
    Ret operator()(Args... args) const {
        Ret r{};
        const void * a[]= { &r, (&args)... };
        QMetaObject::activate(const_cast<Obj*>(this_), &Obj::staticMetaObject, Idx, const_cast<void **>(a));
        return r;
    }
};
template<typename Obj, typename... Args, int Idx>
struct SignalImplementation<void (Obj::*)(Args...) const, Idx>{
    const Obj *this_;
    void operator()(Args... args) {
        const void *a[]= { nullptr, (&args)... };
        QMetaObject::activate(const_cast<Obj*>(this_), &Obj::staticMetaObject, Idx, const_cast<void **>(a));
    }
};



template<typename T> T getParentObjectHelper(void* (T::*)(const char*));

struct FriendHelper2;

// private macro helpers

// strignify and make a StaticStringList out of an array of arguments
#define W_PARAM_TOSTRING(...) W_PARAM_TOSTRING2(__VA_ARGS__ ,,,,,,,,,,,,,,,,)
#define W_PARAM_TOSTRING2(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,...) \
    makeParamStringList(#A1,#A2,#A3,#A4,#A5,#A6,#A7,#A8,#A9,#A10,#A11,#A12,#A13,#A14,#A15,#A16)

#define W_MACRO_EMPTY
#define W_MACRO_EVAL(...) __VA_ARGS__
#define W_MACRO_DELAY(X,...) X(__VA_ARGS__)
#define W_MACRO_DELAY2(X,...) X(__VA_ARGS__)
#define W_MACRO_TAIL(A, ...) __VA_ARGS__
#define W_MACRO_STRIGNIFY(...) W_MACRO_STRIGNIFY2(__VA_ARGS__)
#define W_MACRO_STRIGNIFY2(...) #__VA_ARGS__
#define W_MACRO_CONCAT(A, B) W_MACRO_CONCAT2(A,B)
#define W_MACRO_CONCAT2(A, B) A##_##_##B

// remove the surrounding parentheses
#define W_MACRO_REMOVEPAREN(A) W_MACRO_DELAY(W_MACRO_REMOVEPAREN2, W_MACRO_REMOVEPAREN_HELPER A)
#define W_MACRO_REMOVEPAREN2(...) W_MACRO_DELAY2(W_MACRO_TAIL, W_MACRO_REMOVEPAREN_HELPER_##__VA_ARGS__)
#define W_MACRO_REMOVEPAREN_HELPER(...) _ , __VA_ARGS__
#define W_MACRO_REMOVEPAREN_HELPER_W_MACRO_REMOVEPAREN_HELPER ,


// if __VA_ARGS__ is "(types), foobar"   then return just the types, otherwise return nothing
#define W_OVERLOAD_TYPES(A, ...) W_MACRO_DELAY(W_MACRO_TAIL,W_OVERLOAD_TYPES_HELPER A)
#define W_OVERLOAD_TYPES_HELPER(...) , __VA_ARGS__

#define W_OVERLOAD_RESOLVE(A, ...) W_MACRO_DELAY(W_MACRO_TAIL,W_OVERLOAD_RESOLVE_HELPER A)
#define W_OVERLOAD_RESOLVE_HELPER(...) , qOverload<__VA_ARGS__>


// remove the first argument if it is in parentheses"
#define W_OVERLOAD_REMOVE(...) W_MACRO_DELAY(W_OVERLOAD_REMOVE2, W_OVERLOAD_REMOVE_HELPER __VA_ARGS__)
#define W_OVERLOAD_REMOVE2(...) W_MACRO_DELAY2(W_MACRO_TAIL, W_OVERLOAD_REMOVE_HELPER_##__VA_ARGS__)

#define W_OVERLOAD_REMOVE_HELPER(...) _
#define W_OVERLOAD_REMOVE_HELPER_W_OVERLOAD_REMOVE_HELPER ,


#define W_RETURN(R) -> decltype(R) { return R; }

#define W_OBJECT_COMMON(TYPE) \
        using W_ThisType = TYPE; /* This is the only reason why we need TYPE */ \
        static constexpr auto &W_UnscopedName = #TYPE; /* so we don't repeat it in W_CONSTRUCTOR */ \
        friend struct MetaObjectBuilder::FriendHelper1; \
        friend struct ::FriendHelper2; \
        static constexpr binary::tree<> w_SlotState(w_number<0>) { return {}; } \
        static constexpr binary::tree<> w_SignalState(w_number<0>) { return {}; } \
        static constexpr binary::tree<> w_MethodState(w_number<0>) { return {}; } \
        static constexpr binary::tree<> w_ConstructorState(w_number<0>) { return {}; } \
        static constexpr binary::tree<> w_PropertyState(w_number<0>) { return {}; } \
        static constexpr binary::tree<> w_EnumState(w_number<0>) { return {}; } \
        static constexpr binary::tree<> w_ClassInfoState(w_number<0>) { return {}; } \
    public: \
        struct MetaObjectCreatorHelper;


// public macros
#define W_OBJECT(TYPE) \
    W_OBJECT_COMMON(TYPE) \
    public: \
        using W_BaseType = decltype(getParentObjectHelper(&W_ThisType::qt_metacast)); \
    Q_OBJECT

#define W_GADGET(TYPE) \
    W_OBJECT_COMMON(TYPE) \
    Q_GADGET


#define W_SLOT(NAME, ...) \
    static constexpr auto w_SlotState(w_number<binary::tree_size<decltype(w_SlotState(w_number<>{}))>::value+1> counter) \
        W_RETURN(binary::tree_append(w_SlotState(counter.prev()), MetaObjectBuilder::makeMetaSlotInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME,  \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), \
            W_OVERLOAD_REMOVE(__VA_ARGS__) +W_removeLeadingComa)))

#define W_INVOKABLE(NAME, ...) \
    static constexpr auto w_MethodState(w_number<binary::tree_size<decltype(w_MethodState(w_number<>{}))>::value+1> counter) \
        W_RETURN(binary::tree_append(w_MethodState(counter.prev()), MetaObjectBuilder::makeMetaMethodInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME,  \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), \
            W_OVERLOAD_REMOVE(__VA_ARGS__) +W_removeLeadingComa)))

#define W_SIGNAL_1(...) \
    __VA_ARGS__
#define W_SIGNAL_2(NAME, ...) \
    { \
        using w_SignalType = decltype(W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME)); \
        return SignalImplementation<w_SignalType, W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__)>{this}(W_OVERLOAD_REMOVE(__VA_ARGS__)); \
    } \
    static constexpr int W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__) = binary::tree_size<decltype(w_SignalState(w_number<>{}))>::value; \
    static constexpr auto w_SignalState(w_number<W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__) + 1> counter) \
        W_RETURN(binary::tree_append(w_SignalState(counter.prev()), MetaObjectBuilder::makeMetaSignalInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME, \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), W_PARAM_TOSTRING(W_OVERLOAD_REMOVE(__VA_ARGS__)))))

// Same as W_SIGNAL_2, but set the W_Compat flag
#define W_SIGNAL_2_COMPAT(NAME, ...) \
    { \
        using w_SignalType = decltype(W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME)); \
        return SignalImplementation<w_SignalType, W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__)>{this}(W_OVERLOAD_REMOVE(__VA_ARGS__)); \
    } \
    static constexpr int W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__) = binary::tree_size<decltype(w_SignalState(w_number<>{}))>::value; \
    static constexpr auto w_SignalState(w_number<W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__) + 1> counter) \
        W_RETURN(binary::tree_append(w_SignalState(counter.prev()), MetaObjectBuilder::makeMetaSignalInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME, \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), W_PARAM_TOSTRING(W_OVERLOAD_REMOVE(__VA_ARGS__)), W_Compat)))


#define W_CONSTRUCTOR(...) \
    static constexpr auto w_ConstructorState(w_number<binary::tree_size<decltype(w_ConstructorState(w_number<>{}))>::value+1> counter) \
        W_RETURN(binary::tree_append(w_ConstructorState(counter.prev()), \
            MetaObjectBuilder::makeMetaConstructorInfo<__VA_ARGS__>().setName(W_UnscopedName)))

#define W_PROPERTY(...) W_PROPERTY2(__VA_ARGS__) // expands the READ, WRITE, and other sub marcos
#define W_PROPERTY2(TYPE, NAME, ...) \
    static constexpr auto w_PropertyState(w_number<binary::tree_size<decltype(w_PropertyState(w_number<>{}))>::value+1> counter) \
        W_RETURN(binary::tree_append(w_PropertyState(counter.prev()), \
                              MetaObjectBuilder::makeMetaPropertyInfo<W_MACRO_REMOVEPAREN(TYPE)>(\
                                    #NAME, W_MACRO_STRIGNIFY(W_MACRO_REMOVEPAREN(TYPE)), __VA_ARGS__)))

#define W_ENUM(NAME, ...) \
    static constexpr auto w_EnumState(w_number<binary::tree_size<decltype(w_EnumState(w_number<>{}))>::value+1> counter) \
        W_RETURN(binary::tree_append(w_EnumState(counter.prev()), MetaObjectBuilder::makeMetaEnumInfo<NAME,false>( \
            #NAME, std::integer_sequence<NAME,__VA_ARGS__>{}, W_PARAM_TOSTRING(__VA_ARGS__)))) \
    Q_ENUM(NAME)

template<typename T> struct QEnumOrQFlags { using Type = T; };
template<typename T> struct QEnumOrQFlags<QFlags<T>> { using Type = T; };

#define W_FLAG(NAME, ...) \
    static constexpr auto w_EnumState(w_number<binary::tree_size<decltype(w_EnumState(w_number<>{}))>::value+1> counter) \
        W_RETURN(binary::tree_append(w_EnumState(counter.prev()), MetaObjectBuilder::makeMetaEnumInfo<QEnumOrQFlags<NAME>::Type,true>( \
            #NAME, std::integer_sequence<QEnumOrQFlags<NAME>::Type,__VA_ARGS__>{}, W_PARAM_TOSTRING(__VA_ARGS__)))) \
    Q_FLAG(NAME)

#define W_CLASSINFO(A, B) \
    static constexpr auto w_ClassInfoState(w_number<binary::tree_size<decltype(w_ClassInfoState(w_number<>{}))>::value+1> counter) \
        W_RETURN(binary::tree_append(w_ClassInfoState(counter.prev()), \
            std::pair<StaticString<sizeof(A)>, StaticString<sizeof(B)>>{ {A}, {B} }))

#define WRITE , &W_ThisType::
#define READ , &W_ThisType::
#define NOTIFY , W_Notify, &W_ThisType::
#define RESET , W_Reset, &W_ThisType::
#define MEMBER , &W_ThisType::
#define CONSTANT , W_Constant
#define FINAL , W_Final