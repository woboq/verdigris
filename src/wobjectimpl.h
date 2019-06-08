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
 *  License along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "wobjectdefs.h"
#include <QtCore/qobject.h>

namespace w_internal {

template<class T, size_t N> using RawArray = T[N];

// Match MetaDataFlags constants form the MetaDataFlags in qmetaobject_p.h
enum : uint { IsUnresolvedType = 0x80000000, IsUnresolvedNotifySignal = 0x70000000 };

struct LayoutBuilder {
    size_t stringSize{};
    uint stringCount{};
    uint intCount{};

    constexpr void addString(const StringView& s) {
        stringSize += s.size();
        stringCount += 1;
        intCount += 1;
    }
    constexpr void addStringUntracked(const StringView& s) {
        stringSize += s.size();
        stringCount += 1;
    }
    template<uint Flag = IsUnresolvedType>
    constexpr void addTypeString(const StringView& s) {
        stringSize += s.size();
        stringCount += 1;
        intCount += 1;
    }
    template<class... Ts>
    constexpr void addInts(Ts...) {
        intCount += sizeof... (Ts);
    }
};
template<size_t StringSize, uint StringCount, uint IntCount>
struct DataHolder {
    static constexpr auto stringSize = StringSize;
    static constexpr auto stringCount = StringCount;
    static constexpr auto stringSequence = make_index_sequence<StringCount>{};
    static constexpr auto intCount = IntCount;
    RawArray<char, StringSize> stringChars{};
    RawArray<qptrdiff, StringCount> stringOffsets{};
    RawArray<int, StringCount> stringSizes{};
    RawArray<uint, IntCount> ints{};
};
struct DataBuilder {
    char* scp{};
    qptrdiff* sop{};
    int* ssp{};
    uint* ip{};
    uint stringCount{};
    uint intCount{};
    qptrdiff so{};

    constexpr DataBuilder() = default;
    DataBuilder(const DataBuilder&) = delete;
    template<size_t StringSize, uint StringCount, uint IntCount>
    constexpr DataBuilder(DataHolder<StringSize, StringCount, IntCount>& r)
        : scp(r.stringChars)
        , sop(r.stringOffsets)
        , ssp(r.stringSizes)
        , ip(r.ints) {}

    constexpr void addString(const StringView& s) {
        for (auto c : s) *scp++ = c;
        *sop++ = so;
        *ssp++ = s.size();
        *ip++ = stringCount;
        so += s.size();
        stringCount += 1;
        intCount += 1;
    }
    constexpr void addStringUntracked(const StringView& s) {
        for (auto c : s) *scp++ = c;
        *sop++ = so;
        *ssp++ = s.size();
        so += s.size();
        stringCount += 1;
    }

    template<uint Flag = IsUnresolvedType>
    constexpr void addTypeString(const StringView& s) {
        for (auto c : s) *scp++ = c;
        *sop++ = so;
        *ssp++ = s.size();
        *ip++ = Flag | stringCount;
        so += s.size();
        stringCount += 1;
        intCount += 1;
    }
    template<class... Ts>
    constexpr void addInts(Ts... vs) {
        ((*ip++ = vs),...);
        intCount += sizeof... (Ts);
    }
};


template<class F, size_t... Is>
constexpr auto fold(index_sequence<Is...>, F&& f) {
    (void)f;
    (f(index<Is>), ...);
}

template <size_t L, class State, class TPP, class F>
constexpr auto foldState(F&& f) {
    fold(make_index_sequence<stateCount<L, State, TPP>>{}, [&](auto i) { f(w_state(i, State{}, TPP{}), i); });
}

template <size_t L, class T, class F>
constexpr auto foldMethods(F&& f) {
    foldState<L, SignalStateTag, T>(f);
    foldState<L, SlotStateTag, T>(f);
    foldState<L, MethodStateTag, T>(f);
}

/// Helper to get information about the notify signal of the property within object T
template<size_t L, size_t PropIdx, typename T, typename O>
struct ResolveNotifySignal {
private:
    using TP = T**;
    using OP = O**;
    static constexpr auto prop = w_state(index<PropIdx>, PropertyStateTag{}, TP{});

    template<size_t SigIdx, bool Eq = w_state(index<SigIdx>, SignalStateTag{}, OP{}).func == prop.notify>
    static constexpr auto match(int) { return Eq; }
    template<size_t SigIdx>
    static constexpr auto match(float) { return false; }

    template<size_t... Is>
    static constexpr int indexFold(index_sequence<Is...>) {
        int r = -1;
        ((match<Is>(0) ? r = (int)Is : 0), ...);
        return r;
    }
public:
    static constexpr int signalIndex() {
        return indexFold(make_index_sequence<stateCount<L, SignalStateTag, O**>>{});
    }
};

/// returns true if the object T has at least one property with a notify signal
template <size_t L, typename TPP>
static constexpr bool hasNotifySignal() {
    auto r = bool{};
    foldState<L, PropertyStateTag, TPP>([&](auto p, auto) {
        r = r || !std::is_same<decltype(p.notify), std::nullptr_t>::value;
    });
    return r;
}

/// Holds information about a class, including all the properties and methods
template<size_t L, class T, class Name>
struct ObjectInfo {
    using Type = T;
    static constexpr auto counter = L;
    static constexpr auto name = StringView{Name::value};

