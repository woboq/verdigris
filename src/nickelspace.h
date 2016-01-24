#pragma once

#include <QtCore/qobjectdefs.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qobject.h> // move to impl header
#include <tuple>
#include <utility>

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
    return std::make_tuple(std::get<I+1>(t)...);
}
template<typename T> constexpr auto tuple_tail(const T& tuple) {
    return tuple_tail_helper(tuple, std::make_index_sequence<std::tuple_size<T>::value-1>());
}
constexpr auto tuple_tail(std::tuple<>)
{ return std::tuple<>{}; }


/**
 * tuple_append() Appends one element to the tuple (faster than tuple_cat)
 */
template<typename... Ts, typename T, std::size_t...I>
constexpr auto tuple_append_helper(const std::tuple<Ts...> &tuple, const T &t, std::index_sequence<I...>) {
    return std::tuple<Ts..., T>{std::get<I>(tuple)... , t};
}
template<typename T1, typename T>
constexpr auto tuple_append(const T1 &tuple, const T &t) {
    return tuple_append_helper(tuple, t, std::make_index_sequence<std::tuple_size<T1>::value>());
}

/**
 * tuple_head()  same as get<O> but return something in case the tuple is too small
 */
template<typename T, typename... Ts>
constexpr auto tuple_head(const std::tuple<T, Ts...> &t)
{ return std::get<0>(t); }
constexpr auto tuple_head(const std::tuple<> &) {
    struct _{};
    return _{};
}


/**
 * ones()
 * return an index_sequence with N ones
 **/
template<typename> struct ones_helper;
template<std::size_t...I> struct ones_helper<std::index_sequence<I...>>
{ using result = std::index_sequence<(void(I),1)...>; };
template<int N> using ones = typename ones_helper<std::make_index_sequence<N>>::result;

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
    constexpr StaticString(StaticStringArray<N> &d) : StaticString(d, std::make_index_sequence<N>()) {}
    static constexpr int size = N;
    constexpr char operator[](int p) const { return data[p]; }
};
template <int N> constexpr StaticString<N> makeStaticString(StaticStringArray<N> &d) { return {d}; }

/* A tuple containing many  StaticString with possibly different sizes */
template<int ...Sizes> using StaticStringList = std::tuple<StaticString<Sizes>...>;

/* Creates a StaticStringList from a list of string literal */
template<int... N>
constexpr StaticStringList<N...> makeStaticStringList(StaticStringArray<N> & ...args)  {
    return std::make_tuple(StaticString<N>(args)...);
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
    return concatenate_helper<std::make_index_sequence<H>, std::make_index_sequence<tail.size>>::concatenate(std::get<0>(s), tail);
}

/** Add a string in a StaticStringList */
template<int L, int...N >
constexpr auto addString(const StaticStringList<N...> &l, const StaticString<L> & s) {
    return tuple_append(l, s);
}

