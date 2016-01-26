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


    // std::tuple compilation time is too slow for use in constexpr

    template<typename ... Ts> struct tuple;
    template<typename T, typename ... Ts> struct tuple<T, Ts...> : tuple<Ts...> {
        static constexpr int size = 1 + sizeof...(Ts);
        constexpr tuple(T t, Ts... ts) : tuple<Ts...>{ts...} , data{t} {};
        constexpr tuple() : data{} {};
        T data;
        constexpr T get_helper(std::integral_constant<int, sizeof...(Ts)>) { return data; }
        using tuple<Ts...>::get_helper;
    };
    template<> struct tuple<> {
        static constexpr int size = 0;
        void get_helper() { }

    };

    template<typename T> struct tuple_size { static constexpr int value = T::size; };

    template<int N, typename...Ts> constexpr auto get(tuple<Ts...> t)
    { return t.get_helper(std::integral_constant<int, sizeof...(Ts) - N - 1>{}); }

    template<typename... Ts> constexpr tuple<Ts...> make_tuple(Ts... ts) { return {ts...}; }

    template<typename... T1, typename... T2, std::size_t... I1, std::size_t... I2>
    constexpr tuple<T1..., T2...> tuple_cat_helper(tuple<T1...> t1, tuple<T2...> t2,
                                    std::index_sequence<I1...>, std::index_sequence<I2...>) {
        Q_UNUSED(t1) Q_UNUSED(t2)
        return { get<I1>(t1)..., get<I2>(t2)... };
    }


    template<typename... T1, typename... T2>
    constexpr tuple<T1..., T2...> tuple_cat(tuple<T1...> t1, tuple<T2...> t2) {
        return tuple_cat_helper(t1, t2, simple::make_index_sequence<sizeof...(T1)>{},
                                simple::make_index_sequence<sizeof...(T2)>{});
    }
    template<typename... T1, typename... T2, typename... T>
    constexpr auto tuple_cat(tuple<T1...> t1, tuple<T2...> t2, T... t)
    { return tuple_cat(t1, tuple_cat(t2, t...)); }

    template<int I, typename T> using tuple_element_t = decltype(get<I>(std::declval<T>()));

    /**
     * tuple_head()  same as get<O> but return something in case the tuple is too small
     */
    template<typename T, typename... Ts>
    constexpr T tuple_head(const simple::tuple<T, Ts...> &t)
    { return t.data; }
    constexpr auto tuple_head(const simple::tuple<> &) {
        struct _{};
        return _{};
    }
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
 *  tuple_tail()  Returns a tuple with the first element removed
 */
template<typename T, std::size_t...I> constexpr auto tuple_tail_helper(const T&t , std::index_sequence<I...>) {
    return simple::make_tuple(simple::get<I+1>(t)...);
}
template<typename T> constexpr auto tuple_tail(const T& tuple) {
    return tuple_tail_helper(tuple, simple::make_index_sequence<simple::tuple_size<T>::value-1>());
}
constexpr auto tuple_tail(simple::tuple<>)
{ return simple::tuple<>{}; }


/**
 * tuple_append() Appends one element to the tuple (faster than tuple_cat)
 */
template<typename... Ts, typename T, std::size_t...I>
constexpr auto tuple_append_helper(const simple::tuple<Ts...> &tuple, const T &t, std::index_sequence<I...>) {
    return simple::tuple<Ts..., T>{simple::get<I>(tuple)... , t};
}
template<typename T1, typename T>
constexpr auto tuple_append(const T1 &tuple, const T &t) {
    return tuple_append_helper(tuple, t, simple::make_index_sequence<simple::tuple_size<T1>::value>());
}

#if 0
/**
 * tuple_head()  same as get<O> but return something in case the tuple is too small
 */
template<typename T, typename... Ts>
constexpr auto tuple_head(const simple::tuple<T, Ts...> &t)
{ return simple::tuple_head(t); }
constexpr auto tuple_head(const simple::tuple<> &) {
    struct _{};
    return _{};
}
#endif

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
template<int ...Sizes> using StaticStringList = simple::tuple<StaticString<Sizes>...>;

