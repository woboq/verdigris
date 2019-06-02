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

// Match MetaDataFlags constants form the MetaDataFlags in qmetaobject_p.h
enum : uint { IsUnresolvedType = 0x80000000, IsUnresolvedNotifySignal = 0x70000000 };

struct LayoutBuilder {
    size_t stringSize{};
    uint stringCount{};
    uint intCount{};

    template<size_t L>
    constexpr void addString(const StaticString<L>&) {
        stringSize += L;
        stringCount += 1;
        intCount += 1;
    }
    template<size_t L>
    constexpr void addStringUntracked(const StaticString<L>&) {
        stringSize += L;
        stringCount += 1;
    }
    template<uint Flag = IsUnresolvedType, std::size_t L>
    constexpr void addTypeString(const StaticString<L>&) {
        stringSize += L;
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
    RawArray<size_t, StringCount> stringOffsets{};
    RawArray<int, StringCount> stringSizes{};
    RawArray<uint, IntCount> ints{};
};
struct DataBuilder {
    char* scp{};
    size_t* sop{};
    int* ssp{};
    uint* ip{};
    uint stringCount{};
    uint intCount{};
    size_t so{};

    constexpr DataBuilder() = default;
    DataBuilder(const DataBuilder&) = delete;
    template<size_t StringSize, uint StringCount, uint IntCount>
    constexpr DataBuilder(DataHolder<StringSize, StringCount, IntCount>& r)
        : scp(r.stringChars)
        , sop(r.stringOffsets)
        , ssp(r.stringSizes)
        , ip(r.ints) {}

    template<size_t L>
    constexpr void addString(const StaticString<L>& s) {
        for (auto c : s.data) *scp++ = c;
        *sop++ = so;
        *ssp++ = L;
        *ip++ = stringCount;
        so += L;
        stringCount += 1;
        intCount += 1;
    }
    template<size_t L>
    constexpr void addStringUntracked(const StaticString<L>& s) {
        for (auto c : s.data) *scp++ = c;
        *sop++ = so;
        *ssp++ = L;
        so += L;
        stringCount += 1;
    }