    static constexpr int signalCount = stateCount<L, SignalStateTag, T**>;
    static constexpr int slotCount = stateCount<L, SlotStateTag, T**>;
    static constexpr int methodCount = signalCount + slotCount + stateCount<L, MethodStateTag, T**>;
    static constexpr int constructorCount = stateCount<L, ConstructorStateTag, T**>;
    static constexpr int propertyCount = stateCount<L, PropertyStateTag, T**>;
    static constexpr int enumCount = stateCount<L, EnumStateTag, T**>;
    static constexpr int classInfoCount = stateCount<L, ClassInfoStateTag, T**>;
    static constexpr int interfaceCount = stateCount<L, InterfaceStateTag, T**>;

    static constexpr auto methodSequence = make_index_sequence<methodCount>{};
    static constexpr auto constructorSequence = make_index_sequence<constructorCount>{};
    static constexpr auto propertySequence = make_index_sequence<propertyCount>{};
    static constexpr auto interfaceSequence = make_index_sequence<interfaceCount>{};

    template<size_t Idx>
    static constexpr auto method(Index<Idx>) {
        using TPP = T**;
        if constexpr (Idx < signalCount) return w_state(index<Idx>, SignalStateTag{}, TPP{});
        else if constexpr (Idx - signalCount < slotCount) return w_state(index<Idx - signalCount>, SlotStateTag{}, TPP{});
        else return w_state(index<Idx - signalCount - slotCount>, MethodStateTag{}, TPP{});
    }
};

template<class State>
struct ClassInfoGenerator {
    State& s;
    constexpr ClassInfoGenerator(State& s) : s(s) {}

    template<class ClassInfo, class Index>
    constexpr void operator() (const ClassInfo& ci, Index) {
        s.addString(ci.first);
        s.addString(ci.second);
    }
};

/// auto-detect the access specifiers
template <typename T, typename M, typename = void> struct isPublic : std::false_type {};
template <typename T, typename M> struct isPublic<T, M, decltype(T::w_GetAccessSpecifierHelper(std::declval<M>()))> : std::true_type {};
template <typename T, typename M, typename = void> struct isProtected : std::false_type {};
template <typename T, typename = std::enable_if_t<!std::is_final<T>::value>>
struct Derived : T { template<typename M, typename X = T> static decltype(X::w_GetAccessSpecifierHelper(std::declval<M>())) test(M); };
template <typename T, typename M> struct isProtected<T, M, decltype(Derived<T>::test(std::declval<M>()))> : std::true_type {};

template<class State, class T>
struct MethodGenerator {
    State& s;
    int parameterIndex;
    MethodGenerator(const MethodGenerator&) = delete;
    constexpr MethodGenerator(State& s, int pi) : s(s), parameterIndex(pi) {}

    template<class Method, class Index>
    constexpr void operator() (const Method& method, Index) {
        s.addString(method.name); // name
        s.addInts((uint)Method::argCount,
                   parameterIndex, //parameters
                   1, //tag, always \0
                   adjustFlags(Method::flags, typename Method::IntegralConstant()));
        parameterIndex += 1 + Method::argCount * 2;
    }

private:
    template<typename M>
    static constexpr uint adjustFlags(uint f, M) {
        if (!(f & (W_Access::Protected.value | W_Access::Private.value | W_Access::Public.value))) {
            // Auto-detect the access specifier
            f |= isPublic<T, M>::value ? W_Access::Public.value : isProtected<T,M>::value ? W_Access::Protected.value : W_Access::Private.value;
        }
        return f & static_cast<uint>(~W_Access::Private.value); // Because QMetaMethod::Private is 0, but not W_Access::Private;
    }
};

/// compute if type T is a builtin QMetaType
template <typename T, typename = void> struct MetaTypeIdIsBuiltIn : std::false_type {};
template <typename T> struct MetaTypeIdIsBuiltIn<T, typename std::enable_if<QMetaTypeId2<T>::IsBuiltIn>::type> : std::true_type{};

/// Helper to generate the type information of type 'T':
/// If T is a builtin QMetaType, its meta type id need to be added in the state.
/// If it's not builtin, then it would be an reference to T's name.
template<typename T, typename State, typename TypeStr = int>
static constexpr auto handleType(State& s, TypeStr v = {}) {
    (void)v;
    if constexpr (MetaTypeIdIsBuiltIn<T>::value) {
        return s.addInts(QMetaTypeId2<T>::MetaType);
    }
    else if constexpr (std::is_same_v<std::decay_t<TypeStr>, StringView>) {
        return s.addTypeString(v);
    }
    else {
        return s.addTypeString(W_TypeRegistery<T>::name);
        static_assert(W_TypeRegistery<T>::registered, "Please Register T with W_REGISTER_ARGTYPE");
    }
}

template<class State, class T>
struct PropertyGenerator {
    State& s;
    constexpr PropertyGenerator(State& s) : s(s) {}