/* Creates a StaticStringList from a list of string literal */
template<int... N>
constexpr StaticStringList<N...> makeStaticStringList(StaticStringArray<N> & ...args)  {
    return simple::make_tuple(StaticString<N>(args)...);
}

/** concatenate() : returns a StaticString which is the concatenation of all the strings in a StaticStringList
 *     Note:  keeps the \0 between the strings
 */
template<typename I1, typename I2> struct concatenate_helper;
template<std::size_t... I1, std::size_t... I2> struct concatenate_helper<std::index_sequence<I1...>, std::index_sequence<I2...>> {
    static constexpr int size = sizeof...(I1) + sizeof...(I2);
    static constexpr auto concatenate(const StaticString<sizeof...(I1)> &s1, const StaticString<sizeof...(I2)> &s2) {
        StaticStringArray<size> d = { s1[I1]... , s2[I2]... };
        return StaticString<size>( d );
    }
};
constexpr StaticString<1> concatenate(const StaticStringList<>) { return {""}; }
template<int H,  int... T> constexpr auto concatenate(const StaticStringList<H, T...> &s) {
    auto tail = concatenate(tuple_tail(s));
    return concatenate_helper<simple::make_index_sequence<H>, simple::make_index_sequence<tail.size>>::concatenate(simple::tuple_head(s), tail);
}

/** Add a string in a StaticStringList */
template<int L, int...N >
constexpr auto addString(const StaticStringList<N...> &l, const StaticString<L> & s) {
    return tuple_append(l, s);
}


/*-----------*/

template<int N = 255> struct w_number : public w_number<N - 1> {
    static constexpr int value = N;
    static constexpr w_number<N-1> prev() { return {}; }
};
template<> struct w_number<0> { static constexpr int value = 0; };


// Mirror of QMetaMethod::Access
namespace W_Access {
/* // From qmetaobject_p.h MethodFlags
    AccessPrivate = 0x00,
    AccessProtected = 0x01,
    AccessPublic = 0x02,
    AccessMask = 0x03, //mask
 */
    constexpr w_number<0x02> Public{};
    constexpr w_number<0x01> Protected{};
    constexpr w_number<0x00> Private{};
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
    constexpr w_number<0x00> Method{};
    constexpr w_number<0x04> Signal{};
    constexpr w_number<0x08> Slot{};
    constexpr w_number<0x0c> Constructor{};
}

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
constexpr std::integral_constant<int, MetaObjectBuilder::Constant> W_Constant{};
constexpr std::integral_constant<int, MetaObjectBuilder::Final> W_Final{};