    template<uint Flag = IsUnresolvedType, std::size_t L>
    constexpr void addTypeString(const StaticString<L>& s) {
        for (auto c : s.data) *scp++ = c;
        *sop++ = so;
        *ssp++ = L;
        *ip++ = Flag | stringCount;
        so += L;
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
constexpr auto toTree(index_sequence<Is...>, F f) {
    (void)f;
    if constexpr (0 == sizeof... (Is)) return binary::tree<>{};
    else if constexpr (1 == sizeof... (Is)) return (binary::tree_append(binary::tree<>{}, f(index<Is>)), ...);
    else return binary::tree_cat(binary::tree_append(binary::tree<>{}, f(index<Is>)) ...);
}

template<class F, size_t... Is>
constexpr auto fold(index_sequence<Is...>, F&& f) {
    (void)f;
    (f(index<Is>), ...);
}

template <size_t L, class ThisType, class F>
constexpr auto foldSlotState(F&& f) {
    fold(make_index_sequence<w_SlotStateCount<L, ThisType>>{}, [&](auto i) { f(w_SlotState(i, ThisType{}), i); });
}
template <size_t L, class ThisType, class F>
constexpr auto foldSignalState(F&& f) {
    fold(make_index_sequence<w_SignalStateCount<L, ThisType>>{}, [&](auto i) { f(w_SignalState(i, ThisType{}), i); });
}
template <size_t L, class ThisType, class F>
constexpr auto foldMethodState(F&& f) {
    fold(make_index_sequence<w_MethodStateCount<L, ThisType>>{}, [&](auto i) { f(w_MethodState(i, ThisType{}), i); });
}
template <size_t L, class ThisType, class F>
constexpr auto foldConstructorState(F&& f) {
    fold(make_index_sequence<w_ConstructorStateCount<L, ThisType>>{}, [&](auto i) { f(w_ConstructorState(i, ThisType{}), i); });
}
template <size_t L, class ThisType, class F>
constexpr auto foldPropertyState(F&& f) {
    fold(make_index_sequence<w_PropertyStateCount<L, ThisType>>{}, [&](auto i) { f(w_PropertyState(i, ThisType{}), i); });
}
template <size_t L, class ThisType, class F>
constexpr auto foldEnumState(F&& f) {
    fold(make_index_sequence<w_EnumStateCount<L, ThisType>>{}, [&](auto i) { f(w_EnumState(i, ThisType{}), i); });
}
template <size_t L, class ThisType, class F>
constexpr auto foldClassInfoState(F&& f) {
    fold(make_index_sequence<w_ClassInfoStateCount<L, ThisType>>{}, [&](auto i) { f(w_ClassInfoState(i, ThisType{}), i); });
}
template <size_t L, class ThisType, class F>
constexpr auto foldInterfaceState(F&& f) {
    fold(make_index_sequence<w_InterfaceStateCount<L, ThisType>>{}, [&](auto i) { f(w_InterfaceState(i, ThisType{}), i); });
}

template <size_t L, class T, class F>
constexpr auto foldMethods(F&& f) {
    foldSignalState<L, T>(f);
    foldSlotState<L, T>(f);
    foldMethodState<L, T>(f);
}

template<size_t L, class ThisType>
constexpr auto w_SlotStateTree = toTree(make_index_sequence<w_SlotStateCount<L, ThisType>>{},
                                        [](auto i) { return w_SlotState(i, ThisType{}); });

template<size_t L, class ThisType>
constexpr auto w_SignalStateTree = toTree(make_index_sequence<w_SignalStateCount<L, ThisType>>{},
                                          [](auto i) { return w_SignalState(i, ThisType{}); });

template<size_t L, class ThisType>
constexpr auto w_MethodStateTree = toTree(make_index_sequence<w_MethodStateCount<L, ThisType>>{},
                                          [](auto i) { return w_MethodState(i, ThisType{}); });

template<size_t L, class ThisType>
constexpr auto w_ConstructorStateTree = toTree(make_index_sequence<w_ConstructorStateCount<L, ThisType>>{},
                                               [](auto i) { return w_ConstructorState(i, ThisType{}); });

template<size_t L, class ThisType>
constexpr auto w_PropertyStateTree = toTree(make_index_sequence<w_PropertyStateCount<L, ThisType>>{},
                                            [](auto i) { return w_PropertyState(i, ThisType{}); });

template<size_t L, class ThisType>
constexpr auto w_EnumStateTree = toTree(make_index_sequence<w_EnumStateCount<L, ThisType>>{},
                                        [](auto i) { return w_EnumState(i, ThisType{}); });

template<size_t L, class ThisType>
constexpr auto w_ClassInfoStateTree = toTree(make_index_sequence<w_ClassInfoStateCount<L, ThisType>>{},
                                             [](auto i) { return w_ClassInfoState(i, ThisType{}); });

template<size_t L, class ThisType>
constexpr auto w_InterfaceStateTree = toTree(make_index_sequence<w_InterfaceStateCount<L, ThisType>>{},
                                             [](auto i) { return w_InterfaceState(i, ThisType{}); });

/**
 * Iterate over all the items of a tree and call the Generator::generate function
 *
 * The first parameter of the function is the IntermediateState, and it returns a new
 * InterMediateState with all the information from the tree added to it.
 *
 * The 'Ofst' template parameter is the offset in the integer array to which new data can be added.
 *
 * The Generator template type has a method generate which will be called for every element of the
 * tree with the IntermediateState. It is called with the offset as a template parameter.
 *
 * The Generator also has an offset() method that returns how much things will be added later in
 * the array for this element.
 */
template<typename Generator, int Ofst, typename State>
constexpr void generate(State&, binary::tree<>) {}

template<typename Generator, int Ofst, typename State, typename Tree>
constexpr auto generate(State& s, Tree t) {
    Generator::template generate<Ofst>(s, binary::tree_head(t));
    generate<Generator, Ofst + Generator::template offset<binary::tree_element_t<0,Tree>>()>(s, binary::tree_tail(t));
}

/** Helper to get information about the notify signal of the property within object T */
template<size_t L, size_t PropIdx, typename T, typename O>
struct ResolveNotifySignal {
private:
    using TP = T**;
    using OP = O**;
    static constexpr auto prop = w_PropertyState(index<PropIdx>, TP{});

    template<size_t SigIdx, bool Eq = w_SignalState(index<SigIdx>, OP{}).func == prop.notify>
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
        return indexFold(make_index_sequence<w_SignalStateCount<L, O**>>{});
    }
};

/** returns true if the object T has at least one property with a notify signal */
template <size_t L, typename T>
static constexpr bool hasNotifySignal() {
    auto r = bool{};
    foldPropertyState<L, T>([&](auto p, auto) {
        r = r || !std::is_same<decltype(p.notify), std::nullptr_t>::value;
    });
    return r;
}

/** Holds information about a class, including all the properties and methods */
template<size_t L, class T, class Name>
struct ObjectInfo {
    static constexpr auto name = StaticString{Name::value};
    static constexpr auto sigState = w_SignalStateTree<L,T**>;
    static constexpr auto methods = binary::tree_cat(sigState, w_SlotStateTree<L,T**>, w_MethodStateTree<L,T**>);
    static constexpr auto constructors = w_ConstructorStateTree<L, T**>;
    static constexpr auto properties = w_PropertyStateTree<L, T**>;
    static constexpr auto enums = w_EnumStateTree<L, T**>;
    static constexpr auto classInfos = w_ClassInfoStateTree<L, T**>;
    static constexpr auto interfaces = w_InterfaceStateTree<L, T**>;