// this variant does nothing
template<int ...N >
constexpr auto addString(const StaticStringList<N...> &l, int) {
    return l;
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

// workaround to avoid leading coma in macro that can optionaly take a flag
struct W_RemoveLeadingComa { constexpr w_number<0> operator+() const { return {}; } };
template <typename T> constexpr T operator+(T &&t, W_RemoveLeadingComa) { return t; }
constexpr W_RemoveLeadingComa W_removeLeadingComa{};

constexpr struct {} W_Notify{};

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
        using ReturnType = typename QtPrivate::FunctionPointer<F>::ReturnType;

        template<typename T>
        void metacall(T *_o, QMetaObject::Call _c, void **_a) const {
            using P = QtPrivate::FunctionPointer<F>;
            if (_c == QMetaObject::InvokeMetaMethod) {
                P::template call<typename P::Arguments, ReturnType>(func, _o, _a);
            } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
                auto _t = QtPrivate::ConnectionTypes<typename P::Arguments>::types();
                uint arg = *reinterpret_cast<int*>(_a[1]);
                *reinterpret_cast<int*>(_a[0]) = _t && arg < QtPrivate::FunctionPointer<F>::ArgumentCount ?
                    _t[arg] : -1;
            }
        }
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


    template<typename... Args> struct MetaConstructorInfo {
        static constexpr int argCount = sizeof...(Args);
        static constexpr int flags = W_MethodType::Constructor.value | W_Access::Public.value;
        static constexpr int name = 0;
        template<typename T, std::size_t... I>
        void createInstance(void **_a, std::index_sequence<I...>) const {
            *reinterpret_cast<T**>(_a[0]) =
                new T(*reinterpret_cast<typename std::remove_reference<Args>::type *>(_a[I+1])...);
        }
    };
    template<typename...  Args> constexpr MetaConstructorInfo<Args...> makeMetaConstructorInfo()
    { return { }; }

    /** Holds information about a property */
    template<typename Type, int NameLength, int TypeLength, typename Getter, typename Setter, typename Member, typename Notify>
    struct MetaPropertyInfo {
        using PropertyType = Type;
        StaticString<NameLength> name;
        StaticString<TypeLength> typeStr;
        Getter getter;
        Setter setter;
        Member member;
        Notify notify;
        uint flags = 0;

        template <typename S> constexpr auto setGetter(const S&s) const
        { return MetaPropertyInfo<Type, NameLength, TypeLength, S, Setter, Member, Notify> {name, typeStr, s, setter, member, notify, flags}; }
        template <typename S> constexpr auto setSetter(const S&s) const
        { return MetaPropertyInfo<Type, NameLength, TypeLength, Getter, S, Member, Notify> {name, typeStr, getter, s, member, notify, flags}; }
        template <typename S> constexpr auto setMember(const S&s) const
        { return MetaPropertyInfo<Type, NameLength, TypeLength, Getter, Setter, S, Notify> {name, typeStr, getter, setter, s, notify, flags}; }
        template <typename S> constexpr auto setNotify(const S&s) const
        { return MetaPropertyInfo<Type, NameLength, TypeLength, Getter, Setter, Member, S> {name, typeStr, getter, setter, member, s, flags}; }

        template<typename T>
        void metacall(T *_o, QMetaObject::Call _c, void **_a) const {
            switch(+_c) {
            case QMetaObject::ReadProperty:
                if (getter) {
                    *reinterpret_cast<Type*>(_a[0]) = (_o->*getter)();
                } else if (member) {
                    *reinterpret_cast<Type*>(_a[0]) = _o->*member;
                }
                break;
            case QMetaObject::WriteProperty:
                if (setter) {
                    (_o->*setter)(*reinterpret_cast<Type*>(_a[0]));
                } else if (member) {
                    _o->*member = *reinterpret_cast<Type*>(_a[0]);
                }
            }
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

    template<typename T, int N1, int N2, typename ... Args>
    constexpr auto makeMetaPropertyInfo(StaticStringArray<N1> &name, StaticStringArray<N2> &type, Args... args) {
        MetaPropertyInfo<T, N1, N2, T(QObject::*)(), void(QObject::*)(const T&), T QObject::*, int> meta
        { {name}, {type}, {}, {}, {}, {} };
        return parseProperty(meta, args...);
    }

    /** Holds information about a class,  includeing all the properties and methods */
    template<int NameLength, typename Methods, typename Constructors, typename Properties, int SignalCount>
    struct ClassInfo {
        StaticString<NameLength> name;
        Methods methods;
        Constructors constructors;
        Properties properties;

        static constexpr int methodCount = std::tuple_size<Methods>::value;
        static constexpr int constructorCount = std::tuple_size<Constructors>::value;
        static constexpr int propertyCount = std::tuple_size<Properties>::value;
        static constexpr int signalCount = SignalCount;
    };

struct FriendHelper1 { /* FIXME */
    /** Construct a ClassInfo with just the name */
    template<typename T, int N>
    static constexpr auto makeClassInfo(StaticStringArray<N> &name) {
        const auto sigState = T::w_SignalState(w_number<>{});
        const auto methodInfo = std::tuple_cat(sigState, T::w_SlotState(w_number<>{}), T::w_MethodState(w_number<>{}));
        const auto constructorInfo = T::w_ConstructorState(w_number<>{});
        const auto propertyInfo = T::w_PropertyState(w_number<>{});
        constexpr int sigCount = std::tuple_size<decltype(sigState)>::value;
        return ClassInfo<N, decltype(methodInfo), decltype(constructorInfo), decltype(propertyInfo), sigCount>
            { {name}, methodInfo, constructorInfo, propertyInfo };
    }
};

    template<typename T, int N>
    constexpr auto makeClassInfo(StaticStringArray<N> &name) { return FriendHelper1::makeClassInfo<T>(name); }


    /**
     * generate...
     *  Create the metaobject's integer data array
     *  (as a index_sequence)
     * returns std::pair<StaticStringList, index_sequence>:  the modified strings and the array of strings
     */
    template<int, typename Strings>
    constexpr auto generateMethods(const Strings &s, const std::tuple<>&) {
        return std::make_pair(s, std::index_sequence<>());
    }
    template<int ParamIndex, typename Strings, typename Method, typename... Tail>
    constexpr auto generateMethods(const Strings &s, const std::tuple<Method, Tail...> &t) {

        auto method = std::get<0>(t);
        auto s2 = addString(s, method.name);

        constexpr bool isConstructor = (Method::flags & 0xc) == W_MethodType::Constructor.value;

        using thisMethod = std::index_sequence<
            isConstructor ? 0 : std::tuple_size<Strings>::value, //name
            Method::argCount,
            ParamIndex, //parametters
            1, //tag, always \0
            Method::flags
        >;

        auto next = generateMethods<ParamIndex + 1 + Method::argCount * 2>(s2, tuple_tail(t));
        return std::make_pair(next.first, thisMethod() + next.second);
    }

    template <typename T, typename = void> struct MetaTypeIdIsBuiltIn : std::false_type {};
    template <typename T> struct MetaTypeIdIsBuiltIn<T, typename std::enable_if<QMetaTypeId2<T>::IsBuiltIn>::type> : std::true_type{};
    enum { IsUnresolvedType = 0x80000000 };

    template<typename T, bool Builtin = MetaTypeIdIsBuiltIn<T>::value>
    struct HandleType {
        template<typename S, typename TypeStr = int>
        static constexpr auto result(const S&s, TypeStr = {})
        { return std::make_pair(s, std::index_sequence<QMetaTypeId2<T>::MetaType>()); }
    };
    template<typename T>
    struct HandleType<T, false> {
        template<typename Strings, typename TypeStr = int>
        static constexpr auto result(const Strings &ss, TypeStr = {}) {
            static_assert(W_TypeRegistery<T>::registered, "Please Register T with W_DECLARE_METATYPE");
            auto s2 = addString(ss, W_TypeRegistery<T>::name);
            return std::make_pair(s2, std::index_sequence<IsUnresolvedType
                                                | std::tuple_size<Strings>::value>());
        }
        template<typename Strings, int N>
        static constexpr auto result(const Strings &ss, StaticString<N> typeStr,
                                     typename std::enable_if<(N>1),int>::type=0) {
            auto s2 = addString(ss, typeStr);
            return std::make_pair(s2, std::index_sequence<IsUnresolvedType
                    | std::tuple_size<Strings>::value>());
        }
    };

    template<typename Strings>
    constexpr auto generateProperties(const Strings &s, const std::tuple<>&) {
        return std::make_pair(s, std::index_sequence<>());
    }
    template<typename Strings, typename Prop, typename... Tail>
    constexpr auto generateProperties(const Strings &s, const std::tuple<Prop, Tail...> &t) {

        auto prop = std::get<0>(t);
        auto s2 = addString(s, prop.name);
        auto type = HandleType<typename Prop::PropertyType>::result(s2, prop.typeStr);
        auto next = generateProperties(type.first, tuple_tail(t));

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

        constexpr std::size_t flags = (Writable|Readable); // FIXME

        auto thisProp = std::index_sequence<std::tuple_size<Strings>::value>() //name
                        + type.second
                        + std::index_sequence<flags>()
                        + next.second;
        return std::make_pair(next.first, thisProp);

    }

    //Helper class for generateSingleMethodParameter:  generate the parametter array

    template<typename ...Args> struct HandleArgsHelper {
        template<typename Strings, typename ParamTypes>
        static constexpr auto result(const Strings &ss, const ParamTypes&)
        { return std::make_pair(ss, std::index_sequence<>()); }
    };
    template<typename A, typename... Args>
    struct HandleArgsHelper<A, Args...> {
        template<typename Strings, typename ParamTypes>
        static constexpr auto result(const Strings &ss, const ParamTypes &paramTypes) {
            using Type = typename QtPrivate::RemoveConstRef<A>::Type;
            auto typeStr = tuple_head(paramTypes);
            using ts_t = decltype(typeStr);
            // This way, the overload of result will not pick the StaticString one if it is a tuple (because registered types have the priority)
            auto typeStr2 = std::conditional_t<std::is_same<A, Type>::value, ts_t, std::tuple<ts_t>>{typeStr};
            auto r1 = HandleType<Type>::result(ss, typeStr2);
            auto r2 = HandleArgsHelper<Args...>::result(r1.first, tuple_tail(paramTypes));
            return std::make_pair(r2.first, r1.second + r2.second);
        }
    };

    template<int N> struct HandleArgNames{
        template<typename Strings, int S, int...T>
        static constexpr auto result(const Strings &ss, StaticStringList<S, T...> pn)
        {
            auto s2 = addString(ss, std::get<0>(pn));
            auto tail = tuple_tail(pn);
            auto t = HandleArgNames<N-1>::result(s2, tail);
            return std::make_pair(t.first, std::index_sequence<std::tuple_size<Strings>::value>() + t.second );
        }
        template<typename Strings> static constexpr auto result(const Strings &ss, StaticStringList<>)
        { return std::make_pair(ss, ones<N>()); }

    };
    template<> struct HandleArgNames<0> {
        template<typename Strings, typename PN> static constexpr auto result(const Strings &ss, PN)
        { return std::make_pair(ss, std::index_sequence<>()); }
    };

    template<typename Strings, typename ParamTypes, typename ParamNames, typename Obj, typename Ret, typename... Args>
    constexpr auto generateSingleMethodParameter(const Strings &ss, Ret (Obj::*)(Args...),
                                                 const ParamTypes &paramTypes, const ParamNames &paramNames ) {
        auto types = HandleArgsHelper<Ret, Args...>::result(ss, std::tuple_cat(std::tuple<int>{}, paramTypes));
        auto names = HandleArgNames<sizeof...(Args)>::result(types.first, paramNames);
        return std::make_pair(names.first, types.second + names.second);
    }
    template<typename Strings, typename ParamTypes, typename ParamNames, typename Obj, typename Ret, typename... Args>
    constexpr auto generateSingleMethodParameter(const Strings &ss, Ret (Obj::*)(Args...) const,
                                                 const ParamTypes &paramTypes, const ParamNames &paramNames ) {
        auto types = HandleArgsHelper<Ret, Args...>::result(ss, std::tuple_cat(std::tuple<int>{}, paramTypes));
        auto names = HandleArgNames<sizeof...(Args)>::result(types.first, paramNames);
        return std::make_pair(names.first, types.second + names.second);
    }


    template<typename Strings>
    constexpr auto generateMethodsParameters(const Strings &s, const std::tuple<>&) {
        return std::make_pair(s, std::index_sequence<>());
    }
    template<typename Strings, typename Method, typename... Tail>
    constexpr auto generateMethodsParameters(const Strings &s, const std::tuple<Method, Tail...> &t) {
        auto method = std::get<0>(t);
        auto thisMethod = generateSingleMethodParameter(s, method.func, method.paramTypes, method.paramNames);
        auto next = generateMethodsParameters(thisMethod.first, tuple_tail(t));
        return std::make_pair(next.first, thisMethod.second + next.second);
    }

    template<typename Strings>
    constexpr auto generateConstructorParameters(const Strings &s, const std::tuple<>&) {
        return std::make_pair(s, std::index_sequence<>());
    }
    template<typename Strings, typename... Args, typename... Tail>
    constexpr auto generateConstructorParameters(const Strings &ss, const std::tuple<MetaConstructorInfo<Args...>, Tail...> &t) {
        auto returnT = std::index_sequence<IsUnresolvedType | 1>{};
        auto types = HandleArgsHelper<Args...>::result(ss, std::tuple<>{});
        auto names = ones<sizeof...(Args)>{};
        auto next = generateConstructorParameters(types.first, tuple_tail(t));
        return std::make_pair(next.first, returnT + types.second + names + next.second);
    }

    template<typename Methods, std::size_t... I>
    constexpr int paramOffset(std::index_sequence<I...>)
    { return sums(int(1 + std::tuple_element_t<I, Methods>::argCount * 2)...); }

    // generate the integer array and the lists of string
    template<typename CI>
    constexpr auto generateDataArray(const CI &classInfo) {
        constexpr int methodOffset = 14;
        constexpr int propertyOffset = methodOffset + CI::methodCount * 5;
        constexpr int constructorOffset = propertyOffset + CI::propertyCount * 3 ;
        constexpr int paramIndex = constructorOffset + CI::constructorCount * 5 ;
        constexpr int constructorParamIndex = paramIndex +
            paramOffset<decltype(classInfo.methods)>(std::make_index_sequence<CI::methodCount>{});
        using header = std::index_sequence<
                7,       // revision
                0,       // classname
                0,    0, // classinfo
                CI::methodCount,   methodOffset, // methods
                CI::propertyCount,    propertyOffset, // properties
                0,    0, // enums/sets
                CI::constructorCount, constructorOffset, // constructors
                0,       // flags
                CI::signalCount
        >;
        auto stringData = std::make_tuple(classInfo.name, StaticString<1>(""));
        auto methods = generateMethods<paramIndex>(stringData , classInfo.methods);
        auto properties = generateProperties(methods.first , classInfo.properties);
        auto constructors = generateMethods<constructorParamIndex>(properties.first, classInfo.constructors);
        auto parametters = generateMethodsParameters(constructors.first, classInfo.methods);
        auto parametters2 = generateConstructorParameters(parametters.first, classInfo.constructors);
        return std::make_pair(parametters2.first,  header()  + methods.second + properties.second +
                    parametters.second + parametters2.second);
    }


    /**
     * Entry class for the generation of the QMetaObject
     */
/*
    template<typename T> struct MetaObjectCreatorHelper {
    private:
//         static constexpr auto names = typename_<T>::staticStringList();
//         static constexpr auto values = typedef_<T>::tuple();
//         static constexpr auto zipped = zip(values, names);

    public:

//        using Base =  typename std::remove_reference<decltype(*std::get<1>(values))>::type;

        static constexpr auto classInfo = makeClassInfo<T>();
        static constexpr auto data = generateDataArray(classInfo);
        static constexpr auto string_data = data.first;
        static constexpr auto int_data = data.second;
    };*/


    /**
     * Holder for the string data.  Just like in the moc generated code.
     */
    template<int N, int L> struct qt_meta_stringdata_t {
         QByteArrayData data[N];
         char stringdata[L];
    };

    /** Builds the string data
     * \param S: a index_sequence that goes from 0 to the fill size of the strings
     * \param I: a index_sequence that goes from 0 to the number of string
     * \param O: a index_sequence of the offsets
     * \param N: a index_sequence of the size of each strings
     * \param T: the MetaObjectCreatorHelper
     */
    template<typename S, typename I, typename O, typename N, typename T> struct BuildStringDataHelper;
    template<std::size_t... S, std::size_t... I, std::size_t... O, std::size_t...N, typename T>
    struct BuildStringDataHelper<std::index_sequence<S...>, std::index_sequence<I...>, std::index_sequence<O...>, std::index_sequence<N...>, T> {
        using meta_stringdata_t = const qt_meta_stringdata_t<sizeof...(I), sizeof...(S)>;
        static meta_stringdata_t qt_meta_stringdata;
    };
    template<std::size_t... S, std::size_t... I, std::size_t... O, std::size_t...N, typename T>
    const qt_meta_stringdata_t<sizeof...(I), sizeof...(S)>
    BuildStringDataHelper<std::index_sequence<S...>, std::index_sequence<I...>, std::index_sequence<O...>, std::index_sequence<N...>, T>::qt_meta_stringdata = {
        {Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(N-1,
                qptrdiff(offsetof(meta_stringdata_t, stringdata) + O - I * sizeof(QByteArrayData)) )...},
        { concatenate(T::string_data)[S]...     }
    };


    /**
     * Given N a list of string sizes, compute the list offsets to each of the strings.
     */
    template<std::size_t... N> struct ComputeOffsets;
    template<> struct ComputeOffsets<> {
        using Result = std::index_sequence<>;
    };
    template<std::size_t H, std::size_t... T> struct ComputeOffsets<H, T...> {
        template<std::size_t ... I> static std::index_sequence<0, (I+H)...> func(std::index_sequence<I...>);
        using Result = decltype(func(typename ComputeOffsets<T...>::Result()));
    };

    /**
     * returns the string data suitable for the QMetaObject from a list of string
     * T is MetaObjectCreatorHelper<ObjectType>
     */
    template<typename T, int... N>
    constexpr const QByteArrayData *build_string_data(StaticStringList<N...>)  {
        return BuildStringDataHelper<std::make_index_sequence<sums(N...)>,
                                     std::make_index_sequence<sizeof...(N)>,
                                     typename ComputeOffsets<N...>::Result,
                                     std::index_sequence<N...>,
                                      T>
            ::qt_meta_stringdata.data;
    }

    /**
     * returns a pointer to an array of string built at compile time.
     */
    template<typename I> struct build_int_data;
    template<std::size_t... I> struct build_int_data<std::index_sequence<I...>> {
        static const uint data[sizeof...(I)];
    };
    template<std::size_t... I> const uint build_int_data<std::index_sequence<I...>>::data[sizeof...(I)] = { uint(I)... };


    /**
     * calls metacall on each element of the tuple
     */
    template<typename T> void metacall(T *, QMetaObject::Call, int , void** , const std::tuple<> &) {}
    template<typename T, typename Ms> void metacall(T *_o, QMetaObject::Call _c, int _id, void** _a, const Ms &ms) {
        if (_id == 0) {
            std::get<0>(ms).metacall(_o, _c, _a);
        } else {
           metacall(_o, _c, _id-1, _a, tuple_tail(ms));
        }
    }

    /**
     * Helper for QMetaObject::IndexOfMethod
     */
    void indexOfMethod (int *, void **, int, const std::tuple<> &) {}
    template<typename Ms> void indexOfMethod (int *result, void **func, int _id, const Ms &ms) {
        auto f = std::get<0>(ms).func;
        if (std::get<0>(ms).flags & W_MethodType::Signal.value
                &&  f == *reinterpret_cast<decltype(f)*>(func)) {
            *result = _id;
        } else {
            indexOfMethod(result, func, _id+1, tuple_tail(ms));
        }
    }

    /**
     * helper for QMetaObject::createInstance
     */
    template<typename T> void createInstance(int, void** , const std::tuple<> &) {}
    template<typename T, typename Ms> void createInstance(int _id, void** _a, const Ms &ms) {
        if (_id == 0) {
            auto m = std::get<0>(ms);
            m.template createInstance<T>(_a, std::make_index_sequence<decltype(m)::argCount>{});
        } else {
            createInstance<T>(_id-1, _a, tuple_tail(ms));
        }
    }


}


struct FriendHelper2 {

template<typename T>
static constexpr QMetaObject createMetaObject()
{

    using Creator = typename T::MetaObjectCreatorHelper;

    auto string_data = MetaObjectBuilder::build_string_data<Creator>(Creator::string_data);
    auto int_data = MetaObjectBuilder::build_int_data<typename std::remove_const<decltype(Creator::int_data)>::type>::data;

    return { { &T::W_BaseType::staticMetaObject , string_data , int_data,  T::qt_static_metacall, {}, {} }  };
}


template<typename T> static int qt_metacall_impl(T *_o, QMetaObject::Call _c, int _id, void** _a) {
    using Creator = typename T::MetaObjectCreatorHelper;
    _id = _o->T::W_BaseType::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod || _c == QMetaObject::RegisterMethodArgumentMetaType) {
        constexpr int methodCount = Creator::classInfo.methodCount;
        if (_id < methodCount)
            T::qt_static_metacall(_o, _c, _id, _a);
        _id -= methodCount;
    } else if ((_c >= QMetaObject::ReadProperty && _c <= QMetaObject::QueryPropertyUser)
                || _c == QMetaObject::RegisterPropertyMetaType) {
        constexpr auto ps = Creator::classInfo.properties;
        MetaObjectBuilder::metacall(_o, _c, _id, _a, ps);
    }
    return _id;
}

template<typename T> static void qt_static_metacall_impl(QObject *_o, QMetaObject::Call _c, int _id, void** _a) {
    constexpr auto ms = T::MetaObjectCreatorHelper::classInfo.methods;
    if (_c == QMetaObject::InvokeMetaMethod || _c == QMetaObject::RegisterMethodArgumentMetaType) {
        Q_ASSERT(T::staticMetaObject.cast(_o));
        MetaObjectBuilder::metacall(static_cast<T*>(_o), _c, _id, _a, ms);
    } else if (_c == QMetaObject::IndexOfMethod) {
        MetaObjectBuilder::indexOfMethod(reinterpret_cast<int *>(_a[0]), reinterpret_cast<void **>(_a[1]), 0, ms);
    } else if (_c == QMetaObject::CreateInstance) {
        constexpr auto cs = T::MetaObjectCreatorHelper::classInfo.constructors;
        MetaObjectBuilder::createInstance<T>(_id, _a, cs);
    }
}

};