    template<class Prop, class Index>
    constexpr void operator() (const Prop& prop, Index) {
        s.addString(prop.name);
        handleType<typename Prop::PropertyType>(s, prop.typeStr);
        constexpr uint moreFlags = (QtPrivate::IsQEnumHelper<typename Prop::PropertyType>::Value
                                        ? uint(PropertyFlags::EnumOrFlag) : 0);
        constexpr uint finalFlag = std::is_final<T>::value ? 0 | PropertyFlags::Final : 0;
        constexpr uint defaultFlags = 0 | PropertyFlags::Stored | PropertyFlags::Scriptable
            | PropertyFlags::Designable;
        s.addInts(Prop::flags | moreFlags | finalFlag | defaultFlags);
    }
};

template<class State, size_t L, class T, bool hasNotify>
struct NotifySignalGenerator {
    using TP = T**;
    State& s;
    constexpr NotifySignalGenerator(State& s) : s(s) {}

    template<class Prop, size_t Idx>
    constexpr void operator() (const Prop& prop, Index<Idx>) {
        if constexpr (hasNotify) {
            process(prop.notify, index<Idx>);
        }
    }

private:
    template<size_t Idx>
    constexpr void process(Empty, Index<Idx>) {
        s.addInts(0);
    }

    // Signal in the same class
    template<size_t Idx, typename Func>
    constexpr void process(Func, Index<Idx>, std::enable_if_t<std::is_same<T, typename QtPrivate::FunctionPointer<Func>::Object>::value, int> = 0) {
        constexpr int signalIndex = ResolveNotifySignal<L, Idx, T, T>::signalIndex();
        static_assert(signalIndex >= 0, "NOTIFY signal in parent class not registered as a W_SIGNAL");
        s.addInts(signalIndex);
    }

    // Signal in a parent class
    template<size_t Idx, typename Func>
    constexpr void process(Func, Index<Idx>, std::enable_if_t<!std::is_same<T, typename QtPrivate::FunctionPointer<Func>::Object>::value, int> = 0) {
        using O = typename QtPrivate::FunctionPointer<Func>::Object;
        using OP = O**;
        constexpr int signalIndex = ResolveNotifySignal<L, Idx, T, O>::signalIndex();
        static_assert(signalIndex >= 0, "NOTIFY signal in parent class not registered as a W_SIGNAL");
        static_assert(signalIndex < 0 || QT_VERSION >= QT_VERSION_CHECK(5, 10, 0),
                      "NOTIFY signal in parent class requires Qt 5.10");
        constexpr auto sig = w_state(index<signalIndex>, SignalStateTag{}, OP{});
        s.template addTypeString<IsUnresolvedNotifySignal>(sig.name);
    }
};

template<class State>
struct EnumGenerator {
    State& s;
    int dataIndex{};
    EnumGenerator(const EnumGenerator&) = delete;
    constexpr EnumGenerator(State& s, int di) : s(s), dataIndex(di) {}

    template<class Enum, class Index>
    constexpr void operator() (const Enum& e, Index) {
        auto nameIndex = s.stringCount; // required for MSVC-2019
        (void)nameIndex;
        s.addString(e.name); // name
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        if constexpr (Enum::hasAlias) s.addString(e.alias); // alias
        else s.addInts(nameIndex);
#endif
        s.addInts(Enum::flags, (uint)Enum::count, dataIndex);
        dataIndex += Enum::count * 2;
    }
};

template<class State>
struct EnumValuesGenerator {
    State& s;
    constexpr EnumValuesGenerator(State& s) : s(s) {}

    template<class Enum, class Index>
    constexpr void operator() (const Enum& e, Index) {
        generateAll(typename Enum::Values{}, e.names, Enum::sequence);
    }

private:
    template<size_t... Values, typename Names, size_t... Is>
    constexpr void generateAll(index_sequence<Values...>, const Names& names, index_sequence<Is...>) {
        ((s.addString(names[Is]), s.addInts((uint)Values)), ...);
    }
};


template<size_t I, size_t N>
constexpr auto stringFetch(const StringViewArray<N>& s) {
    if constexpr (I < N) {
        return s[I];
    }
    else {
        (void)s;
        struct _{};
        return _{};
    }
}
template<class Arg, class State, class TypeName>
constexpr void handleArgType(State& ss, const TypeName& typeName) {
    using Type = typename QtPrivate::RemoveConstRef<Arg>::Type;
    // This way, the overload of result will not pick the StringView one if it is a tuple (because registered types have the priority)
    auto typeName2 = std::conditional_t<std::is_same<Arg, Type>::value, TypeName, std::tuple<TypeName>>{typeName};
    handleType<Type>(ss, typeName2);
}
template<class... Args, class State, class TypeNames, size_t... Is>
constexpr void handleArgTypes(State& ss, const TypeNames& typeNames, index_sequence<Is...>) {
    (handleArgType<Args>(ss, stringFetch<Is>(typeNames)), ...);
}

template<size_t ArgCount, class State, size_t NameCount>
constexpr void handleArgNames(State& ss, const StringViewArray<NameCount>& paramNames) {
    auto i = size_t{};
    for (; i < ArgCount && i < NameCount; ++i) ss.addString(paramNames[i]);
    for (; i < ArgCount; ++i) ss.addInts(1);
}

template<class State>
struct MethodParametersGenerator {
    State& s;
    constexpr MethodParametersGenerator(State& s) : s(s) {}
    template<class Method, class Index>
    constexpr void operator() (const Method& method, Index) {
        generateSingleMethodParameter(method.func, method);
    }

private:
    // non-const function
    template<typename Method, typename Obj, typename Ret, typename... Args>
    constexpr void generateSingleMethodParameter(Ret (Obj::*)(Args...), const Method &method) {
        handleType<Ret>(s);
        handleArgTypes<Args...>(s, method.paramTypes, method.argSequence);
        handleArgNames<method.argCount>(s, method.paramNames);
    }
    template<typename Method, typename Obj, typename Ret, typename... Args>
    // const function
    constexpr void generateSingleMethodParameter(Ret (Obj::*)(Args...) const, const Method &method) {
        handleType<Ret>(s);
        handleArgTypes<Args...>(s, method.paramTypes, method.argSequence);
        handleArgNames<method.argCount>(s, method.paramNames);
    }
    // static member functions
    template<typename Method, typename Ret, typename... Args>
    constexpr void generateSingleMethodParameter(Ret (*)(Args...), const Method &method) {
        handleType<Ret>(s);
        handleArgTypes<Args...>(s, method.paramTypes, method.argSequence);
        handleArgNames<method.argCount>(s, method.paramNames);
    }
};

template<class State>
struct ConstructorParametersGenerator {
    State& s;
    constexpr ConstructorParametersGenerator(State& s) : s(s) {}