// workaround to avoid leading coma in macro that can optionaly take a flag
struct W_RemoveLeadingComa { constexpr w_number<0> operator+() const { return {}; } };
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

    template<typename F, int N, typename ParamTypes, int Flags = W_Access::Public.value>
    constexpr MetaMethodInfo<F, N, Flags | W_MethodType::Slot.value, ParamTypes>
    makeMetaSlotInfo(F f, StaticStringArray<N> &name, const ParamTypes &paramTypes, w_number<Flags> = {})
    { return { f, {name}, paramTypes, {} }; }

    template<typename F, int N, typename ParamTypes, int Flags = W_Access::Public.value>
    constexpr MetaMethodInfo<F, N, Flags | W_MethodType::Method.value, ParamTypes>
    makeMetaMethodInfo(F f, StaticStringArray<N> &name, const ParamTypes &paramTypes, w_number<Flags> = {})
    { return { f, {name}, paramTypes, {} }; }

    template<typename F, int N, typename ParamTypes, typename ParamNames>
    constexpr MetaMethodInfo<F, N, W_MethodType::Signal.value | W_Access::Public.value, ParamTypes, ParamNames>
    makeMetaSignalInfo(F f, StaticStringArray<N> &name, const ParamTypes &paramTypes, const ParamNames &paramNames)
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
             typename Notify = std::nullptr_t, int Flags = 0>
    struct MetaPropertyInfo {
        using PropertyType = Type;
        StaticString<NameLength> name;
        StaticString<TypeLength> typeStr;
        Getter getter;
        Setter setter;
        Member member;
        Notify notify;
        static constexpr uint flags = Flags;

        template <typename S> constexpr auto setGetter(const S&s) const {
            return MetaPropertyInfo<Type, NameLength, TypeLength, S, Setter, Member, Notify,
                                    Flags | PropertyFlags::Readable>
            {name, typeStr, s, setter, member, notify};
        }
        template <typename S> constexpr auto setSetter(const S&s) const {
            return MetaPropertyInfo<Type, NameLength, TypeLength, Getter, S, Member, Notify,
                                    Flags | PropertyFlags::Writable>
            {name, typeStr, getter, s, member, notify};
        }
        template <typename S> constexpr auto setMember(const S&s) const {
            return MetaPropertyInfo<Type, NameLength, TypeLength, Getter, Setter, S, Notify,
                                    Flags | PropertyFlags::Writable | PropertyFlags::Readable>
            {name, typeStr, getter, setter, s, notify};
        }
        template <typename S> constexpr auto setNotify(const S&s) const {
            return MetaPropertyInfo<Type, NameLength, TypeLength, Getter, Setter, Member, S,
                                    Flags | PropertyFlags::Notify>
            { name, typeStr, getter, setter, member, s};
        }
        template <int Flag> constexpr auto addFlag() const {
            return MetaPropertyInfo<Type, NameLength, TypeLength, Getter, Setter, Member, Notify,
                                    Flags | Flag>
            { name, typeStr, getter, setter, member, notify};
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
    // other flags flags
    template <typename PropInfo, int Flag, typename... Tail>
    constexpr auto parseProperty(const PropInfo &p, std::integral_constant<int, Flag>, Tail... t)
    { return parseProperty(p.template addFlag<Flag>() ,t...); }

    template<typename T, int N1, int N2, typename ... Args>
    constexpr auto makeMetaPropertyInfo(StaticStringArray<N1> &name, StaticStringArray<N2> &type, Args... args) {
        MetaPropertyInfo<T, N1, N2> meta
        { {name}, {type}, {}, {}, {}, {} };
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


template<typename T> T getParentObjectHelper(void* (T::*)(const char*));

struct FriendHelper2;

// private macro helpers

// strignify and make a StaticStringList out of an array of arguments
#define W_PARAM_TOSTRING(...) W_PARAM_TOSTRING2(__VA_ARGS__ ,,,,,,,,,,,,,,,,)
#define W_PARAM_TOSTRING2(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,...) \
makeStaticStringList(#A1,#A2,#A3,#A4,#A5,#A6,#A7,#A8,#A9,#A10,#A11,#A12,#A13,#A14,#A15,#A16)

#define W_MACRO_EMPTY
#define W_MACRO_EVAL(...) __VA_ARGS__
#define W_MACRO_DELAY(X,...) X(__VA_ARGS__)
#define W_MACRO_DELAY2(X,...) X(__VA_ARGS__)
#define W_MACRO_TAIL(A, ...) __VA_ARGS__

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
        static constexpr simple::tuple<> w_SlotState(w_number<0>) { return {}; } \
        static constexpr simple::tuple<> w_SignalState(w_number<0>) { return {}; } \
        static constexpr simple::tuple<> w_MethodState(w_number<0>) { return {}; } \
        static constexpr simple::tuple<> w_ConstructorState(w_number<0>) { return {}; } \
        static constexpr simple::tuple<> w_PropertyState(w_number<0>) { return {}; } \
        static constexpr simple::tuple<> w_EnumState(w_number<0>) { return {}; } \
        static constexpr simple::tuple<> w_ClassInfoState(w_number<0>) { return {}; } \
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
    static constexpr auto w_SlotState(w_number<simple::tuple_size<decltype(w_SlotState(w_number<>{}))>::value+1> counter) \
        W_RETURN(tuple_append(w_SlotState(counter.prev()), MetaObjectBuilder::makeMetaSlotInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME,  \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), \
            W_OVERLOAD_REMOVE(__VA_ARGS__) +W_removeLeadingComa)))

#define W_INVOKABLE(NAME, ...) \
    static constexpr auto w_MethodState(w_number<simple::tuple_size<decltype(w_MethodState(w_number<>{}))>::value+1> counter) \
        W_RETURN(tuple_append(w_MethodState(counter.prev()), MetaObjectBuilder::makeMetaMethodInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME,  \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), \
            W_OVERLOAD_REMOVE(__VA_ARGS__) +W_removeLeadingComa)))

