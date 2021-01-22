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

/// all details about a class T
template<class T, class Name, size_t L = 1024*1024*1024>
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

#if __cplusplus > 201700L
    template<size_t Idx>
    static constexpr auto method(Index<Idx>) {
        using TPP = T**;
        if constexpr (Idx < signalCount) return w_state(index<Idx>, SignalStateTag{}, TPP{});
        else if constexpr (Idx - signalCount < slotCount) return w_state(index<Idx - signalCount>, SlotStateTag{}, TPP{});
        else return w_state(index<Idx - signalCount - slotCount>, MethodStateTag{}, TPP{});
    }
#else
    template<size_t Idx>
    static constexpr auto method(Index<Idx>, std::enable_if_t<(Idx < signalCount)>* = {}) {
        using TPP = T**;
        return w_state(index<Idx>, SignalStateTag{}, TPP{});
    }
    template<size_t Idx>
    static constexpr auto method(Index<Idx>, std::enable_if_t<(Idx >= signalCount && Idx - signalCount < slotCount)>* = {}) {
        using TPP = T**;
        return w_state(index<Idx - signalCount>, SlotStateTag{}, TPP{});
    }
    template<size_t Idx>
    static constexpr auto method(Index<Idx>, std::enable_if_t<(Idx >= signalCount + slotCount)>* = {}) {
        using TPP = T**;
        return w_state(index<Idx - signalCount - slotCount>, MethodStateTag{}, TPP{});
    }
#endif
};

template<class F, size_t... Is>
constexpr void fold(index_sequence<Is...>, F&& f) {
    (void)f;
#if __cplusplus > 201700L
    (f(index<Is>), ...);
#else
    ordered((f(index<Is>),0)...);
#endif
}

#if __cplusplus > 201700L
template <size_t L, class State, class TPP, class F>
constexpr void foldState(F&& f) {
    fold(make_index_sequence<stateCount<L, State, TPP>>{}, [&](auto i) { f(w_state(i, State{}, TPP{}), i); });
}
#else
template<class F, class State, class TPP>
struct FoldState {
    F&& f;
    template<class I>
    constexpr void operator() (I i) { f(w_state(i, State{}, TPP{}), i); }
};
template <size_t L, class State, class TPP, class F>
constexpr void foldState(F&& f) {
    auto fs = FoldState<F, State, TPP>{std::forward<F>(f)};
    fold(make_index_sequence<stateCount<L, State, TPP>>{}, fs);
}
#endif

template <size_t L, class T, class F>
constexpr void foldMethods(F&& f) {
    foldState<L, SignalStateTag, T>(f);
    foldState<L, SlotStateTag, T>(f);
    foldState<L, MethodStateTag, T>(f);
}

template<class T>
constexpr auto fetchExplicitName(const StringView& defaultName, ...) {
    return defaultName;
}

template<class T>
constexpr auto fetchExplicitName(const StringView&, int) -> decltype (w_explicitObjectName(static_cast<T*>(nullptr))) {
    return w_explicitObjectName(static_cast<T*>(nullptr));
}

constexpr bool isPropertyMetacall(QMetaObject::Call call) noexcept
{
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    return (call >= QMetaObject::ReadProperty && call <= QMetaObject::ResetProperty)
        || call == QMetaObject::RegisterPropertyMetaType || call == QMetaObject::BindableProperty;
#else
    return (call >= QMetaObject::ReadProperty && call <= QMetaObject::QueryPropertyUser)
        || call == QMetaObject::RegisterPropertyMetaType;
#endif
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
#if __cplusplus > 201700L
        ((match<Is>(0) ? r = (int)Is : 0), ...);
#else
        ordered2<int>({(match<Is>(0) ? r = (int)Is : 0)...});
#endif
        return r;
    }
public:
    static constexpr int signalIndex() {
        return indexFold(make_index_sequence<stateCount<L, SignalStateTag, O**>>{});
    }
};