    template<typename... Args, class Index>
    constexpr void operator() (const MetaConstructorInfo<Args...>& info, Index) {
        s.addInts(IsUnresolvedType | 1);
        handleArgTypes<Args...>(s, StringViewArray<>{}, info.argSequence);
        s.addInts(((void)sizeof(Args),1)...); // all the names are 1 (for "\0")
    }
};

/// Given method, a binary::tree containing information about methods or constructor,
/// return the amount of item it will add in the int array.
template<size_t L, class T>
constexpr int methodsParamOffset() {
    auto sum = int{};
    foldMethods<L, T>([&](auto m, auto) { sum += int(1 + m.argCount * 2); });
    return sum;
}
template<size_t L, class T>
constexpr int constructorParamOffset() {
    auto sum = int{};
    foldState<L, ConstructorStateTag, T>([&](auto m, auto) { sum += int(1 + m.argCount * 2); });
    return sum;
}

/// Transform ObjI into State
template<typename ObjI, typename State>
constexpr void generateDataPass(State& state) {
    using T = typename ObjI::Type;
    constexpr size_t L = ObjI::counter;
    constexpr bool hasNotify = hasNotifySignal<L, T**>();
    constexpr int classInfoOffset = 14;
    constexpr int methodOffset = classInfoOffset + ObjI::classInfoCount * 2;
    constexpr int propertyOffset = methodOffset + ObjI::methodCount * 5;
    constexpr int enumOffset = propertyOffset + ObjI::propertyCount * (hasNotify ? 4: 3);
    constexpr int constructorOffset = enumOffset + ObjI::enumCount * (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) ? 5 : 4);
    constexpr int paramIndex = constructorOffset + ObjI::constructorCount * 5 ;
    constexpr int constructorParamIndex = paramIndex + methodsParamOffset<L, T**>();
    constexpr int enumValueOffset = constructorParamIndex + constructorParamOffset<L, T**>();

    state.addInts(QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) ? 8 : 7); // revision
    state.addString(ObjI::name);
    state.addStringUntracked(viewLiteral(""));
    state.addInts(
        ObjI::classInfoCount, classInfoOffset, // classinfo
        ObjI::methodCount, methodOffset, // methods
        ObjI::propertyCount, propertyOffset, // properties
        ObjI::enumCount, enumOffset, // enums/sets
        ObjI::constructorCount, constructorOffset, // constructors
        0x4 /* PropertyAccessInStaticMetaCall */,   // flags
        ObjI::signalCount);

    //if (state.intCount != classInfoOffset) throw "offset mismatch!";
    foldState<L, ClassInfoStateTag, T**>(ClassInfoGenerator<State>{state});

    //if (state.intCount != methodOffset) throw "offset mismatch!";
    foldMethods<L, T**>(MethodGenerator<State, T>{state, paramIndex});

    //if (state.intCount != propertyOffset) throw "offset mismatch!";
    foldState<L, PropertyStateTag, T**>(PropertyGenerator<State, T>{state});
    foldState<L, PropertyStateTag, T**>(NotifySignalGenerator<State, L, T, hasNotify>{state});

    //if (state.intCount != enumOffset) throw "offset mismatch!";
    foldState<L, EnumStateTag, T**>(EnumGenerator<State>{state, enumValueOffset});

    //if (state.intCount != constructorOffset) throw "offset mismatch!";
    foldState<L, ConstructorStateTag, T**>(MethodGenerator<State, T>{state, constructorParamIndex});

    //if (state.intCount != paramIndex) throw "offset mismatch!";
    foldMethods<L, T**>(MethodParametersGenerator<State>{state});

    //if (state.intCount != constructorParamIndex) throw "offset mismatch!";
    foldState<L, ConstructorStateTag, T**>(ConstructorParametersGenerator<State>{state});

    //if (state.intCount != enumValueOffset) throw "offset mismatch!";
    foldState<L, EnumStateTag, T**>(EnumValuesGenerator<State>{state});
}
template<typename ObjI>
constexpr auto generateDataArray() {
    constexpr LayoutBuilder layout = [] {
        auto r = LayoutBuilder{};
        generateDataPass<ObjI>(r);
        return r;
    }();
    auto r = DataHolder<layout.stringSize, layout.stringCount, layout.intCount>{};
    auto b = DataBuilder{r};
    generateDataPass<ObjI>(b);
    return r;
}
template<typename ObjI>
constexpr auto dataArray = generateDataArray<ObjI>();