    static constexpr int methodCount = methods.size;
    static constexpr int constructorCount = constructors.size;
    static constexpr int propertyCount = properties.size;
    static constexpr int enumCount = enums.size;
    static constexpr int classInfoCount = classInfos.size;
    static constexpr int interfaceCount = interfaces.size;
    static constexpr int signalCount = sigState.size;
};

// Generator for Q_CLASSINFO to be used in foldClassInfoState
template<class State>
struct ClassInfoGenerator {
    State& s;
    constexpr ClassInfoGenerator(State& s) : s(s) {}
    template<class ClassInfo, class Index>
    constexpr auto operator() (const ClassInfo& ci, Index) {
        s.addString(ci.first);
        s.addString(ci.second);
    }
};

/* Helpers to auto-detect the access specifier */
template <typename T, typename M, typename = void> struct isPublic : std::false_type {};
template <typename T, typename M> struct isPublic<T, M, decltype(T::w_GetAccessSpecifierHelper(std::declval<M>()))> : std::true_type {};
template <typename T, typename M, typename = void> struct isProtected : std::false_type {};
template <typename T, typename = std::enable_if_t<!std::is_final<T>::value>>
struct Derived : T { template<typename M, typename X = T> static decltype(X::w_GetAccessSpecifierHelper(std::declval<M>())) test(M); };
template <typename T, typename M> struct isProtected<T, M, decltype(Derived<T>::test(std::declval<M>()))> : std::true_type {};

// Generator for methods to be used in fold*
template<class State, class T>
struct MethodGenerator {
    State& s;
    int parameterIndex;
    MethodGenerator(const MethodGenerator&) = delete;
    constexpr MethodGenerator(State& s, int pi) : s(s), parameterIndex(pi) {}
    template<class Method, class Index>
    constexpr auto operator() (const Method& method, Index) {
        s.addString(method.name); // name
        s.addInts((uint)Method::argCount,
                   parameterIndex, //parameters
                   1, //tag, always \0
                   adjustFlags(Method::flags, typename Method::IntegralConstant()));
        parameterIndex += 1 + Method::argCount * 2;
    }
    template<typename M>
    static constexpr uint adjustFlags(uint f, M) {
        if (!(f & (W_Access::Protected.value | W_Access::Private.value | W_Access::Public.value))) {
            // Auto-detect the access specifier
            f |= isPublic<T, M>::value ? W_Access::Public.value : isProtected<T,M>::value ? W_Access::Protected.value : W_Access::Private.value;
        }
        return f & static_cast<uint>(~W_Access::Private.value); // Because QMetaMethod::Private is 0, but not W_Access::Private;
    }
};

/** Helper that computes if type T is a builtin QMetaType  */
template <typename T, typename = void> struct MetaTypeIdIsBuiltIn : std::false_type {};
template <typename T> struct MetaTypeIdIsBuiltIn<T, typename std::enable_if<QMetaTypeId2<T>::IsBuiltIn>::type> : std::true_type{};

/**
 * Helper to generate the type information of type 'T':
 * If T is a builtin QMetaType, its meta type id need to be added in the state.
 * If it's not builtin, then it would be an reference to T's name in the string array.
 */
template<typename T, bool Builtin = MetaTypeIdIsBuiltIn<T>::value>
struct HandleType {
    template<typename S, typename TypeStr = int>
    static constexpr auto result(S&s, TypeStr = {}) {
        return s.addInts(QMetaTypeId2<T>::MetaType);
    }
};
template<typename T>
struct HandleType<T, false> {
    template<typename Strings, typename TypeStr = int>
    static constexpr void result(Strings &ss, TypeStr = {}) {
        return ss.addTypeString(W_TypeRegistery<T>::name);
        static_assert(W_TypeRegistery<T>::registered, "Please Register T with W_REGISTER_ARGTYPE");
    }
    template<typename Strings, std::size_t N>
    static constexpr void result(Strings &ss, StaticString<N> typeStr,
                                    typename std::enable_if<(N>1),int>::type=0) {
        return ss.addTypeString(typeStr);
    }
};

// Generator for properties to be used in generate<>()
template<class State, class T>
struct PropertyGenerator {
    State& s;
    constexpr PropertyGenerator(State& s) : s(s) {}
    template<class Prop, class Index>
    constexpr auto operator() (const Prop& prop, Index) {
        s.addString(prop.name);
        HandleType<typename Prop::PropertyType>::result(s, prop.typeStr);
        constexpr uint moreFlags = (QtPrivate::IsQEnumHelper<typename Prop::PropertyType>::Value
                                        ? uint(PropertyFlags::EnumOrFlag) : 0);
        constexpr uint finalFlag = std::is_final<T>::value ? 0 | PropertyFlags::Final : 0;
        constexpr uint defaultFlags = 0 | PropertyFlags::Stored | PropertyFlags::Scriptable
            | PropertyFlags::Designable;
        s.addInts(Prop::flags | moreFlags | finalFlag | defaultFlags);
    }
};


// Generator for notify signals to be used in generate
template<class State, size_t L, class T, bool hasNotify>
struct NotifySignalGenerator {
    using TP = T**;
    State& s;
    constexpr NotifySignalGenerator(State& s) : s(s) {}
    template<class Prop, size_t Idx>
    constexpr auto operator() (const Prop& prop, Index<Idx>) {
        if constexpr (hasNotify) {
            process(prop.notify, index<Idx>);
        }
    }