/// returns true if the object T has at least one property with a notify signal
#if __cplusplus > 201700L
template <size_t L, typename TPP>
constexpr bool hasNotifySignal() {
    auto r = bool{};
    foldState<L, PropertyStateTag, TPP>([&](auto p, auto) {
        r = r || !std::is_same<decltype(p.notify), std::nullptr_t>::value;
    });
    return r;
}
#else
struct HasNotifySignal {
    bool r{};
    template<class P, class I>
    constexpr void operator() (P p, I) {
        r = r || !std::is_same<decltype(p.notify), std::nullptr_t>::value;
    }
};
template <size_t L, typename TPP>
constexpr bool hasNotifySignal() {
    auto hns = HasNotifySignal{};
    foldState<L, PropertyStateTag, TPP>(hns);
    return hns.r;
}
#endif


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
template<class T, class M>
auto test_public(int) -> std::enable_if_t<std::is_same<void, decltype(T::w_GetAccessSpecifierHelper(M{}))>::value, std::true_type>;
template<class T, class M>
auto test_public(float) -> std::false_type;
template<class T, class M>
using isPublic = decltype(test_public<T, M>(0));

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
        s.addInts(
            (uint)Method::argCount,
            parameterIndex, //parameters
            1, //tag, always \0
            adjustFlags(Method::flags, typename Method::IntegralConstant())
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
            , s.metaTypeCount
#endif
            );
        parameterIndex += 1 + Method::argCount * 2;
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
        registerMetaTypes(method);
#endif
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

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    template<class Method>
    constexpr void registerMetaTypes(const Method& method) {
        generateSingleMethodParameter(method.func);
    }
    template<class... Args>
    constexpr void registerMetaTypes(const MetaConstructorInfo<Args...>&) {
        (s.template addMetaType<Args>(), ...);
    }

    // non-const function
    template<typename Obj, typename Ret, typename... Args>
    constexpr void generateSingleMethodParameter(Ret (Obj::*)(Args...)) {
        s.template addMetaType<Ret>();
        (s.template addMetaType<Args>(), ...);
    }
    template<typename Obj, typename Ret, typename... Args>
    constexpr void generateSingleMethodParameter(Ret (Obj::*)(Args...) noexcept) {
        s.template addMetaType<Ret>();
        (s.template addMetaType<Args>(), ...);
    }
    template<typename Obj, typename Ret, typename... Args>
    // const function
    constexpr void generateSingleMethodParameter(Ret (Obj::*)(Args...) const) {
        s.template addMetaType<Ret>();
        (s.template addMetaType<Args>(), ...);
    }
    template<typename Obj, typename Ret, typename... Args>
    constexpr void generateSingleMethodParameter(Ret (Obj::*)(Args...) const noexcept) {
        s.template addMetaType<Ret>();
        (s.template addMetaType<Args>(), ...);
    }
    // static member functions
    template<typename Ret, typename... Args>
    constexpr void generateSingleMethodParameter(Ret (*)(Args...)) {
        s.template addMetaType<Ret>();
        (s.template addMetaType<Args>(), ...);
    }
    template<typename Ret, typename... Args>
    constexpr void generateSingleMethodParameter(Ret (*)(Args...) noexcept) {
        s.template addMetaType<Ret>();
        (s.template addMetaType<Args>(), ...);
    }
#endif
};

/// compute if type T is a builtin QMetaType
template <typename T, typename = void> struct MetaTypeIdIsBuiltIn : std::false_type {};
template <typename T> struct MetaTypeIdIsBuiltIn<T, typename std::enable_if<QMetaTypeId2<T>::IsBuiltIn>::type> : std::true_type{};

/// Helper to generate the type information of type 'T':
/// If T is a builtin QMetaType, its meta type id need to be added in the state.
/// If it's not builtin, then it would be an reference to T's name.
#if __cplusplus > 201700L
template<typename T, typename State, typename TypeStr = int>
constexpr void handleType(State& s, TypeStr v = {}) {
    (void)v;
    if constexpr (MetaTypeIdIsBuiltIn<T>::value) {
        s.addInts(QMetaTypeId2<T>::MetaType);
    }
    else if constexpr (std::is_same_v<std::decay_t<TypeStr>, StringView>) {
        s.addTypeString(v);
    }
    else {
        s.addTypeString(W_TypeRegistery<T>::name);
        static_assert(W_TypeRegistery<T>::registered, "Please Register T with W_REGISTER_ARGTYPE");
    }
}
#else
template<typename T, typename State, typename TypeStr = int>
constexpr void handleType(State& s, TypeStr = {}, std::enable_if_t<MetaTypeIdIsBuiltIn<T>::value>* = {}) {
    s.addInts(QMetaTypeId2<T>::MetaType);
}
template<typename T, typename State, typename TypeStr = int>
constexpr void handleType(State& s, TypeStr = {}, std::enable_if_t<!MetaTypeIdIsBuiltIn<T>::value>* = {}) {
    s.addTypeString(W_TypeRegistery<T>::name);
    static_assert(W_TypeRegistery<T>::registered, "Please Register T with W_REGISTER_ARGTYPE");
}
template<typename T, typename State>
constexpr void handleType(State& s, StringView v, std::enable_if_t<!MetaTypeIdIsBuiltIn<T>::value>* = {}) {
    s.addTypeString(v);
}
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
template<class State, size_t L, class T>
#else
template<class State, class T>
#endif
struct PropertyGenerator {
    State& s;
    constexpr PropertyGenerator(State& s) : s(s) {}