template<size_t N>
struct OwnString {
    char data[N];

    constexpr OwnString(const char (&s)[N]) {
        auto p = data;
        for (auto c : s) *p++ = c;
    }
};

/// Holder for the string data.  Just like in the moc generated code.
template<std::size_t N, std::size_t L> struct qt_meta_stringdata_t {
     QByteArrayData data[N];
     OwnString<L> stringdata;
};

/// Builds the string data
/// \tparam T the W_MetaObjectCreatorHelper
/// \tparam I an index_sequence that goes from 0 to the number of string
template<typename ObjI, std::size_t... I>
struct BuildStringDataHelper {
    static constexpr auto& data = dataArray<ObjI>;
    using meta_stringdata_t = const qt_meta_stringdata_t<data.stringCount, data.stringSize>;
#ifndef Q_CC_MSVC
    static constexpr qptrdiff stringdata_offset = offsetof(meta_stringdata_t, stringdata);
#else // offsetof does not work with MSVC
    static constexpr qptrdiff stringdata_offset = sizeof(meta_stringdata_t::data);
#endif
    inline static meta_stringdata_t qt_meta_stringdata = {
        {Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(
            data.stringSizes[I]-1,
            static_cast<qptrdiff>(stringdata_offset + data.stringOffsets[I] - I * sizeof(QByteArrayData)))
        ...}, {data.stringChars}
    };
};

/// returns the string data suitable for the QMetaObject from a list of string
/// T is W_MetaObjectCreatorHelper<ObjectType>
template<typename ObjI, size_t... Is>
constexpr const QByteArrayData *build_string_data(index_sequence<Is...>)  {
    return BuildStringDataHelper<ObjI, Is...>::qt_meta_stringdata.data;
}

/// Returns the QMetaObject* of the base type. Use SFINAE to only return it if it exists
template<typename T>
static constexpr auto parentMetaObject(int) W_RETURN(&T::W_BaseType::staticMetaObject)
template<typename T>
static constexpr const QMetaObject *parentMetaObject(...) { return nullptr; }

// Bunch of helpers for propertyOperation
// this uses SFINAE of the return value to work either with F a pointer to member, or to member function
// or nullptr.
template <typename F, typename O, typename T>
inline auto propSet(F f, O *o, const T &t) W_RETURN(((o->*f)(t),0))
template <typename F, typename O, typename T>
inline auto propSet(F f, O *o, const T &t) W_RETURN(o->*f = t)
template <typename O, typename T>
inline void propSet(Empty, O *, const T &) {}

template <typename F, typename O, typename T>
inline auto propGet(F f, O *o, T &t) W_RETURN(t = (o->*f)())
template <typename F, typename O, typename T>
inline auto propGet(F f, O *o, T &t) W_RETURN(t = o->*f)
template <typename O, typename T>
inline void propGet(Empty, O *, T &) {}

template <typename F, typename M, typename O>
inline auto propNotify(F f, M m, O *o) W_RETURN(((o->*f)(o->*m),0))
template <typename F, typename M, typename O>
inline auto propNotify(F f, M, O *o) W_RETURN(((o->*f)(),0))
template <typename... T>
inline void propNotify(T...) {}

template <typename F, typename O>
inline auto propReset(F f, O *o) W_RETURN(((o->*f)(),0))
template <typename... T>
inline void propReset(T...) {}

struct FriendHelper {

    template<typename T>
    static constexpr QMetaObject createMetaObject()
    {
        using ObjI = typename T::W_MetaObjectCreatorHelper::ObjectInfo;
        constexpr auto& data = dataArray<ObjI>;
        auto string_data = build_string_data<ObjI>(data.stringSequence);
        return { { parentMetaObject<T>(0), string_data, data.ints, T::qt_static_metacall, {}, {} } };
    }