    template<size_t Idx>
    constexpr auto process(Empty, Index<Idx>) {
        return s.addInts(0);
    }

    // Signal in the same class
    template<size_t Idx, typename Func>
    constexpr auto process(Func, Index<Idx>, std::enable_if_t<std::is_same<T, typename QtPrivate::FunctionPointer<Func>::Object>::value, int> = 0)
    {
        constexpr int signalIndex = ResolveNotifySignal<L, Idx, T, T>::signalIndex();
        static_assert(signalIndex >= 0, "NOTIFY signal in parent class not registered as a W_SIGNAL");
        s.addInts(signalIndex);
    }

    // Signal in a parent class
    template<size_t Idx, typename Func>
    constexpr auto process(Func, Index<Idx>, std::enable_if_t<!std::is_same<T, typename QtPrivate::FunctionPointer<Func>::Object>::value, int> = 0)
    {
        using O = typename QtPrivate::FunctionPointer<Func>::Object;
        using OP = O**;
        constexpr int signalIndex = ResolveNotifySignal<L, Idx, T, O>::signalIndex();
        static_assert(signalIndex >= 0, "NOTIFY signal in parent class not registered as a W_SIGNAL");
        static_assert(signalIndex < 0 || QT_VERSION >= QT_VERSION_CHECK(5, 10, 0),
                      "NOTIFY signal in parent class requires Qt 5.10");
        constexpr auto sig = w_SignalState(index<signalIndex>, OP{});
        s.template addTypeString<IsUnresolvedNotifySignal>(sig.name);
    }
};

// Generator for enums to be used in generate<>()
template<class State>
struct EnumGenerator {
    State& s;
    int dataIndex{};
    EnumGenerator(const EnumGenerator&) = delete;
    constexpr EnumGenerator(State& s, int di) : s(s), dataIndex(di) {}
    template<class Enum, class Index>
    constexpr auto operator() (const Enum& e, Index) {
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

// Generator for enum values
template<class State>
struct EnumValuesGenerator {
    State& s;
    constexpr EnumValuesGenerator(State& s) : s(s) {}
    template<class Enum, class Index>
    constexpr auto operator() (const Enum& e, Index) {
        generateAll(typename Enum::Values{}, e.names, make_index_sequence<Enum::count>{});
    }
private:
    template<size_t... Values, typename Names, size_t... Is>
    constexpr auto generateAll(index_sequence<Values...>, const Names& names, index_sequence<Is...>) {
        ((s.addString(names[index<Is>]), s.addInts((uint)Values)), ...);
    }
};

/**
 * Helper classes for MethodParametersGenerator::generateSingleMethodParameter:
 * generate the parameter array
 */
template<typename ...Args> struct HandleArgsHelper {
    template<typename Strings, typename ParamTypes, size_t I = 0>
    static constexpr auto result(Strings&, const ParamTypes&, Index<I> = {})
    {}
};
template<typename A, typename... Args>
struct HandleArgsHelper<A, Args...> {
    template<typename Strings, typename ParamTypes, size_t I = 0>
    static constexpr auto result(Strings &ss, const ParamTypes &paramTypes, Index<I> = {}) {
        using Type = typename QtPrivate::RemoveConstRef<A>::Type;
        auto typeStr = stringFetch(paramTypes, index<I>);
        using ts_t = decltype(typeStr);
        // This way, the overload of result will not pick the StaticString one if it is a tuple (because registered types have the priority)
        auto typeStr2 = std::conditional_t<std::is_same<A, Type>::value, ts_t, std::tuple<ts_t>>{typeStr};
        HandleType<Type>::result(ss, typeStr2);
        return HandleArgsHelper<Args...>::result(ss, paramTypes, index<I+1>);
    }
};
template<std::size_t N> struct HandleArgNames {
    template<typename Strings, size_t... Ns, size_t I = 0>
    static constexpr auto result(Strings &ss, const StaticStrings<Ns...>& pn, Index<I> = {}, std::enable_if_t<(I < sizeof... (Ns))>* = {}) {
        ss.addString(pn[index<I>]);
        return HandleArgNames<N-1>::result(ss, pn, index<I+1>);
    }
    template<typename Strings, size_t... Ns, size_t I = 0>
    static constexpr auto result(Strings &ss, const StaticStrings<Ns...>& pn, Index<I> = {}, std::enable_if_t<(I >= sizeof... (Ns))>* = {}) {
        ss.addInts(1);
        return HandleArgNames<N-1>::result(ss, pn, index<I+1>);
    }
};
template<> struct HandleArgNames<0> {
    template<typename Strings, typename PN, size_t I = 0>
    static constexpr auto result(Strings &, const PN&, Index<I> = {})
    {}
};

// Generator for method parameters to be used in generate<>()
template<class State>
struct MethodParametersGenerator {
    State& s;
    constexpr MethodParametersGenerator(State& s) : s(s) {}
    template<class Method, class Index>
    constexpr auto operator() (const Method& method, Index) {
        generateSingleMethodParameter(s, method.func, method.paramTypes, method.paramNames);
    }

private:
    // non-const function
    template<typename Strings, typename ParamTypes, typename ParamNames, typename Obj, typename Ret, typename... Args>
    static constexpr auto generateSingleMethodParameter(Strings &ss, Ret (Obj::*)(Args...),
                                                        const ParamTypes &paramTypes, const ParamNames &paramNames ) {
        HandleType<Ret>::result(ss, 0);
        HandleArgsHelper<Args...>::result(ss, paramTypes);
        HandleArgNames<sizeof...(Args)>::result(ss, paramNames);
    }
    template<typename Strings, typename ParamTypes, typename ParamNames, typename Obj, typename Ret, typename... Args>
    // const function
    static constexpr auto generateSingleMethodParameter(Strings &ss, Ret (Obj::*)(Args...) const,
                                                        const ParamTypes &paramTypes, const ParamNames &paramNames ) {
        HandleType<Ret>::result(ss, 0);
        HandleArgsHelper<Args...>::result(ss, paramTypes);
        HandleArgNames<sizeof...(Args)>::result(ss, paramNames);
    }
    // static member functions
    template<typename Strings, typename ParamTypes, typename ParamNames, typename Ret, typename... Args>
    static constexpr auto generateSingleMethodParameter(Strings &ss, Ret (*)(Args...),
                                                        const ParamTypes &paramTypes, const ParamNames &paramNames ) {
        HandleType<Ret>::result(ss, 0);
        HandleArgsHelper<Args...>::result(ss, paramTypes);
        HandleArgNames<sizeof...(Args)>::result(ss, paramNames);
    }
};

// Generator for constructor parameter to be used in generate<>()
template<class State>
struct ConstructorParametersGenerator {
    State& s;
    constexpr ConstructorParametersGenerator(State& s) : s(s) {}
    template<std::size_t N, typename... Args, class Index>
    constexpr auto operator() (MetaConstructorInfo<N,Args...>, Index) {
        s.addInts(IsUnresolvedType | 1);
        HandleArgsHelper<Args...>::result(s, StaticStrings<>{});
        s.addInts(((void)sizeof(Args),1)...); // all the names are 1 (for "\0")
    }
};

/** Given method, a binary::tree containing information about methods or constructor,
 *  return the amount of item it will add in the int array. */
template<size_t L, class T>
constexpr int methodsParamOffset() {
    auto sum = int{};
    foldMethods<L, T>([&](auto m, auto) { sum += int(1 + m.argCount * 2); });
    return sum;
}
template<size_t L, class T>
constexpr int constructorParamOffset() {
    auto sum = int{};
    foldConstructorState<L, T>([&](auto m, auto) { sum += int(1 + m.argCount * 2); });
    return sum;
}

/**
 * Generate the integer array and the lists of string
 */
template<size_t L, typename T, typename ObjI, typename State>
constexpr auto generateDataPass(const ObjI &objectInfo, State& state) {
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
    state.addString(objectInfo.name);
    state.addStringUntracked(StaticString{""});
    state.addInts(
        ObjI::classInfoCount, classInfoOffset, // classinfo
        ObjI::methodCount, methodOffset, // methods
        ObjI::propertyCount, propertyOffset, // properties
        ObjI::enumCount, enumOffset, // enums/sets
        ObjI::constructorCount, constructorOffset, // constructors
        0x4 /* PropertyAccessInStaticMetaCall */,   // flags
        ObjI::signalCount);

    //if (state.intCount != classInfoOffset) throw "offset mismatch!";
    foldClassInfoState<L, T**>(ClassInfoGenerator<State>{state});

    //if (state.intCount != methodOffset) throw "offset mismatch!";
    foldMethods<L, T**>(MethodGenerator<State, T>{state, paramIndex});

    //if (state.intCount != propertyOffset) throw "offset mismatch!";
    foldPropertyState<L, T**>(PropertyGenerator<State, T>{state});
    foldPropertyState<L, T**>(NotifySignalGenerator<State, L, T, hasNotify>{state});

    //if (state.intCount != enumOffset) throw "offset mismatch!";
    foldEnumState<L, T**>(EnumGenerator<State>{state, enumValueOffset});

    //if (state.intCount != constructorOffset) throw "offset mismatch!";
    foldConstructorState<L, T**>(MethodGenerator<State, T>{state, constructorParamIndex});

    //if (state.intCount != paramIndex) throw "offset mismatch!";
    foldMethods<L, T**>(MethodParametersGenerator<State>{state});

    //if (state.intCount != constructorParamIndex) throw "offset mismatch!";
    foldConstructorState<L, T**>(ConstructorParametersGenerator<State>{state});

    //if (state.intCount != enumValueOffset) throw "offset mismatch!";
    foldEnumState<L, T**>(EnumValuesGenerator<State>{state});
}
template<size_t L, typename T, typename ObjI>
constexpr auto generateDataArray(const ObjI &) {
    constexpr LayoutBuilder layout = [] {
        auto r = LayoutBuilder{};
        generateDataPass<L,T>(ObjI{}, r);
        return r;
    }();
    auto r = DataHolder<layout.stringSize, layout.stringCount, layout.intCount>{};
    auto b = DataBuilder{r};
    generateDataPass<L,T>(ObjI{}, b);
    return r;
}

/**
 * Holder for the string data.  Just like in the moc generated code.
 */
template<std::size_t N, std::size_t L> struct qt_meta_stringdata_t {
     QByteArrayData data[N];
     StaticString<L> stringdata;
};

/** Builds the string data
 * \tparam T the W_MetaObjectCreatorHelper
 * \tparam I an index_sequence that goes from 0 to the number of string
 */
template<typename T, std::size_t... I>
struct BuildStringDataHelper {
    using meta_stringdata_t = const qt_meta_stringdata_t<T::data.stringCount, T::data.stringSize>;
#ifndef Q_CC_MSVC
    static constexpr qptrdiff stringdata_offset = offsetof(meta_stringdata_t, stringdata);
#else // offsetof does not work with MSVC
    static constexpr qptrdiff stringdata_offset = sizeof(meta_stringdata_t::data);
#endif
    inline static meta_stringdata_t qt_meta_stringdata = {
        {Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(T::data.stringSizes[I]-1,
                stringdata_offset + T::data.stringOffsets[I] - I * sizeof(QByteArrayData))...},
        StaticString{T::data.stringChars}
    };
};

/**
 * returns the string data suitable for the QMetaObject from a list of string
 * T is W_MetaObjectCreatorHelper<ObjectType>
 */
// N... are all the sizes of the strings
template<typename T, size_t... Is>
constexpr const QByteArrayData *build_string_data(index_sequence<Is...>)  {
    return BuildStringDataHelper<T, Is...>::qt_meta_stringdata.data;
}

/** Returns the QMetaObject* of the base type. Use SFINAE to only return it if it exists */
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
/** Instancies the QMetaObject for class T */
template<typename T>
static constexpr QMetaObject createMetaObject()
{
    using Creator = typename T::W_MetaObjectCreatorHelper;
    auto string_data = build_string_data<Creator>(Creator::data.stringSequence);
    auto int_data = Creator::data.ints;
    return { { parentMetaObject<T>(0), string_data, int_data, T::qt_static_metacall, {}, {} } };
}

/** Implementation of qt_metacall */
template<typename T> static int qt_metacall_impl(T *_o, QMetaObject::Call _c, int _id, void** _a) {
    using Creator = typename T::W_MetaObjectCreatorHelper;
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
 * Helper for implementation of qt_static_metacall for QMetaObject::IndexOfMethod
 * T is the class, and I is the index of a method.
 * Returns I if the argument is equal to the pointer to member function of the signal of index 'I'
 */
template<typename T, int I>
static int indexOfMethod(void **func) {
    constexpr auto f = binary::get<I>(T::W_MetaObjectCreatorHelper::objectInfo.methods).func;
    using Ms = decltype(T::W_MetaObjectCreatorHelper::objectInfo.methods);
    if ((binary::tree_element_t<I,Ms>::flags & 0xc) == W_MethodType::Signal.value
        && f == *reinterpret_cast<decltype(f)*>(func))
        return I;
    return -1;
}

/**
 * Helper for implementation of qt_static_metacall for QMetaObject::InvokeMetaMethod
 * T is the class, and I is the index of a method.
 * Invoke the method with index I if id == I.
 */
template <typename T, int I>
static void invokeMethod(T *_o, int _id, void **_a) {
    if (_id == I) {
        constexpr auto f = binary::get<I>(T::W_MetaObjectCreatorHelper::objectInfo.methods).func;
        using P = QtPrivate::FunctionPointer<std::remove_const_t<decltype(f)>>;
        P::template call<typename P::Arguments, typename P::ReturnType>(f, _o, _a);
    }
}

/**
 * Helper for implementation of qt_static_metacall for QMetaObject::RegisterMethodArgumentMetaType
 * T is the class, and I is the index of a method.
 */
template <typename T, int I>
static void registerMethodArgumentType(int _id, void **_a) {
    if (_id == I) {
        constexpr auto f = binary::get<I>(T::W_MetaObjectCreatorHelper::objectInfo.methods).func;
        using P = QtPrivate::FunctionPointer<std::remove_const_t<decltype(f)>>;
        auto _t = QtPrivate::ConnectionTypes<typename P::Arguments>::types();
        uint arg = *reinterpret_cast<uint*>(_a[1]);
        *reinterpret_cast<int*>(_a[0]) = _t && arg < P::ArgumentCount ?
                _t[arg] : -1;
    }
}

/**
 * Helper for implementation of qt_static_metacall for any of the operations in a property
 * T is the class, and I is the index of a property.
 */
template<typename T, int I>
static void propertyOperation(T *_o, QMetaObject::Call _c, int _id, void **_a) {
    if (_id != I)
        return;
    constexpr auto p = binary::get<I>(T::W_MetaObjectCreatorHelper::objectInfo.properties);
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

/**
 * Helper for implementation of qt_static_metacall for QMetaObject::CreateInstance
 * T is the class, and I is the index of a constructor.
 */
template<typename T, int I>
static void createInstance(int _id, void** _a) {
    if (_id == I) {
        constexpr auto m = binary::get<I>(T::W_MetaObjectCreatorHelper::objectInfo.constructors);
        m.template createInstance<T>(_a, make_index_sequence<decltype(m)::argCount>{});
    }
}

/**
 * Function that does nothing but helps to expand template packs.
 */
template<typename...Ts> static constexpr void nop(Ts...) {}

/**
 * Implementation of qt_static_metacall for W_OBJECT_IMPL which should be called with
 * std::index_sequence for the amount of method, constructor, and properties.
 */
template<typename T, size_t...MethI, size_t ...ConsI, size_t...PropI>
static void qt_static_metacall_impl(QObject *_o, QMetaObject::Call _c, int _id, void** _a,
                        std::index_sequence<MethI...>, std::index_sequence<ConsI...>, std::index_sequence<PropI...>) {
    Q_UNUSED(_id) Q_UNUSED(_o) Q_UNUSED(_a)
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(T::staticMetaObject.cast(_o));
        nop((invokeMethod<T, MethI>(reinterpret_cast<T*>(_o), _id, _a),0)...);
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        nop((registerMethodArgumentType<T,MethI>(_id, _a),0)...);
    } else if (_c == QMetaObject::IndexOfMethod) {
        *reinterpret_cast<int *>(_a[0]) = sums((1+indexOfMethod<T,MethI>(reinterpret_cast<void **>(_a[1])))...)-1;
    } else if (_c == QMetaObject::CreateInstance) {
        nop((createInstance<T, ConsI>(_id, _a),0)...);
    } else if ((_c >= QMetaObject::ReadProperty && _c <= QMetaObject::QueryPropertyUser)
            || _c == QMetaObject::RegisterPropertyMetaType) {
        nop((propertyOperation<T,PropI>(static_cast<T*>(_o), _c, _id, _a),0)...);
    }
}

/** Same for W_GADGET */
template<typename T, size_t...MethI, size_t ...ConsI, size_t...PropI>
static void qt_static_metacall_impl(T *_o, QMetaObject::Call _c, int _id, void** _a,
                        std::index_sequence<MethI...>, std::index_sequence<ConsI...>, std::index_sequence<PropI...>) {
    Q_UNUSED(_id) Q_UNUSED(_o) Q_UNUSED(_a)
    if (_c == QMetaObject::InvokeMetaMethod) {
        nop((invokeMethod<T, MethI>(_o, _id, _a),0)...);
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        nop((registerMethodArgumentType<T,MethI>(_id, _a),0)...);
    } else if (_c == QMetaObject::IndexOfMethod) {
        Q_ASSERT_X(false, "qt_static_metacall", "IndexOfMethod called on a Q_GADGET");
    } else if (_c == QMetaObject::CreateInstance) {
        nop((createInstance<T, ConsI>(_id, _a),0)...);
    } else if ((_c >= QMetaObject::ReadProperty && _c <= QMetaObject::QueryPropertyUser)
            || _c == QMetaObject::RegisterPropertyMetaType) {
        nop((propertyOperation<T,PropI>(_o, _c, _id, _a),0)...);
    }
}

/** helper for the implementation of qt_metacast */
template <typename Interface, typename T>
static void interfaceMetaCast(void *&result, T *o, const char *clname) {
    const char *iid = qobject_interface_iid<Interface>();
    if (iid && !strcmp(clname, iid))
        result = static_cast<Interface>(o);
}

/** implementation of qt_metacast */
template<typename T, size_t... InterfaceI>
static void *qt_metacast_impl(T *o, const char *_clname, std::index_sequence<InterfaceI...>) {
    if (!_clname)
        return nullptr;
    const QByteArrayDataPtr sd = { const_cast<QByteArrayData*>(T::staticMetaObject.d.stringdata) };
    if (_clname == QByteArray(sd))
        return o;
    void *result = nullptr;
    nop((interfaceMetaCast<decltype(binary::get<InterfaceI>(T::W_MetaObjectCreatorHelper::objectInfo.interfaces))>(result, o, _clname),0)...);
    return result ? result : o->T::W_BaseType::qt_metacast(_clname);
}

}; // FriendHelper

// Forward to the FriendHelper
template<typename T> constexpr auto createMetaObject() {  return FriendHelper::createMetaObject<T>(); }
template<typename T, typename... Ts> auto qt_metacall_impl(Ts &&...args)
{ return FriendHelper::qt_metacall_impl<T>(std::forward<Ts>(args)...); }
template<typename T> auto qt_metacast_impl(T *o, const char *_clname) {
    using ObjI = decltype(T::W_MetaObjectCreatorHelper::objectInfo);
    return FriendHelper::qt_metacast_impl<T>(o, _clname,
                                              make_index_sequence<ObjI::interfaceCount>{});
}
template<typename T, typename... Ts> auto qt_static_metacall_impl(Ts &&... args) {
    using ObjI = decltype(T::W_MetaObjectCreatorHelper::objectInfo);
    return FriendHelper::qt_static_metacall_impl<T>(std::forward<Ts>(args)...,
                                                     make_index_sequence<ObjI::methodCount>{},
                                                     make_index_sequence<ObjI::constructorCount>{},
                                                     make_index_sequence<ObjI::propertyCount>{});
}
} // w_internal

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
        struct Name { static constexpr auto& value = W_MACRO_STRIGNIFY(W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)); }; \
        static constexpr auto objectInfo = w_internal::ObjectInfo<__COUNTER__, W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::W_ThisType, Name>{}; \
        static constexpr auto data = w_internal::generateDataArray<__COUNTER__, W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::W_ThisType>(objectInfo); \
    }; \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) INLINE const QT_INIT_METAOBJECT QMetaObject W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::staticMetaObject = \
        w_internal::createMetaObject<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::W_ThisType>();