    template<class Prop, size_t Idx>
    constexpr void operator() (const Prop& prop, Index<Idx>) {
        s.addString(prop.name);
        handleType<typename Prop::PropertyType>(s, prop.typeStr);
        constexpr uint moreFlags = (QtPrivate::IsQEnumHelper<typename Prop::PropertyType>::Value
                                        ? uint(PropertyFlags::EnumOrFlag) : 0);
        constexpr uint finalFlag = std::is_final<T>::value ? 0 | PropertyFlags::Final : 0;
        constexpr uint defaultFlags = 0 | PropertyFlags::Stored | PropertyFlags::Scriptable
            | PropertyFlags::Designable;
        s.addInts(Prop::flags | moreFlags | finalFlag | defaultFlags);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
        addSignal(prop.notify, index<Idx>);
        s.addInts(0); // revision
#endif
    }

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    template<size_t Idx>
    constexpr void addSignal(Empty, Index<Idx>) {
        s.addInts(-1);
    }

    // Signal in the same class
    template<size_t Idx, typename Func>
    constexpr void addSignal(Func, Index<Idx>, std::enable_if_t<std::is_same<T, typename QtPrivate::FunctionPointer<Func>::Object>::value, int> = 0) {
        constexpr int signalIndex = ResolveNotifySignal<L, Idx, T, T>::signalIndex();
        static_assert(signalIndex >= 0, "NOTIFY signal in parent class not registered as a W_SIGNAL");
        s.addInts(signalIndex);
    }

    // Signal in a parent class
    template<size_t Idx, typename Func>
    constexpr void addSignal(Func, Index<Idx>, std::enable_if_t<!std::is_same<T, typename QtPrivate::FunctionPointer<Func>::Object>::value, int> = 0) {
        using O = typename QtPrivate::FunctionPointer<Func>::Object;
        using OP = O**;
        constexpr int signalIndex = ResolveNotifySignal<L, Idx, T, O>::signalIndex();
        static_assert(signalIndex >= 0, "NOTIFY signal in parent class not registered as a W_SIGNAL");
        constexpr auto sig = w_state(index<signalIndex>, SignalStateTag{}, OP{});
        s.template addTypeString<IsUnresolvedNotifySignal>(sig.name);
    }
#endif
};

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
template<class State, class T>
struct PropertyMetaTypeGenerator {
    State& s;
    constexpr PropertyMetaTypeGenerator(State& s) : s(s) {}

    template<class Prop, class I>
    constexpr void operator() (const Prop&, I) {
        s.template addMetaType<typename Prop::PropertyType>();
    }
};
#endif

