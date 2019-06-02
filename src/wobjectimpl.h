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
    }
    template<class... Ts>
    constexpr void addInts(Ts... vs) {
        ((*ip++ = vs),...);
    }
};


template<class F, size_t... Is>
constexpr auto toTree(index_sequence<Is...>, F f) {
    (void)f;
    if constexpr (0 == sizeof... (Is)) return binary::tree<>{};
    else if constexpr (1 == sizeof... (Is)) return (binary::tree_append(binary::tree<>{}, f(index<Is>)), ...);
    else return binary::tree_cat(binary::tree_append(binary::tree<>{}, f(index<Is>)) ...);
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


#if defined Q_CC_GNU && !defined Q_CC_CLANG /* Work around GCC bug 69681 */
template <typename T> constexpr bool getSignalIndexHelperCompare(T f1, T f2) { return f1 == f2; }
/** Helper to get information about the notify signal of the property within object T */
template<size_t L, typename T, int PropIdx, typename BaseT = T>
struct ResolveNotifySignal {
    static constexpr auto signalState = w_SignalStateTree<L, BaseT**>;
    static constexpr auto propertyInfo = w_PropertyStateTree<L, T**>;
private:
    static constexpr auto prop = binary::get<PropIdx>(propertyInfo);
    // We need to use SFINAE because of GCC bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=69681
    // For some reason, GCC fails to consider f1==f2 as a constexpr if f1 and f2 are pointer to
    // different function of the same type. Fortunately, when both are pointing to the same function
    // it compiles fine, so we can use SFINAE for it.
    template<int SigIdx,
             bool Eq = getSignalIndexHelperCompare(binary::get<SigIdx>(signalState).func,
                                                   prop.notify)>
    static constexpr int helper(int)
    { return Eq ? SigIdx : -1; }
    template<int SigIdx> static constexpr int helper(...) { return -1; }

    template<size_t... I>
    static constexpr int computeSignalIndex(index_sequence<I...>)
    { return std::max({-1, helper<I>(0)...}); }
public:
    static constexpr int signalIndex()
    { return computeSignalIndex(make_index_sequence<signalState.size>()); }
};
#else
/** Helper comparator that compares function pointers and return true if they are the same.
 * If thier type are different, pointer to member functions are different
 */
template <typename T> constexpr bool getSignalIndexHelperCompare(T f1, T f2) { return f1 == f2; }
template <typename T1, typename T2> constexpr bool getSignalIndexHelperCompare(T1, T2) { return false; }

/** Helper to get information about the notify signal of the property within object T */
template<size_t L, typename T>
struct ResolveNotifySignal {
    static constexpr auto signalState = w_SignalStateTree<L, T**>;
private:
    template<typename F, size_t... I>
    static constexpr int computeSignalIndex(F f, index_sequence<I...>) {
        return std::max({-1,
            (getSignalIndexHelperCompare(binary::get<I>(signalState).func,f) ? int(I) : -1)...});
    }
public:
    template<typename F>
    static constexpr int signalIndex(F f)
    { return computeSignalIndex(f, make_index_sequence<signalState.size>()); }
};
#endif

/** returns true if the object T has at least one property with a notify signal */
template <size_t L, typename T, std::size_t... I>
static constexpr bool hasNotifySignal(std::index_sequence<I...>)
{
    constexpr auto propertyInfo = w_PropertyStateTree<L, T**>;
    Q_UNUSED(propertyInfo) // in case I is empty
    return sums(!std::is_same<decltype(binary::get<I>(propertyInfo).notify), std::nullptr_t>::value ...);
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

// Generator for Q_CLASSINFO to be used in generate<>()
struct ClassInfoGenerator {
    template<typename> static constexpr int offset() { return 0; }
    template<int, typename State, typename CI>
    static constexpr auto generate(State& s, CI ci) {
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

template<class... T>
struct debug_types;

// Generator for methods to be used in generate<>()
template <typename T>
struct MethodGenerator {
    template<typename Method> static constexpr int offset() { return 1 + Method::argCount * 2; }
    template<int ParamIndex, typename State, typename Method>
    static constexpr auto generate(State& s, const Method& method) {
        //auto x = debug_types<decltype(s)>{};
        s.addString(method.name); // name
        s.addInts((uint)Method::argCount,
                   ParamIndex, //parameters
                   1, //tag, always \0
                   adjustFlags(Method::flags, typename Method::IntegralConstant()));
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
template<typename T>
struct PropertyGenerator {
    template<typename> static constexpr int offset() { return 0; }
    template<int, typename State, typename Prop>
    static constexpr void generate(State& s, Prop prop) {
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
template<size_t L, typename T, bool hasNotify>
struct NotifySignalGenerator {
    template<typename> static constexpr int offset() { return 1; }
        template<int Idx, typename State, typename Prop>
    static constexpr auto generate(State& s, Prop prop) {
        return process<Idx>(s, prop.notify);
    }
private:

    static constexpr auto propertyInfo = w_PropertyStateTree<L, T**>;

    // No notify signal
    template<int, typename State>
    static constexpr auto process(State& s, Empty) {
        return s.addInts(0);
    }

    // Signal in the same class
    template<int Idx, typename State, typename Func>
    static constexpr auto process(State& s, Func, std::enable_if_t<
        std::is_same<T, typename QtPrivate::FunctionPointer<Func>::Object>::value, int> = 0)
    {
#if defined Q_CC_GNU && !defined Q_CC_CLANG
        constexpr int signalIndex = ResolveNotifySignal<L, T, Idx>::signalIndex();
#else
        constexpr int signalIndex = ResolveNotifySignal<L, T>::signalIndex(binary::get<Idx>(propertyInfo).notify);
#endif
        static_assert(signalIndex >= 0, "NOTIFY signal not registered as a signal");
        return s.addInts(signalIndex);
    }

    // Signal in a parent class
    template<int Idx, typename State, typename Func>
    static constexpr auto process(State& s, Func, std::enable_if_t<
        !std::is_same<T, typename QtPrivate::FunctionPointer<Func>::Object>::value, int> = 0)
    {
#if defined Q_CC_GNU && !defined Q_CC_CLANG
        using Finder = ResolveNotifySignal<L, T, Idx, typename QtPrivate::FunctionPointer<Func>::Object>;
        constexpr int signalIndex = Finder::signalIndex();
#else
        using Finder = ResolveNotifySignal<L, typename QtPrivate::FunctionPointer<Func>::Object>;
        constexpr int signalIndex = Finder::signalIndex(binary::get<Idx>(propertyInfo).notify);
#endif
        static_assert(signalIndex >= 0, "NOTIFY signal in parent class not registered as a W_SIGNAL");
        static_assert(signalIndex < 0 || QT_VERSION >= QT_VERSION_CHECK(5, 10, 0),
                      "NOTIFY signal in parent class requires Qt 5.10");
        constexpr auto sig = binary::get<signalIndex>(Finder::signalState);
        return s.template addTypeString<IsUnresolvedNotifySignal>(sig.name);
    }
};
template<size_t L, typename T> struct NotifySignalGenerator<L, T,false> {
    template<typename> static constexpr int offset() { return 0; }
    template<int, typename State, typename Prop>
    static constexpr auto generate(State& s, Prop) { return s; }
};

// Generator for enums to be used in generate<>()
struct EnumGenerator {
    template<typename Enum> static constexpr int offset() { return Enum::count * 2; }
    template<int DataIndex, typename State, typename Enum>
    static constexpr auto generate(State& s, Enum e, std::enable_if_t<Enum::hasAlias> * = nullptr) {
        s.addString(e.name); // name
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        s.addString(e.alias); // alias
#endif
        s.addInts(Enum::flags, (uint)Enum::count, DataIndex);
    }

    template<int DataIndex, typename State, typename Enum>
    static constexpr auto generate(State& s, Enum e, std::enable_if_t<!Enum::hasAlias> * = nullptr) {
        auto index = s.stringCount; // required for MSVC-2019
        s.addString(e.name); // name
        s.addInts(
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
                index, // alias is the same as the name
#endif
            Enum::flags, (uint)Enum::count, DataIndex);
    }

};

// Generator for enum values
struct EnumValuesGenerator {
    template<typename Strings, typename Names, size_t I = 0>
    static constexpr auto generateSingleEnumValues(Strings &, std::index_sequence<>, const Names&, Index<I> = {}) {}

    template<typename Strings, std::size_t Value, std::size_t... Vs, typename Names, size_t I = 0>
    static constexpr auto generateSingleEnumValues(Strings &s, std::index_sequence<Value, Vs...>, const Names& names, Index<I> = {}) {
        s.addString(names[index<I>]);
        s.addInts((uint)Value);
        generateSingleEnumValues(s, std::index_sequence<Vs...>{}, names, index<I+1>);
    }

    template<typename> static constexpr int offset() { return 0; }
    template<int, typename State, typename Enum>
    static constexpr auto generate(State& s, Enum e) {
        return generateSingleEnumValues(s, typename Enum::Values{}, e.names);
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
struct MethodParametersGenerator {
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
public:

    template<typename> static constexpr int offset() { return 0; }
    template<int, typename State, typename Method>
    static constexpr auto generate(State& s, Method method) {
        // Dispatch either  non const function, const function or static function
        return generateSingleMethodParameter(s, method.func, method.paramTypes, method.paramNames);
    }
};

// Generator for constructor parameter to be used in generate<>()
struct ConstructorParametersGenerator {
    template<typename> static constexpr int offset() { return 0; }
    template<int, typename State, std::size_t N, typename... Args>
    static constexpr auto generate(State& s, MetaConstructorInfo<N,Args...>) {
        s.addInts(IsUnresolvedType | 1);
        HandleArgsHelper<Args...>::result(s, StaticStrings<>{});
        s.addInts(((void)sizeof(Args),1)...); // all the names are 1 (for "\0")
    }
};

/** Given method, a binary::tree containing information about methods or constructor,
 *  return the amount of item it will add in the int array. */
template<typename Methods, std::size_t... I>
constexpr int paramOffset(std::index_sequence<I...>)
{ return sums(int(1 + binary::tree_element_t<I, Methods>::argCount * 2)...); }

/**
 * Generate the integer array and the lists of string
 */
template<size_t L, typename T, typename ObjI, typename State>
constexpr auto generateDataPass(const ObjI &objectInfo, State& state) {
    constexpr bool hasNotify = hasNotifySignal<L, T>(make_index_sequence<ObjI::propertyCount>{});
    constexpr int classInfoOffset = 14;
    constexpr int methodOffset = classInfoOffset + ObjI::classInfoCount * 2;
    constexpr int propertyOffset = methodOffset + ObjI::methodCount * 5;
    constexpr int enumOffset = propertyOffset + ObjI::propertyCount * (hasNotify ? 4: 3);
    constexpr int constructorOffset = enumOffset + ObjI::enumCount * (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) ? 5 : 4);
    constexpr int paramIndex = constructorOffset + ObjI::constructorCount * 5 ;
    constexpr int constructorParamIndex = paramIndex +
        paramOffset<decltype(objectInfo.methods)>(make_index_sequence<ObjI::methodCount>{});
    constexpr int enumValueOffset = constructorParamIndex +
        paramOffset<decltype(objectInfo.constructors)>(make_index_sequence<ObjI::constructorCount>{});

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

    generate<ClassInfoGenerator, 0>(state , objectInfo.classInfos);
    generate<MethodGenerator<T>, paramIndex>(state , objectInfo.methods);
    generate<PropertyGenerator<T>, 0>(state, objectInfo.properties);
    generate<NotifySignalGenerator<L, T, hasNotify>, 0>(state, objectInfo.properties);
    generate<EnumGenerator, enumValueOffset>(state, objectInfo.enums);
    generate<MethodGenerator<T>, constructorParamIndex>(state, objectInfo.constructors);
    generate<MethodParametersGenerator, 0>(state, objectInfo.methods);
    generate<ConstructorParametersGenerator, 0>(state, objectInfo.constructors);
    generate<EnumValuesGenerator, 0>(state, objectInfo.enums);
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