/** \macro W_OBJECT_IMPL(TYPE [, TEMPLATE_STUFF])
 * This macro expand to the code that instantiate the QMetaObject. It must be placed outside of
 * the class, in the .cpp file. The TYPE argument must be the qualified name of the class,
 * including the namespace, if any.
 * Example: `W_OBJECT_IMPL(Namespace::MyClass)`
 *
 * If the class is a templated class, the second argument contains the template introducer.
 * Example:  `W_OBJECT_IMPL(MyTemplate<T>, template <typename T>)`
 * Parentheses are required if there is several template arguments:
 * `W_OBJECT_IMPL((MyTemplate2<A,B>), template<typename A, typename B>)`
 */
#define W_OBJECT_IMPL(...) \
    W_OBJECT_IMPL_COMMON(W_MACRO_EMPTY, __VA_ARGS__) \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) void W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void** _a) \
    { w_internal::qt_static_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(_o, _c, _id, _a); } \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) const QMetaObject *W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::metaObject() const  { return &staticMetaObject; } \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) void *W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_metacast(const char *_clname) \
    { return w_internal::qt_metacast_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(this, _clname); } \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) int W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_metacall(QMetaObject::Call _c, int _id, void** _a) \
    { return w_internal::qt_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(this, _c, _id, _a); }