#if __cplusplus > 201700L
template<class State, size_t L, class T, bool hasNotify>
struct NotifySignalGenerator {
#else
template<class State, size_t L, class T, bool hasNotify>
struct NotifySignalGenerator;
template<class State, size_t L, class T>
struct NotifySignalGenerator<State, L, T, false> {
    constexpr NotifySignalGenerator(State&) {}
    template<class Prop, size_t Idx>
    constexpr void operator() (const Prop&, Index<Idx>) {}
};
template<class State, size_t L, class T>
struct NotifySignalGenerator<State, L, T, true> {
#endif
    State& s;
    constexpr NotifySignalGenerator(State& s) : s(s) {}

#if __cplusplus > 201700L
    template<class Prop, size_t Idx>
    constexpr void operator() (const Prop& prop, Index<Idx>) {
        if constexpr (hasNotify) {
            process(prop.notify, index<Idx>);
        }
    }
#else
    template<class Prop, size_t Idx>
    constexpr void operator() (const Prop& prop, Index<Idx>) {
        process(prop.notify, index<Idx>);
    }
#endif

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
#if __cplusplus > 201700L
        if constexpr (Enum::hasAlias) s.addString(e.alias); // alias
#else
        if (Enum::hasAlias) s.addString(e.alias); // alias
#endif
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
#if __cplusplus > 201700L
        ((s.addString(names[Is]), s.addInts((uint)Values)), ...);
#else
        ordered((s.addString(names[Is]), s.addInts((uint)Values), 0)...);
#endif
    }
};

#if __cplusplus > 201700L
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
#else
template<size_t I, size_t N>
constexpr auto stringFetch(const StringViewArray<N>& s, std::enable_if_t<(I < N)>* = {}) {
    return s[I];
}
template<size_t I, size_t N>
constexpr auto stringFetch(const StringViewArray<N>& s, std::enable_if_t<!(I < N)>* = {}) {
    (void)s;
    struct _{};
    return _{};
}
#endif
template<class Arg, class State, class TypeName>
constexpr void handleArgType(State& ss, const TypeName& typeName) {
    using Type = typename QtPrivate::RemoveConstRef<Arg>::Type;
    // This way, the overload of result will not pick the StringView one if it is a tuple (because registered types have the priority)
    auto typeName2 = std::conditional_t<std::is_same<Arg, Type>::value, TypeName, std::tuple<TypeName>>{typeName};
    handleType<Type>(ss, typeName2);
}
template<class... Args, class State, class TypeNames, size_t... Is>
constexpr void handleArgTypes(State& ss, const TypeNames& typeNames, index_sequence<Is...>) {
#if __cplusplus > 201700L
    (handleArgType<Args>(ss, stringFetch<Is>(typeNames)), ...);
#else
    ordered((handleArgType<Args>(ss, stringFetch<Is>(typeNames)), 0)...);
#endif
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
        handleArgTypes<Args...>(s, method.paramTypes, Method::argSequence);
        handleArgNames<Method::argCount>(s, method.paramNames);
    }
#if defined(__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510
    template<typename Method, typename Obj, typename Ret, typename... Args>
    constexpr void generateSingleMethodParameter(Ret (Obj::*)(Args...) noexcept, const Method &method) {
        handleType<Ret>(s);
        handleArgTypes<Args...>(s, method.paramTypes, Method::argSequence);
        handleArgNames<Method::argCount>(s, method.paramNames);
    }
#endif
    template<typename Method, typename Obj, typename Ret, typename... Args>
    // const function
    constexpr void generateSingleMethodParameter(Ret (Obj::*)(Args...) const, const Method &method) {
        handleType<Ret>(s);
        handleArgTypes<Args...>(s, method.paramTypes, Method::argSequence);
        handleArgNames<Method::argCount>(s, method.paramNames);
    }
#if defined(__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510
    template<typename Method, typename Obj, typename Ret, typename... Args>
    constexpr void generateSingleMethodParameter(Ret (Obj::*)(Args...) const noexcept, const Method &method) {
        handleType<Ret>(s);
        handleArgTypes<Args...>(s, method.paramTypes, Method::argSequence);
        handleArgNames<Method::argCount>(s, method.paramNames);
    }
#endif
    // static member functions
    template<typename Method, typename Ret, typename... Args>
    constexpr void generateSingleMethodParameter(Ret (*)(Args...), const Method &method) {
        handleType<Ret>(s);
        handleArgTypes<Args...>(s, method.paramTypes, Method::argSequence);
        handleArgNames<Method::argCount>(s, method.paramNames);
    }
#if defined(__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510
    template<typename Method, typename Ret, typename... Args>
    constexpr void generateSingleMethodParameter(Ret (*)(Args...) noexcept, const Method &method) {
        handleType<Ret>(s);
        handleArgTypes<Args...>(s, method.paramTypes, Method::argSequence);
        handleArgNames<Method::argCount>(s, method.paramNames);
    }