template<typename T> constexpr auto createMetaObject() {  return FriendHelper2::createMetaObject<T>(); }
template<typename T, typename... Ts> auto qt_metacall_impl(Ts &&...args)
{  return FriendHelper2::qt_metacall_impl<T>(std::forward<Ts>(args)...); }
template<typename T, typename... Ts> auto qt_static_metacall_impl(Ts &&... args)
{  return FriendHelper2::qt_static_metacall_impl<T>(std::forward<Ts>(args)...); }



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


// public macros
#define W_OBJECT(TYPE) \
        using W_ThisType = TYPE; /* This is the only reason why we need TYPE */ \
        friend struct MetaObjectBuilder::FriendHelper1; \
        friend struct ::FriendHelper2; \
        static constexpr std::tuple<> w_SlotState(w_number<0>) { return {}; } \
        static constexpr std::tuple<> w_SignalState(w_number<0>) { return {}; } \
        static constexpr std::tuple<> w_MethodState(w_number<0>) { return {}; } \
        static constexpr std::tuple<> w_ConstructorState(w_number<0>) { return {}; } \
        static constexpr std::tuple<> w_PropertyState(w_number<0>) { return {}; } \
    public: \
        struct MetaObjectCreatorHelper; \
        using W_BaseType = decltype(getParentObjectHelper(&W_ThisType::qt_metacast)); \
    Q_OBJECT