#define W_SIGNAL_1(...) \
    __VA_ARGS__
#define W_SIGNAL_2(NAME, ...) \
    { \
        using w_SignalType = decltype(W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME)); \
        return SignalImplementation<w_SignalType, w_signalIndex_##NAME>{this}(W_OVERLOAD_REMOVE(__VA_ARGS__)); \
    } \
    static constexpr int w_signalIndex_##NAME = simple::tuple_size<decltype(w_SignalState(w_number<>{}))>::value; \
    static constexpr auto w_SignalState(w_number<w_signalIndex_##NAME + 1> counter) \
        W_RETURN(tuple_append(w_SignalState(counter.prev()), MetaObjectBuilder::makeMetaSignalInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME, \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), W_PARAM_TOSTRING(W_OVERLOAD_REMOVE(__VA_ARGS__)))))

#define W_CONSTRUCTOR(...) \
    static constexpr auto w_ConstructorState(w_number<simple::tuple_size<decltype(w_ConstructorState(w_number<>{}))>::value+1> counter) \
        W_RETURN(tuple_append(w_ConstructorState(counter.prev()), \
            MetaObjectBuilder::makeMetaConstructorInfo<__VA_ARGS__>().setName(W_UnscopedName)))

#define W_PROPERTY(...) W_PROPERTY2(__VA_ARGS__) // expands the READ, WRITE, and other sub marcos
#define W_PROPERTY2(TYPE, NAME, ...) \
    static constexpr auto w_PropertyState(w_number<simple::tuple_size<decltype(w_PropertyState(w_number<>{}))>::value+1> counter) \
        W_RETURN(tuple_append(w_PropertyState(counter.prev()), \
                              MetaObjectBuilder::makeMetaPropertyInfo<TYPE>(#NAME, #TYPE, __VA_ARGS__)))

#define W_ENUM(NAME, ...) \
    static constexpr auto w_EnumState(w_number<simple::tuple_size<decltype(w_EnumState(w_number<>{}))>::value+1> counter) \
        W_RETURN(tuple_append(w_EnumState(counter.prev()), MetaObjectBuilder::makeMetaEnumInfo<NAME,false>( \
            #NAME, std::integer_sequence<NAME,__VA_ARGS__>{}, W_PARAM_TOSTRING(__VA_ARGS__)))) \
    Q_ENUM(NAME)

template<typename T> struct QEnumOrQFlags { using Type = T; };
template<typename T> struct QEnumOrQFlags<QFlags<T>> { using Type = T; };

#define W_FLAG(NAME, ...) \
    static constexpr auto w_EnumState(w_number<simple::tuple_size<decltype(w_EnumState(w_number<>{}))>::value+1> counter) \
        W_RETURN(tuple_append(w_EnumState(counter.prev()), MetaObjectBuilder::makeMetaEnumInfo<QEnumOrQFlags<NAME>::Type,true>( \
            #NAME, std::integer_sequence<QEnumOrQFlags<NAME>::Type,__VA_ARGS__>{}, W_PARAM_TOSTRING(__VA_ARGS__)))) \
    Q_FLAG(NAME)

#define W_CLASSINFO(A, B) \
    static constexpr auto w_ClassInfoState(w_number<simple::tuple_size<decltype(w_ClassInfoState(w_number<>{}))>::value+1> counter) \
        W_RETURN(tuple_append(w_ClassInfoState(counter.prev()), \
            std::pair<StaticString<sizeof(A)>, StaticString<sizeof(B)>>{ {A}, {B} }))

#define WRITE , &W_ThisType::
#define READ , &W_ThisType::
#define NOTIFY , W_Notify, &W_ThisType::
#define MEMBER , &W_ThisType::
#define CONSTANT , W_Constant
#define FINAL , W_Final