#endif
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
#if __cplusplus > 201700L
template<size_t L, class T>
constexpr int methodsParamOffset() {
    auto sum = int{};
    foldMethods<L, T>([&](auto m, auto) { sum += int(1 + m.argCount * 2); });
    return sum;
}
#else
struct MethodParamOffset {
    int sum{};
    template<class M, class I>
    constexpr void operator() (M m, I) { sum += int(1 + m.argCount * 2); }
};
template<size_t L, class T>
constexpr int methodsParamOffset() {
    auto mpo = MethodParamOffset{};
    foldMethods<L, T>(mpo);
    return mpo.sum;
}
#endif
#if __cplusplus > 201700L
template<size_t L, class T>
constexpr int constructorParamOffset() {
    auto sum = int{};
    foldState<L, ConstructorStateTag, T>([&](auto m, auto) { sum += int(1 + m.argCount * 2); });
    return sum;
}
#else
struct ConstructorParamOffset {
    int sum{};
    template<class M, class I>
    constexpr void operator() (M m, I) { sum += int(1 + m.argCount * 2); }
};
template<size_t L, class T>
constexpr int constructorParamOffset() {
    auto cpo = ConstructorParamOffset{};
    foldState<L, ConstructorStateTag, T>(cpo);
    return cpo.sum;
}

#endif

template<class T, size_t N> using RawArray = T[N];
template<class T, size_t N> struct OwnArray {
    RawArray<T, N> data{};
    constexpr OwnArray(const T (&s)[N]) {
        auto p = data;
        for (auto c : s) *p++ = c;
    }
};

struct LayoutBuilder {
    size_t stringSize{};
    uint stringCount{};
    uint intCount{};
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    uint metaTypeCount{};
#endif

    constexpr void addString(const StringView& s) {
        stringSize += s.size() + 1;
        stringCount += 1;
        intCount += 1;
    }
    constexpr void addStringUntracked(const StringView& s) {
        stringSize += s.size() + 1;
        stringCount += 1;
    }
    template<uint Flag = IsUnresolvedType>
    constexpr void addTypeString(const StringView& s) {
        stringSize += s.size() + 1;
        stringCount += 1;
        intCount += 1;
    }
    template<class... Ts>
    constexpr void addInts(Ts...) {
        intCount += sizeof... (Ts);
    }
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    template<class T>
    constexpr void addMetaType() {
        metaTypeCount += 1;
    }
#endif
};
struct DataBuilder {
    char* stringCharP{};
    qptrdiff* stringOffestP{};
    int* stringLengthP{};
    uint* intP{};
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    const QtPrivate::QMetaTypeInterface** metaTypeP{};
#endif
    uint stringCount{};
    uint intCount{};
    qptrdiff stringOffset{};
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    uint metaTypeCount{};
#endif

    constexpr DataBuilder() = default;
    DataBuilder(const DataBuilder&) = delete;
    template<class Holder>
    constexpr DataBuilder(Holder& r)
        : stringCharP(r.stringChars)
        , stringOffestP(r.stringOffsets)
        , stringLengthP(r.stringLengths)
        , intP(r.ints)
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
        , metaTypeP(r.metaTypes)
#endif
        , stringOffset(r.stringOffset) {}

    constexpr void addString(const StringView& s) {
        for (auto c : s) *stringCharP++ = c;
        *stringCharP++ = '\0';
        *stringOffestP++ = stringOffset;
        *stringLengthP++ = s.size();
        *intP++ = stringCount;
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
        stringOffset += 1 + s.size();
#else
        stringOffset += 1 + s.size() - qptrdiff(sizeof(QByteArrayData));
#endif
        stringCount += 1;
        intCount += 1;
    }
    constexpr void addStringUntracked(const StringView& s) {
        for (auto c : s) *stringCharP++ = c;
        *stringCharP++ = '\0';
        *stringOffestP++ = stringOffset;
        *stringLengthP++ = s.size();
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
        stringOffset += 1 + s.size();
#else
        stringOffset += 1 + s.size() - qptrdiff(sizeof(QByteArrayData));
#endif
        stringCount += 1;
    }