#define W_OBJECT_IMPL(TYPE) \
    struct TYPE::MetaObjectCreatorHelper { \
        static constexpr auto classInfo = MetaObjectBuilder::makeClassInfo<TYPE>(#TYPE); \
        static constexpr auto data = generateDataArray(classInfo); \
        static constexpr auto string_data = data.first; \
        static constexpr auto int_data = data.second; \
    }; \
    constexpr const QMetaObject TYPE::staticMetaObject = createMetaObject<TYPE>(); \
    const QMetaObject *TYPE::metaObject() const  { return &staticMetaObject; } \
    void *TYPE::qt_metacast(const char *) { return nullptr; } /* TODO */ \
    int TYPE::qt_metacall(QMetaObject::Call _c, int _id, void** _a) { \
        return qt_metacall_impl<TYPE>(this, _c, _id, _a); \
    } \
    void TYPE::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void** _a) { \
        qt_static_metacall_impl<TYPE>(_o, _c, _id, _a); \
    } \



#define W_SLOT(NAME, ...) \
    static constexpr auto w_SlotState(w_number<std::tuple_size<decltype(w_SlotState(w_number<>{}))>::value+1> counter) \
        W_RETURN(tuple_append(w_SlotState(counter.prev()), MetaObjectBuilder::makeMetaSlotInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME,  \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), \
            W_OVERLOAD_REMOVE(__VA_ARGS__) +W_removeLeadingComa)))