    template<typename T> static int qt_metacall_impl(T *_o, QMetaObject::Call _c, int _id, void** _a) {
        using ObjI = typename T::W_MetaObjectCreatorHelper::ObjectInfo;
        _id = _o->T::W_BaseType::qt_metacall(_c, _id, _a);
        if (_id < 0)
            return _id;
        if (_c == QMetaObject::InvokeMetaMethod || _c == QMetaObject::RegisterMethodArgumentMetaType) {
            constexpr int methodCount = ObjI::methodCount;
            if (_id < methodCount)
                T::qt_static_metacall(_o, _c, _id, _a);
            _id -= methodCount;
        } else if ((_c >= QMetaObject::ReadProperty && _c <= QMetaObject::QueryPropertyUser)
                    || _c == QMetaObject::RegisterPropertyMetaType) {
            constexpr int propertyCount = ObjI::propertyCount;
            if (_id < propertyCount)
                T::qt_static_metacall(_o, _c, _id, _a);
            _id -= propertyCount;
        }
        return _id;
    }

    /// Helper for implementation of qt_static_metacall for QMetaObject::IndexOfMethod
    /// T is the class, and I is the index of a method.
    /// Returns I if the argument is equal to the pointer to member function of the signal of index 'I'
    template<typename T, int I>
    static int indexOfMethod(void **func) {
        using ObjI = typename T::W_MetaObjectCreatorHelper::ObjectInfo;
        constexpr auto m = ObjI::method(index<I>);
        if ((m.flags & 0xc) == W_MethodType::Signal.value
            && m.func == *reinterpret_cast<decltype(m.func)*>(func))
            return I;
        return -1;
    }

    /// Helper for implementation of qt_static_metacall for QMetaObject::InvokeMetaMethod
    /// T is the class, and I is the index of a method.
    /// Invoke the method with index I if id == I.
    template <typename T, int I>
    static void invokeMethod(T *_o, int _id, void **_a) {
        if (_id == I) {
            using ObjI = typename T::W_MetaObjectCreatorHelper::ObjectInfo;
            constexpr auto f = ObjI::method(index<I>).func;
            using P = QtPrivate::FunctionPointer<std::remove_const_t<decltype(f)>>;
            P::template call<typename P::Arguments, typename P::ReturnType>(f, _o, _a);
        }
    }

    /// Helper for implementation of qt_static_metacall for QMetaObject::RegisterMethodArgumentMetaType
    /// T is the class, and I is the index of a method.
    template <typename T, int I>
    static void registerMethodArgumentType(int _id, void **_a) {
        if (_id == I) {
            using ObjI = typename T::W_MetaObjectCreatorHelper::ObjectInfo;
            constexpr auto f = ObjI::method(index<I>).func;
            using P = QtPrivate::FunctionPointer<std::remove_const_t<decltype(f)>>;
            auto _t = QtPrivate::ConnectionTypes<typename P::Arguments>::types();
            uint arg = *reinterpret_cast<uint*>(_a[1]);
            *reinterpret_cast<int*>(_a[0]) = _t && arg < P::ArgumentCount ?
                    _t[arg] : -1;
        }
    }

    /// Helper for implementation of qt_static_metacall for any of the operations in a property
    /// T is the class, and I is the index of a property.
    template<typename T, int I>
    static void propertyOperation(T *_o, QMetaObject::Call _c, int _id, void **_a) {
        if (_id != I)
            return;
        using TPP = T**;
        constexpr auto p = w_state(index<I>, PropertyStateTag{}, TPP{});
        using Type = typename decltype(p)::PropertyType;
        switch(+_c) {
        case QMetaObject::ReadProperty:
            if (p.getter) {
                propGet(p.getter, _o, *reinterpret_cast<Type*>(_a[0]));
            } else if (p.member) {
                propGet(p.member, _o, *reinterpret_cast<Type*>(_a[0]));
            }
            break;
        case QMetaObject::WriteProperty:
            if (p.setter) {
                propSet(p.setter, _o, *reinterpret_cast<Type*>(_a[0]));
            } else if (p.member) {
                propSet(p.member, _o, *reinterpret_cast<Type*>(_a[0]));
                propNotify(p.notify, p.member, _o);
            }
            break;
        case QMetaObject::ResetProperty:
            if (p.reset) {
                propReset(p.reset, _o);
            }
            break;
        case QMetaObject::RegisterPropertyMetaType:
            *reinterpret_cast<int*>(_a[0]) = QtPrivate::QMetaTypeIdHelper<Type>::qt_metatype_id();
        }
    }

    /// Helper for implementation of qt_static_metacall for QMetaObject::CreateInstance
    /// T is the class, and I is the index of a constructor.
    template<typename T, class... Args, std::size_t... I>
    static void createInstanceImpl(void **_a, const MetaConstructorInfo<Args...>&, index_sequence<I...>) {
        *reinterpret_cast<T**>(_a[0]) =
            new T(*reinterpret_cast<std::remove_reference_t<Args> *>(_a[I+1])...);
    }
    template<typename T, int I>
    static void createInstance(int _id, void** _a) {
        if (_id == I) {
            using TPP = T**;
            constexpr auto m = w_state(index<I>, ConstructorStateTag{}, TPP{});
            createInstanceImpl<T>(_a, m, m.argSequence);
        }
    }