    template<uint Flag = IsUnresolvedType>
    constexpr void addTypeString(const StringView& s) {
        for (auto c : s) *stringCharP++ = c;
        *stringCharP++ = '\0';
        *stringOffestP++ = stringOffset;
        *stringLengthP++ = s.size();
        *intP++ = Flag | stringCount;
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
        stringOffset += 1 + s.size();
#else
        stringOffset += 1 + s.size() - qptrdiff(sizeof(QByteArrayData));
#endif
        stringCount += 1;
        intCount += 1;
    }
    template<class... Ts>
    constexpr void addInts(Ts... vs) {
#if __cplusplus > 201700L
        ((*intP++ = vs),...);
#else
        ordered2<uint>({(*intP++ = vs)...});
#endif
        intCount += sizeof... (Ts);
    }

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    template<class T>
    static constexpr const QtPrivate::QMetaTypeInterface *metaTypeInterface = QtPrivate::qTryMetaTypeInterfaceForType<T, QtPrivate::TypeAndForceComplete<T, std::false_type>>();

    template<class T>
    constexpr void addMetaType() {
        *metaTypeP++ = metaTypeInterface<T>;
        metaTypeCount += 1;
    }
#endif
};


/// fold ObjectInfo into State
template<typename T, typename State>
constexpr void generateDataPass(State& state) {
    using ObjI = typename T::W_MetaObjectCreatorHelper::ObjectInfo;
    constexpr size_t L = ObjI::counter;
    constexpr bool hasNotify = hasNotifySignal<L, T**>();
    constexpr int classInfoOffset = 14;
    constexpr int methodOffset = classInfoOffset + ObjI::classInfoCount * 2;
    constexpr int propertyOffset = methodOffset + ObjI::methodCount * (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) ? 6 : 5);
    constexpr int enumOffset = propertyOffset + ObjI::propertyCount * (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) ? 5 : (hasNotify ? 4: 3));
    constexpr int constructorOffset = enumOffset + ObjI::enumCount * (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) ? 5 : 4);
    constexpr int paramIndex = constructorOffset + ObjI::constructorCount * (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) ? 6 : 5);
    constexpr int constructorParamIndex = paramIndex + methodsParamOffset<L, T**>();
    constexpr int enumValueOffset = constructorParamIndex + constructorParamOffset<L, T**>();

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    state.addInts(9); // revision
#else
    state.addInts(QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) ? 8 : 7); // revision
#endif
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

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    foldState<L, PropertyStateTag, T**>(PropertyMetaTypeGenerator<State, T>{state});
#endif

    //if (state.intCount != methodOffset) throw "offset mismatch!";
    foldMethods<L, T**>(MethodGenerator<State, T>{state, paramIndex});

    //if (state.intCount != propertyOffset) throw "offset mismatch!";
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    foldState<L, PropertyStateTag, T**>(PropertyGenerator<State, L, T>{state});
#else
    foldState<L, PropertyStateTag, T**>(PropertyGenerator<State, T>{state});
    foldState<L, PropertyStateTag, T**>(NotifySignalGenerator<State, L, T, hasNotify>{state});
#endif

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

#if __cplusplus > 201700L
template<class T>
constexpr LayoutBuilder dataLayout = [](){
    auto r = LayoutBuilder{};
    generateDataPass<T>(r);
    return r;
}();
#else
template<class T>
constexpr auto createLayout() {
    auto r = LayoutBuilder{};
    generateDataPass<T>(r);
    return r;
}
template<class T>
constexpr LayoutBuilder dataLayout = createLayout<T>();
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
struct OffsetLenPair {
    uint offset;
    uint length;
};
#endif

/// Final data holder
template<std::size_t StringSize, std::size_t StringCount, std::size_t IntCount>
struct MetaDataHolder {
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    RawArray<OffsetLenPair, StringCount+1> stringOffsetLens;
#else
    RawArray<QByteArrayData,StringCount> byteArrays;
#endif
    OwnArray<char, StringSize> stringChars;
#if __cplusplus > 201700L
    const uint* ints;
#else
    OwnArray<uint, IntCount> ints;
#endif
};
template<class T>
struct MetaDataProvider {
    static constexpr auto stringSize = dataLayout<T>.stringSize;
    static constexpr auto stringCount = dataLayout<T>.stringCount;
    static constexpr auto intCount = dataLayout<T>.intCount;
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    static constexpr auto metaTypeCount = dataLayout<T>.metaTypeCount == 0 ? 1 : dataLayout<T>.metaTypeCount;
#endif
    using MetaDataType = const MetaDataHolder<stringSize, stringCount, intCount>;

