#pragma once

#include "wobjectdefs.h"
#include <QtCore/qobject.h>

namespace MetaObjectBuilder {


        /** Holds information about a class,  includeing all the properties and methods */
    template<int NameLength, typename Methods, typename Constructors, typename Properties,
             typename Enums, typename ClassInfos, int SignalCount>
    struct ObjectInfo {
        StaticString<NameLength> name;
        Methods methods;
        Constructors constructors;
        Properties properties;
        Enums enums;
        ClassInfos classInfos;

        static constexpr int methodCount = simple::tuple_size<Methods>::value;
        static constexpr int constructorCount = simple::tuple_size<Constructors>::value;
        static constexpr int propertyCount = simple::tuple_size<Properties>::value;
        static constexpr int enumCount = simple::tuple_size<Enums>::value;
        static constexpr int signalCount = SignalCount;
    };

struct FriendHelper1 { /* FIXME */
    /** Construct a ObjectInfo with just the name */
    template<typename T, int N>
    static constexpr auto makeObjectInfo(StaticStringArray<N> &name) {
        const auto sigState = T::w_SignalState(w_number<>{});
        const auto methodInfo = simple::tuple_cat(sigState, T::w_SlotState(w_number<>{}), T::w_MethodState(w_number<>{}));
        const auto constructorInfo = T::w_ConstructorState(w_number<>{});
        const auto propertyInfo = T::w_PropertyState(w_number<>{});
        const auto enumInfo = T::w_EnumState(w_number<>{});
        const auto classInfo = T::w_ClassInfoState(w_number<>{});
        constexpr int sigCount = simple::tuple_size<decltype(sigState)>::value;
        return ObjectInfo<N, decltype(methodInfo), decltype(constructorInfo), decltype(propertyInfo),
                          decltype(enumInfo), decltype(classInfo), sigCount>
            { {name}, methodInfo, constructorInfo, propertyInfo, enumInfo, classInfo };
    }
};

    template<typename T, int N>
    constexpr auto makeObjectInfo(StaticStringArray<N> &name) { return FriendHelper1::makeObjectInfo<T>(name); }