    /// Implementation of qt_static_metacall for W_OBJECT_IMPL which should be called with
    /// std::index_sequence for the amount of method, constructor, and properties.
    template<typename T, size_t...MethI, size_t ...ConsI, size_t...PropI>
    static void qt_static_metacall_impl2(QObject *_o, QMetaObject::Call _c, int _id, void** _a,
                            std::index_sequence<MethI...>, std::index_sequence<ConsI...>, std::index_sequence<PropI...>) {
        Q_UNUSED(_id) Q_UNUSED(_o) Q_UNUSED(_a)
        if (_c == QMetaObject::InvokeMetaMethod) {
            Q_ASSERT(T::staticMetaObject.cast(_o));
            (invokeMethod<T, MethI>(reinterpret_cast<T*>(_o), _id, _a),...);
        } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
            (registerMethodArgumentType<T,MethI>(_id, _a),...);
        } else if (_c == QMetaObject::IndexOfMethod) {
            *reinterpret_cast<int *>(_a[0]) = ((1+indexOfMethod<T,MethI>(reinterpret_cast<void **>(_a[1]))) + ... + 0)-1;
        } else if (_c == QMetaObject::CreateInstance) {
            (createInstance<T, ConsI>(_id, _a),...);
        } else if ((_c >= QMetaObject::ReadProperty && _c <= QMetaObject::QueryPropertyUser)
                || _c == QMetaObject::RegisterPropertyMetaType) {
            (propertyOperation<T,PropI>(static_cast<T*>(_o), _c, _id, _a),...);
        }
    }

    // Same for W_GADGET
    template<typename T, size_t...MethI, size_t ...ConsI, size_t...PropI>
    static void qt_static_metacall_impl2(T *_o, QMetaObject::Call _c, int _id, void** _a,
                            std::index_sequence<MethI...>, std::index_sequence<ConsI...>, std::index_sequence<PropI...>) {
        Q_UNUSED(_id) Q_UNUSED(_o) Q_UNUSED(_a)
        if (_c == QMetaObject::InvokeMetaMethod) {
            (invokeMethod<T, MethI>(_o, _id, _a),...);
        } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
            (registerMethodArgumentType<T,MethI>(_id, _a),...);
        } else if (_c == QMetaObject::IndexOfMethod) {
            Q_ASSERT_X(false, "qt_static_metacall", "IndexOfMethod called on a Q_GADGET");
        } else if (_c == QMetaObject::CreateInstance) {
            (createInstance<T, ConsI>(_id, _a),...);
        } else if ((_c >= QMetaObject::ReadProperty && _c <= QMetaObject::QueryPropertyUser)
                || _c == QMetaObject::RegisterPropertyMetaType) {
            (propertyOperation<T,PropI>(_o, _c, _id, _a),...);
        }
    }

    template<typename T, typename... Ts>
    static void qt_static_metacall_impl(Ts &&... args) {
        using ObjI = typename T::W_MetaObjectCreatorHelper::ObjectInfo;
        return qt_static_metacall_impl2<T>(std::forward<Ts>(args)...,
                                           ObjI::methodSequence, ObjI::constructorSequence, ObjI::propertySequence);
    }

    /// implementation of qt_metacast
    template<typename T>
    static void* qt_metacast_impl(T *o, const char *_clname) {
        if (!_clname)
            return nullptr;
        const QByteArrayDataPtr sd = { const_cast<QByteArrayData*>(T::staticMetaObject.d.stringdata) };
        if (_clname == QByteArray(sd))
            return o;
        using ObjI = typename T::W_MetaObjectCreatorHelper::ObjectInfo;
        void *result = {};
        auto l = [&](auto i) {
            using TPP = T**;
            using Interface = decltype(w_state(i, InterfaceStateTag{}, TPP{}));
            const char *iid = qobject_interface_iid<Interface>();
            if (iid && !strcmp(_clname, iid))
                result = static_cast<Interface>(o);
        };
        fold(ObjI::interfaceSequence, l);
        if (!result) return o->T::W_BaseType::qt_metacast(_clname);
        return result;
    }

}; // FriendHelper

} // namespace w_internal

#ifndef QT_INIT_METAOBJECT // Defined in qglobal.h since Qt 5.10
#define QT_INIT_METAOBJECT
#endif

// W_OBJECT_IMPL was designed to take advantage of the GNU extension that ... can have zero arguments.
// So we need to work around that to extract the template stuff which may not exist or be composed
// of several macro arguments: If the first argument has parentheses, there must be at least  two
// arguments, so just do a tail. Otherwise, there should be only one or two argument, so take the second.
#define W_MACRO_TEMPLATE_STUFF(...)  W_MACRO_CONCAT(W_MACRO_TEMPLATE_STUFF_HELPER, W_MACRO_DELAY(W_MACRO_TEMPLATE_STUFF_QUERY,W_MACRO_TEMPLATE_STUFF_HELPER __VA_ARGS__))(__VA_ARGS__)
#define W_MACRO_TEMPLATE_STUFF_QUERY(...) W_MACRO_DELAY2(W_MACRO_FIRST, W_MACRO_TEMPLATE_STUFF_HELPER_ ## __VA_ARGS__)
#define W_MACRO_TEMPLATE_STUFF_HELPER(...) YES
#define W_MACRO_TEMPLATE_STUFF_HELPER_YES TAIL,
#define W_MACRO_TEMPLATE_STUFF_HELPER_W_MACRO_TEMPLATE_STUFF_HELPER SECOND,
#define W_MACRO_TEMPLATE_STUFF_HELPER_TAIL(...) W_MACRO_MSVC_EXPAND(W_MACRO_TAIL(__VA_ARGS__))
#define W_MACRO_TEMPLATE_STUFF_HELPER_SECOND(...) W_MACRO_MSVC_EXPAND(W_MACRO_TEMPLATE_STUFF_HELPER_SECOND2(__VA_ARGS__,,))
#define W_MACRO_TEMPLATE_STUFF_HELPER_SECOND2(A,B,...) B
#define W_MACRO_FIRST_REMOVEPAREN(...) W_MACRO_REMOVEPAREN(W_MACRO_FIRST(__VA_ARGS__))