    struct Arrays {
#ifndef Q_CC_MSVC
        constexpr static qptrdiff stringOffset = offsetof(MetaDataType, stringChars);
#else // offsetof does not work with MSVC
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
        constexpr static qptrdiff stringOffset = sizeof(MetaDataType::stringOffsetLens);
#else
        constexpr static qptrdiff stringOffset = sizeof(MetaDataType::byteArrays);
#endif
#endif
        RawArray<qptrdiff, stringCount> stringOffsets{};
        RawArray<int, stringCount> stringLengths{};
        RawArray<char, stringSize> stringChars{};
        RawArray<uint, intCount> ints{};
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
        RawArray<const QtPrivate::QMetaTypeInterface *, metaTypeCount> metaTypes{};
#endif
    };
#if __cplusplus > 201700L
    constexpr static Arrays arrays = []() {
        auto r = Arrays{};
        DataBuilder b{r};
        generateDataPass<T>(b);
        return r;
    }();
#else
    constexpr static auto buildArrays() {
        auto r = Arrays{};
        DataBuilder b{r};
        generateDataPass<T>(b);
        return r;
    }
    constexpr static Arrays arrays = buildArrays();
#endif
};

template<class T, class IS>
struct MetaDataBuilder;
template<class T, std::size_t... Is>
struct MetaDataBuilder<T, index_sequence<Is...>> {
    using P = MetaDataProvider<T>;
    using MetaDataType = typename P::MetaDataType;
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    constexpr static MetaDataType meta_data = {
        {
            OffsetLenPair{static_cast<uint>(P::arrays.stringOffsets[Is]), P::arrays.stringLengths[Is]}...,
            OffsetLenPair{static_cast<uint>(P::arrays.stringOffsets[sizeof...(Is)-1] + P::arrays.stringLengths[sizeof...(Is)-1]), 0},
        },
        P::arrays.stringChars,
        P::arrays.ints
    };
#elif __cplusplus > 201700L
    constexpr static MetaDataType meta_data = {
        {Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(P::arrays.stringLengths[Is], P::arrays.stringOffsets[Is])...},
        P::arrays.stringChars, P::arrays.ints
    };
#else
    static MetaDataType meta_data;
};
template<class T, std::size_t... Is>
typename MetaDataBuilder<T, index_sequence<Is...>>::MetaDataType MetaDataBuilder<T, index_sequence<Is...>>::meta_data = {
    {Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(P::arrays.stringLengths[Is], P::arrays.stringOffsets[Is])...},
    {P::arrays.stringChars}, {P::arrays.ints}
#endif
};

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
    static constexpr QMetaObject createMetaObject() {
        using MetaData = MetaDataBuilder<T, make_index_sequence<dataLayout<T>.stringCount>>;
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
        using P = MetaDataProvider<T>;
        return { {
            parentMetaObject<T>(0),
            &MetaData::meta_data.stringOffsetLens[0].offset,
            MetaData::meta_data.ints,
            T::qt_static_metacall,
            nullptr,
            P::arrays.metaTypes,
            nullptr,
        } };
#elif __cplusplus > 201700L
        return { { parentMetaObject<T>(0), MetaData::meta_data.byteArrays, MetaData::meta_data.ints, T::qt_static_metacall, {}, {} } };
#else
        return { { parentMetaObject<T>(0), MetaData::meta_data.byteArrays, MetaData::meta_data.ints.data, T::qt_static_metacall, {}, {} } };
#endif
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
        } else if (isPropertyMetacall(_c)) {
            constexpr int propertyCount = ObjI::propertyCount;
            if (_id < propertyCount)
                T::qt_static_metacall(_o, _c, _id, _a);
            _id -= propertyCount;
        }
        return _id;
    }

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Waddress")
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
QT_WARNING_POP

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
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
            *reinterpret_cast<QMetaType *>(_a[0]) = _t && arg < P::ArgumentCount ? QMetaType(_t[arg]) : QMetaType();
#else
            *reinterpret_cast<int*>(_a[0]) = _t && arg < P::ArgumentCount ?
                    _t[arg] : -1;