/** \macro W_GADGET_IMPL(TYPE [, TEMPLATE_STUFF])
 * Same as W_OBJECT_IMPL, but for a W_GADGET
 */
#define W_GADGET_IMPL(...) \
    W_OBJECT_IMPL_COMMON(W_MACRO_EMPTY, __VA_ARGS__) \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) void W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void** _a) \
    { w_internal::qt_static_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(reinterpret_cast<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)*>(_o), _c, _id, _a); }

/** \macro W_NAMESPACE_IMPL(...)
 * Same as W_OBJECT_IMPL, but for a W_NAMESPACE
 */
#define W_NAMESPACE_IMPL(...) \
    W_OBJECT_IMPL_COMMON(W_MACRO_EMPTY, __VA_ARGS__)


/** \macro W_OBJECT_IMPL_INLINE(TYPE [, TEMPLATE_STUFF])
 * Same as W_OBJECT_IMPL, but to be used in a header
 * (Requires support for c++17 inline variables)
 */
#define W_OBJECT_IMPL_INLINE(...) \
    W_OBJECT_IMPL_COMMON(inline, __VA_ARGS__) \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) inline void W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void** _a) \
    { w_internal::qt_static_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(_o, _c, _id, _a); } \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) inline const QMetaObject *W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::metaObject() const  { return &staticMetaObject; } \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) inline void *W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_metacast(const char *_clname) \
    { return w_internal::qt_metacast_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(this, _clname); } \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) inline int W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_metacall(QMetaObject::Call _c, int _id, void** _a) \
    { return w_internal::qt_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(this, _c, _id, _a); }


/** \macro W_GADGET_IMPL_INLINE(TYPE [, TEMPLATE_STUFF])
 * Same as W_GADGET_IMPL, but to be used in a header
 * (Requires support for c++17 inline variables)
 */
#define W_GADGET_IMPL_INLINE(...) \
    W_OBJECT_IMPL_COMMON(inline, __VA_ARGS__) \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) inline void W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void** _a) \
    { w_internal::qt_static_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(reinterpret_cast<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)*>(_o), _c, _id, _a); }

/** \macro W_NAMESPACE_IMPL_INLINE(...)
 * Same as W_NAMESPACE_IMPL, but to be used in a header
 * (Requires support for c++17 inline variables)
 */
#define W_NAMESPACE_IMPL_INLINE(...) \
    W_OBJECT_IMPL_COMMON(inline, __VA_ARGS__)