#define W_OBJECT_IMPL_COMMON(INLINE, ...) \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) struct W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::W_MetaObjectCreatorHelper { \
        struct Name { static constexpr auto value = w_internal::viewLiteral(W_MACRO_STRIGNIFY(W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__))); }; \
        using ObjectInfo = w_internal::ObjectInfo<1024*1024*1024, W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::W_ThisType, Name>; \
    }; \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) INLINE const QT_INIT_METAOBJECT QMetaObject W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::staticMetaObject = \
        w_internal::FriendHelper::createMetaObject<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::W_ThisType>();

/// \macro W_OBJECT_IMPL(TYPE [, TEMPLATE_STUFF])
/// This macro expand to the code that instantiate the QMetaObject. It must be placed outside of
/// the class, in the .cpp file. The TYPE argument must be the qualified name of the class,
/// including the namespace, if any.
/// Example: `W_OBJECT_IMPL(Namespace::MyClass)`
///
/// If the class is a templated class, the second argument contains the template introducer.
/// Example:  `W_OBJECT_IMPL(MyTemplate<T>, template <typename T>)`
/// Parentheses are required if there is several template arguments:
/// `W_OBJECT_IMPL((MyTemplate2<A,B>), template<typename A, typename B>)`
#define W_OBJECT_IMPL(...) \
    W_OBJECT_IMPL_COMMON(W_MACRO_EMPTY, __VA_ARGS__) \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) void W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void** _a) \
    { w_internal::FriendHelper::qt_static_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(_o, _c, _id, _a); } \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) const QMetaObject *W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::metaObject() const  { return &staticMetaObject; } \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) void *W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_metacast(const char *_clname) \
    { return w_internal::FriendHelper::qt_metacast_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(this, _clname); } \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) int W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_metacall(QMetaObject::Call _c, int _id, void** _a) \
    { return w_internal::FriendHelper::qt_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(this, _c, _id, _a); }


/// \macro W_GADGET_IMPL(TYPE [, TEMPLATE_STUFF])
/// Same as W_OBJECT_IMPL, but for a W_GADGET
#define W_GADGET_IMPL(...) \
    W_OBJECT_IMPL_COMMON(W_MACRO_EMPTY, __VA_ARGS__) \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) void W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void** _a) \
    { w_internal::FriendHelper::qt_static_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(reinterpret_cast<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)*>(_o), _c, _id, _a); }

/// \macro W_NAMESPACE_IMPL(...)
/// Same as W_OBJECT_IMPL, but for a W_NAMESPACE
#define W_NAMESPACE_IMPL(...) \
    W_OBJECT_IMPL_COMMON(W_MACRO_EMPTY, __VA_ARGS__)


/// \macro W_OBJECT_IMPL_INLINE(TYPE [, TEMPLATE_STUFF])
/// Same as W_OBJECT_IMPL, but to be used in a header
/// (Requires support for c++17 inline variables)
#define W_OBJECT_IMPL_INLINE(...) \
    W_OBJECT_IMPL_COMMON(inline, __VA_ARGS__) \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) inline void W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void** _a) \
    { w_internal::FriendHelper::qt_static_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(_o, _c, _id, _a); } \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) inline const QMetaObject *W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::metaObject() const  { return &staticMetaObject; } \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) inline void *W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_metacast(const char *_clname) \
    { return w_internal::FriendHelper::qt_metacast_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(this, _clname); } \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) inline int W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_metacall(QMetaObject::Call _c, int _id, void** _a) \
    { return w_internal::FriendHelper::qt_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(this, _c, _id, _a); }


/// \macro W_GADGET_IMPL_INLINE(TYPE [, TEMPLATE_STUFF])
/// Same as W_GADGET_IMPL, but to be used in a header
/// (Requires support for c++17 inline variables)
#define W_GADGET_IMPL_INLINE(...) \
    W_OBJECT_IMPL_COMMON(inline, __VA_ARGS__) \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) inline void W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void** _a) \
    { w_internal::FriendHelper::qt_static_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(reinterpret_cast<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)*>(_o), _c, _id, _a); }

/// \macro W_NAMESPACE_IMPL_INLINE(...)
/// Same as W_NAMESPACE_IMPL, but to be used in a header
/// (Requires support for c++17 inline variables)
#define W_NAMESPACE_IMPL_INLINE(...) \
    W_OBJECT_IMPL_COMMON(inline, __VA_ARGS__)