#endif
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
#if __cplusplus > 201700L
            (invokeMethod<T, MethI>(reinterpret_cast<T*>(_o), _id, _a),...);
#else
            ordered((invokeMethod<T, MethI>(reinterpret_cast<T*>(_o), _id, _a),0)...);
#endif
        } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
#if __cplusplus > 201700L
            (registerMethodArgumentType<T,MethI>(_id, _a),...);
#else
            ordered((registerMethodArgumentType<T,MethI>(_id, _a),0)...);
#endif
        } else if (_c == QMetaObject::IndexOfMethod) {
#if __cplusplus > 201700L
            *reinterpret_cast<int *>(_a[0]) = ((1+indexOfMethod<T,MethI>(reinterpret_cast<void **>(_a[1]))) + ... + 0)-1;
#else
            auto r = int{-1};
            ordered2<int>({(r += (1+indexOfMethod<T,MethI>(reinterpret_cast<void **>(_a[1]))))...});
            *reinterpret_cast<int *>(_a[0]) = r;
#endif
        } else if (_c == QMetaObject::CreateInstance) {
#if __cplusplus > 201700L
            (createInstance<T, ConsI>(_id, _a),...);
#else
            ordered((createInstance<T, ConsI>(_id, _a),0)...);
#endif
        } else if (isPropertyMetacall(_c)) {
#if __cplusplus > 201700L
            (propertyOperation<T,PropI>(static_cast<T*>(_o), _c, _id, _a),...);
#else
            ordered((propertyOperation<T,PropI>(static_cast<T*>(_o), _c, _id, _a),0)...);
#endif
        }
    }

    // Same for W_GADGET
    template<typename T, size_t...MethI, size_t ...ConsI, size_t...PropI>
    static void qt_static_metacall_impl2(T *_o, QMetaObject::Call _c, int _id, void** _a,
                            std::index_sequence<MethI...>, std::index_sequence<ConsI...>, std::index_sequence<PropI...>) {
        Q_UNUSED(_id) Q_UNUSED(_o) Q_UNUSED(_a)
        if (_c == QMetaObject::InvokeMetaMethod) {
#if __cplusplus > 201700L
            (invokeMethod<T, MethI>(_o, _id, _a),...);
#else
            ordered((invokeMethod<T, MethI>(_o, _id, _a), 0)...);
#endif
        } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
#if __cplusplus > 201700L
            (registerMethodArgumentType<T,MethI>(_id, _a),...);
#else
            ordered((registerMethodArgumentType<T,MethI>(_id, _a), 0)...);
#endif
        } else if (_c == QMetaObject::IndexOfMethod) {
            Q_ASSERT_X(false, "qt_static_metacall", "IndexOfMethod called on a Q_GADGET");
        } else if (_c == QMetaObject::CreateInstance) {
#if __cplusplus > 201700L
            (createInstance<T, ConsI>(_id, _a),...);
#else
            ordered((createInstance<T, ConsI>(_id, _a), 0)...);
#endif
        } else if (isPropertyMetacall(_c)) {
#if __cplusplus > 201700L
            (propertyOperation<T,PropI>(_o, _c, _id, _a),...);
#else
            ordered((propertyOperation<T,PropI>(_o, _c, _id, _a), 0)...);
#endif
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
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
        using MetaData = MetaDataBuilder<T, make_index_sequence<dataLayout<T>.stringCount>>;
        if (!strcmp(_clname, MetaData::meta_data.stringChars.data))
            return o;
#else
        const QByteArrayDataPtr sd = { const_cast<QByteArrayData*>(T::staticMetaObject.d.stringdata) };
        if (_clname == QByteArray(sd))
            return o;
#endif
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
        struct Name { \
            static constexpr auto defaultName = w_internal::viewLiteral(W_MACRO_STRIGNIFY(W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__))); \
            static constexpr auto value = w_internal::fetchExplicitName<typename W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::W_ThisType>(defaultName, 0); \
        }; \
        using ObjectInfo = w_internal::ObjectInfo<typename W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::W_ThisType, Name>; \
    }; \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) INLINE const QT_INIT_METAOBJECT QMetaObject W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::staticMetaObject = \
        w_internal::FriendHelper::createMetaObject<typename W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::W_ThisType>();

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