#define W_INVOKABLE(NAME, ...) \
    static constexpr auto w_MethodState(w_number<std::tuple_size<decltype(w_MethodState(w_number<>{}))>::value+1> counter) \
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
    static constexpr int w_signalIndex_##NAME = std::tuple_size<decltype(w_SignalState(w_number<>{}))>::value; \
    static constexpr auto w_SignalState(w_number<w_signalIndex_##NAME + 1> counter) \
        W_RETURN(tuple_append(w_SignalState(counter.prev()), MetaObjectBuilder::makeMetaSignalInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME, \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), W_PARAM_TOSTRING(W_OVERLOAD_REMOVE(__VA_ARGS__)))))

#define W_CONSTRUCTOR(...) \
    static constexpr auto w_ConstructorState(w_number<std::tuple_size<decltype(w_MethodState(w_number<>{}))>::value+1> counter) \
        W_RETURN(tuple_append(w_ConstructorState(counter.prev()), MetaObjectBuilder::makeMetaConstructorInfo<__VA_ARGS__>()))



#define W_PROPERTY(TYPE, NAME, ...) \
    static constexpr auto w_PropertyState(w_number<std::tuple_size<decltype(w_PropertyState(w_number<>{}))>::value+1> counter) \
        W_RETURN(tuple_append(w_PropertyState(counter.prev()), \
                              MetaObjectBuilder::makeMetaPropertyInfo<TYPE>(#NAME, #TYPE, __VA_ARGS__)))