    /**
     * generate...
     *  Create the metaobject's integer data array
     *  (as a index_sequence)
     * returns std::pair<StaticStringList, index_sequence>:  the modified strings and the array of strings
     */
    template<int, typename Strings>
    constexpr auto generateMethods(const Strings &s, const simple::tuple<>&) {
        return std::make_pair(s, std::index_sequence<>());
    }
    template<int ParamIndex, typename Strings, typename Method, typename... Tail>
    constexpr auto generateMethods(const Strings &s, const simple::tuple<Method, Tail...> &t) {

        auto method = simple::get<0>(t);
        auto s2 = addString(s, method.name);


        using thisMethod = std::index_sequence<
            simple::tuple_size<Strings>::value, //name
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
                                                | simple::tuple_size<Strings>::value>());
        }
        template<typename Strings, int N>
        static constexpr auto result(const Strings &ss, StaticString<N> typeStr,
                                     typename std::enable_if<(N>1),int>::type=0) {
            auto s2 = addString(ss, typeStr);
            return std::make_pair(s2, std::index_sequence<IsUnresolvedType
                    | simple::tuple_size<Strings>::value>());
        }
    };

    template<typename Strings>
    constexpr auto generateProperties(const Strings &s, const simple::tuple<>) {
        return std::make_pair(s, std::index_sequence<>());
    }
    template<typename Strings, typename Prop, typename... Tail>
    constexpr auto generateProperties(const Strings &s, const simple::tuple<Prop, Tail...> &t) {

        auto prop = simple::get<0>(t);
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

        auto thisProp = std::index_sequence<simple::tuple_size<Strings>::value>() //name
                        + type.second
                        + std::index_sequence<flags>()
                        + next.second;
        return std::make_pair(next.first, thisProp);

    }

    template<int DataIndex, typename Strings>
    constexpr auto generateEnums(const Strings &s, const simple::tuple<>) {
        return std::make_pair(s, std::index_sequence<>());
    }
    template<int DataIndex, typename Strings, typename Enum, typename... Tail>
    constexpr auto generateEnums(const Strings &s, const simple::tuple<Enum, Tail...> &t) {

        auto enum_ = simple::get<0>(t);
        auto s2 = addString(s, enum_.name);
        auto next = generateEnums<DataIndex + Enum::count*2>(s2, tuple_tail(t));

        auto thisProp = std::index_sequence<
                simple::tuple_size<Strings>::value, //name
                Enum::flags,
                Enum::count,
                DataIndex
            >{};

        return std::make_pair(next.first, thisProp + next.second);
    }

    template<typename Strings>
    constexpr auto generateSingleEnumValues(const Strings &s, std::index_sequence<> e, simple::tuple<>)
    { return std::make_pair(s, e); }

    template<typename Strings, std::size_t Value, std::size_t... I, typename Names>
    constexpr auto generateSingleEnumValues(const Strings &s, std::index_sequence<Value, I...>, Names names) {
        auto s2 = addString(s, simple::get<0>(names));
        auto next = generateSingleEnumValues(s2, std::index_sequence<I...>{}, tuple_tail(names));
        return std::make_pair(next.first, std::index_sequence<
            simple::tuple_size<Strings>::value, Value>{} + next.second);
    }

    template<typename Strings>
    constexpr auto generateEnumsValues(const Strings &s, const simple::tuple<> &)
    { return std::make_pair(s, std::index_sequence<>{}); }

    template<typename Strings, typename Enum, typename... Tail>
    constexpr auto generateEnumsValues(const Strings &s, const simple::tuple<Enum, Tail...> &t) {
        auto e = simple::get<0>(t);
        auto r = generateSingleEnumValues(s, typename Enum::Values{}, e.names);
        auto next = generateEnumsValues(r.first, tuple_tail(t));
        return std::make_pair(next.first,  r.second + next.second);
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
            auto typeStr2 = std::conditional_t<std::is_same<A, Type>::value, ts_t, simple::tuple<ts_t>>{typeStr};
            auto r1 = HandleType<Type>::result(ss, typeStr2);
            auto r2 = HandleArgsHelper<Args...>::result(r1.first, tuple_tail(paramTypes));
            return std::make_pair(r2.first, r1.second + r2.second);
        }
    };

    template<int N> struct HandleArgNames{
        template<typename Strings, int S, int...T>
        static constexpr auto result(const Strings &ss, StaticStringList<S, T...> pn)
        {
            auto s2 = addString(ss, simple::get<0>(pn));
            auto tail = tuple_tail(pn);
            auto t = HandleArgNames<N-1>::result(s2, tail);
            return std::make_pair(t.first, std::index_sequence<simple::tuple_size<Strings>::value>() + t.second );
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
        auto types = HandleArgsHelper<Ret, Args...>::result(ss, simple::tuple_cat(simple::tuple<int>{}, paramTypes));
        auto names = HandleArgNames<sizeof...(Args)>::result(types.first, paramNames);
        return std::make_pair(names.first, types.second + names.second);
    }
    template<typename Strings, typename ParamTypes, typename ParamNames, typename Obj, typename Ret, typename... Args>
    constexpr auto generateSingleMethodParameter(const Strings &ss, Ret (Obj::*)(Args...) const,
                                                 const ParamTypes &paramTypes, const ParamNames &paramNames ) {
        auto types = HandleArgsHelper<Ret, Args...>::result(ss, simple::tuple_cat(simple::tuple<int>{}, paramTypes));
        auto names = HandleArgNames<sizeof...(Args)>::result(types.first, paramNames);
        return std::make_pair(names.first, types.second + names.second);
    }


    template<typename Strings>
    constexpr auto generateMethodsParameters(const Strings &s, const simple::tuple<>&) {
        return std::make_pair(s, std::index_sequence<>());
    }
    template<typename Strings, typename Method, typename... Tail>
    constexpr auto generateMethodsParameters(const Strings &s, const simple::tuple<Method, Tail...> &t) {
        auto method = simple::get<0>(t);
        auto thisMethod = generateSingleMethodParameter(s, method.func, method.paramTypes, method.paramNames);
        auto next = generateMethodsParameters(thisMethod.first, tuple_tail(t));
        return std::make_pair(next.first, thisMethod.second + next.second);
    }

    template<typename Strings>
    constexpr auto generateConstructorParameters(const Strings &s, const simple::tuple<>&) {
        return std::make_pair(s, std::index_sequence<>());
    }
    template<typename Strings, int N, typename... Args, typename... Tail>
    constexpr auto generateConstructorParameters(const Strings &ss, const simple::tuple<MetaConstructorInfo<N,Args...>, Tail...> &t) {
        auto returnT = std::index_sequence<IsUnresolvedType | 1>{};
        auto types = HandleArgsHelper<Args...>::result(ss, simple::tuple<>{});
        auto names = ones<sizeof...(Args)>{};
        auto next = generateConstructorParameters(types.first, tuple_tail(t));
        return std::make_pair(next.first, returnT + types.second + names + next.second);
    }

    template<typename Methods, std::size_t... I>
    constexpr int paramOffset(std::index_sequence<I...>)
    { return sums(int(1 + simple::tuple_element_t<I, Methods>::argCount * 2)...); }

    // generate the integer array and the lists of string
    template<typename ObjI>
    constexpr auto generateDataArray(const ObjI &objectInfo) {
        constexpr int methodOffset = 14;
        constexpr int propertyOffset = methodOffset + ObjI::methodCount * 5;
        constexpr int enumOffset = propertyOffset + ObjI::propertyCount * 3;
        constexpr int constructorOffset = enumOffset + ObjI::enumCount* 4;
        constexpr int paramIndex = constructorOffset + ObjI::constructorCount * 5 ;
        constexpr int constructorParamIndex = paramIndex +
            paramOffset<decltype(objectInfo.methods)>(simple::make_index_sequence<ObjI::methodCount>{});
        constexpr int enumValueOffset = constructorParamIndex +
            paramOffset<decltype(objectInfo.constructors)>(simple::make_index_sequence<ObjI::constructorCount>{});
        using header = std::index_sequence<
                7,       // revision
                0,       // classname
                0,    0, // classinfo
                ObjI::methodCount,   methodOffset, // methods
                ObjI::propertyCount,    propertyOffset, // properties
                ObjI::enumCount,    enumOffset, // enums/sets
                ObjI::constructorCount, constructorOffset, // constructors
                0,       // flags
                ObjI::signalCount
        >;
        auto stringData = simple::make_tuple(objectInfo.name, StaticString<1>(""));
        auto methods = generateMethods<paramIndex>(stringData , objectInfo.methods);
        auto properties = generateProperties(methods.first , objectInfo.properties);
        auto enums = generateEnums<enumValueOffset>(properties.first, objectInfo.enums);
        auto constructors = generateMethods<constructorParamIndex>(enums.first, objectInfo.constructors);
        auto parametters = generateMethodsParameters(constructors.first, objectInfo.methods);
        auto parametters2 = generateConstructorParameters(parametters.first, objectInfo.constructors);
        auto enumValues = generateEnumsValues(parametters2.first, objectInfo.enums);
        return std::make_pair(enumValues.first,  header()  + methods.second + properties.second
            + enums.second + constructors.second + parametters.second + parametters2.second
            + enumValues.second);
    }




    /**
     * Holder for the string data.  Just like in the moc generated code.
     */
    template<int N, int L> struct qt_meta_stringdata_t {
         QByteArrayData data[N];
         char stringdata[L];
    };

    /** Builds the string data
     * \param S: a index_sequence that goes from 0 to the full size of the strings
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
        return BuildStringDataHelper<simple::make_index_sequence<sums(N...)>,
                                     simple::make_index_sequence<sizeof...(N)>,
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
        constexpr int methodCount = Creator::objectInfo.methodCount;
        if (_id < methodCount)
            T::qt_static_metacall(_o, _c, _id, _a);
        _id -= methodCount;
    } else if ((_c >= QMetaObject::ReadProperty && _c <= QMetaObject::QueryPropertyUser)
                || _c == QMetaObject::RegisterPropertyMetaType) {
        constexpr int propertyCount = Creator::objectInfo.propertyCount;
        if (_id < propertyCount)
            T::qt_static_metacall(_o, _c, _id, _a);
        _id -= propertyCount;
    }
    return _id;
}



/**
 * Helper for QMetaObject::IndexOfMethod
 */
template<typename T, int I>
static int indexOfMethod(void **func) {
    constexpr auto f = simple::get<I>(T::MetaObjectCreatorHelper::objectInfo.methods).func;
    using Ms = decltype(T::MetaObjectCreatorHelper::objectInfo.methods);
    if ((simple::tuple_element_t<I,Ms>::flags & 0xc) == W_MethodType::Signal.value
        && f == *reinterpret_cast<decltype(f)*>(func))
        return I;
    return -1;
}

template <typename T, int I>
static void invokeMethod(T *_o, int _id, void **_a) {
    if (_id == I) {
        constexpr auto f = simple::get<I>(T::MetaObjectCreatorHelper::objectInfo.methods).func;
        using P = QtPrivate::FunctionPointer<std::remove_const_t<decltype(f)>>;
        P::template call<typename P::Arguments, typename P::ReturnType>(f, _o, _a);
    }
}

template <typename T, int I>
static void registerMethodArgumentType(int _id, void **_a) {
    if (_id == I) {
        constexpr auto f = simple::get<I>(T::MetaObjectCreatorHelper::objectInfo.methods).func;
        using P = QtPrivate::FunctionPointer<std::remove_const_t<decltype(f)>>;
        auto _t = QtPrivate::ConnectionTypes<typename P::Arguments>::types();
        uint arg = *reinterpret_cast<int*>(_a[1]);
        *reinterpret_cast<int*>(_a[0]) = _t && arg < P::ArgumentCount ?
                _t[arg] : -1;
    }
}

template<typename T, int I>
static void propertyOp(T *_o, QMetaObject::Call _c, int _id, void **_a) {
    if (_id != I)
        return;
    constexpr auto p = simple::get<I>(T::MetaObjectCreatorHelper::objectInfo.properties);
    using Type = typename decltype(p)::PropertyType;
    switch(+_c) {
        case QMetaObject::ReadProperty:
            if (p.getter) {
                *reinterpret_cast<Type*>(_a[0]) = (_o->*(p.getter))();
            } else if (p.member) {
                *reinterpret_cast<Type*>(_a[0]) = _o->*(p.member);
            }
            break;
        case QMetaObject::WriteProperty:
            if (p.setter) {
                (_o->*(p.setter))(*reinterpret_cast<Type*>(_a[0]));
            } else if (p.member) {
                _o->*(p.member) = *reinterpret_cast<Type*>(_a[0]);
            }
    }
}


/**
 * helper for QMetaObject::createInstance
 */
template<typename T, int I>
static void createInstance(int _id, void** _a) {
    if (_id == I) {
        constexpr auto m = simple::get<I>(T::MetaObjectCreatorHelper::objectInfo.constructors);
        m.template createInstance<T>(_a, simple::make_index_sequence<decltype(m)::argCount>{});
    }
}



template<typename...Ts> static constexpr void nop(Ts...) {}

template<typename T, size_t...MethI, size_t ...ConsI, size_t...PropI>
static void qt_static_metacall_impl(QObject *_o, QMetaObject::Call _c, int _id, void** _a,
                        std::index_sequence<MethI...>, std::index_sequence<ConsI...>, std::index_sequence<PropI...>) {
    Q_UNUSED(_id)
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(T::staticMetaObject.cast(_o));
        nop((invokeMethod<T, MethI>(static_cast<T*>(_o), _id, _a),0)...);
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        nop((registerMethodArgumentType<T,MethI>(_id, _a),0)...);
    } else if (_c == QMetaObject::IndexOfMethod) {
        *reinterpret_cast<int *>(_a[0]) = sums((1+indexOfMethod<T,MethI>(reinterpret_cast<void **>(_a[1])))...)-1;
    } else if (_c == QMetaObject::CreateInstance) {
        nop((createInstance<T, ConsI>(_id, _a),0)...);
    } else if ((_c >= QMetaObject::ReadProperty && _c <= QMetaObject::QueryPropertyUser)
            || _c == QMetaObject::RegisterPropertyMetaType) {
        nop((propertyOp<T,PropI>(static_cast<T*>(_o), _c, _id, _a),0)...);
    }
}

};

template<typename T> constexpr auto createMetaObject() {  return FriendHelper2::createMetaObject<T>(); }
template<typename T, typename... Ts> auto qt_metacall_impl(Ts &&...args)
{  return FriendHelper2::qt_metacall_impl<T>(std::forward<Ts>(args)...); }
template<typename T, typename... Ts> auto qt_static_metacall_impl(Ts &&... args)
{
    using ObjI = decltype(T::MetaObjectCreatorHelper::objectInfo);
    return FriendHelper2::qt_static_metacall_impl<T>(std::forward<Ts>(args)...,
                                                     simple::make_index_sequence<ObjI::methodCount>{},
                                                     simple::make_index_sequence<ObjI::constructorCount>{},
                                                     simple::make_index_sequence<ObjI::propertyCount>{});
}


#define W_OBJECT_IMPL(TYPE) \
    struct TYPE::MetaObjectCreatorHelper { \
        static constexpr auto objectInfo = MetaObjectBuilder::makeObjectInfo<TYPE>(#TYPE); \
        static constexpr auto data = generateDataArray(objectInfo); \
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

