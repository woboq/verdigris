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
#ifndef Q_MOC_RUN // don't define anything when moc is run

#include "wobjectdefs.h"
#include <QtCore/qobject.h>

namespace w_internal {

#if __cplusplus > 201700L
#define W_IF_CONSTEXPR constexpr
#else
#define W_IF_CONSTEXPR
#endif

// Match MetaDataFlags constants form the MetaDataFlags in qmetaobject_p.h
enum : uint { IsUnresolvedType = 0x80000000, IsUnresolvedNotifySignal = 0x70000000 };

/// all details about a class T
template<class T, size_t L = 1024 * 1024 * 1024> struct ObjectInfo {
    using Type = T;
    static constexpr auto counter = L;

    static constexpr int signalCount = stateCount<L, SignalStateTag, T**>();
    static constexpr int slotCount = stateCount<L, SlotStateTag, T**>();
    static constexpr int methodCount = signalCount + slotCount + stateCount<L, MethodStateTag, T**>();
    static constexpr int constructorCount = stateCount<L, ConstructorStateTag, T**>();
    static constexpr int propertyCount = stateCount<L, PropertyStateTag, T**>();
    static constexpr int enumCount = stateCount<L, EnumStateTag, T**>();
    static constexpr int classInfoCount = stateCount<L, ClassInfoStateTag, T**>();
    static constexpr int interfaceCount = stateCount<L, InterfaceStateTag, T**>();

    template<size_t Idx> static consteval auto method(Index<Idx>) {
        using TPP = T**;
        if constexpr (Idx < signalCount)
            return w_state(index<Idx>, SignalStateTag{}, TPP{});
        else if constexpr (Idx - signalCount < slotCount)
            return w_state(index<Idx - signalCount>, SlotStateTag{}, TPP{});
        else
            return w_state(index<Idx - signalCount - slotCount>, MethodStateTag{}, TPP{});
    }
};

constexpr bool isPropertyMetacall(QMetaObject::Call call) noexcept {
    return (call >= QMetaObject::ReadProperty && call <= QMetaObject::ResetProperty) ||
        call == QMetaObject::RegisterPropertyMetaType || call == QMetaObject::BindableProperty;
}

template<class OPP, size_t SigIdx, auto notify>
concept IsNotifySignal = (w_state(index<SigIdx>, SignalStateTag{}, OPP{}).getFunc() == notify);

/// Helper to get information about the notify signal of the property within object T
template<size_t L, size_t PropIdx, typename TPP, typename OPP> consteval int resolveNotifySignal() {
    return []<size_t... Is>(const index_sequence<Is...>&)->int {
        int r = -1;
        constexpr auto notify = w_state(index<PropIdx>, PropertyStateTag{}, TPP{}).notify;
        ((IsNotifySignal<OPP, Is, notify> ? r = (int)Is : 0), ...);
        return r;
    }
    (make_index_sequence<stateCount<L, SignalStateTag, OPP>()>{});
}

/// returns true if the object T has at least one property with a notify signal
template<size_t L, typename TPP> consteval bool hasNotifySignal() {
    return []<size_t... Is>(const index_sequence<Is...>&)->bool {
        return (false || ... || (w_state(index<Is>, PropertyStateTag{}, TPP{}).notify != nullptr));
    }
    (make_index_sequence<stateCount<L, PropertyStateTag, TPP>()>{});
}

template<class State> struct ClassInfoGenerator {
    State& s;

    template<class ClassInfo> constexpr void operator()(const ClassInfo& ci) {
        s.addString(ci.first);
        s.addString(ci.second);
    }
};

/// auto-detect the access specifiers
template<class T, class State, size_t I>
concept IsPublic = requires(T** tpp) {
    T::w_GetAccessSpecifierHelper(index<I>, State{}, tpp);
};

template<class T, class State, size_t I> struct Derived : T {
    static constexpr bool w_accessOracle = requires(T * *tpp) {
        T::w_GetAccessSpecifierHelper(index<I>, State{}, tpp);
    };
};
template<class T, class State, size_t I>
concept IsProtected = !std::is_final_v<T> && Derived<T, State, I>::w_accessOracle;

template<class Builder, class T> struct MethodGenerator {
    Builder& s;
    int parameterIndex;

    template<class Method, class State, size_t I> constexpr void operator()(const Method& method, State, Index<I>) {
        using FP = QtPrivate::FunctionPointer<typename Method::Func>;
        s.addString(method.name); // name
        constexpr uint flags = adjustFlags<State, I>(Method::flags);
        s.addInts(
            (uint)FP::ArgumentCount,
            parameterIndex, // parameters
            1, // tag, always \0
            flags,
            s.metaTypeCount);
        parameterIndex += 1 + FP::ArgumentCount * 2;
        s.template addMetaType<typename FP::ReturnType>();
        using ArgsPtr = typename FP::Arguments*;
        [this]<class... Args>(QtPrivate::List<Args...>*) { (s.template addMetaType<Args>(), ...); }(ArgsPtr{});
    }

    template<class... Args> constexpr void operator()(const MetaConstructorInfo<Args...>& method) {
        s.addString(method.name); // name
        s.addInts(
            (uint)sizeof...(Args),
            parameterIndex, // parameters
            1, // tag, always \0
            MethodConstructor | AccessPublic,
            s.metaTypeCount);
        parameterIndex += 1 + sizeof...(Args) * 2;
        (s.template addMetaType<Args>(), ...);
    }

private:
    template<class State, size_t I> static constexpr uint adjustFlags(uint f) {
        if (!(f & (AccessProtected | AccessPrivate | AccessPublic))) {
            // Auto-detect the access specifier
            f |= IsPublic<T, State, I> ? AccessPublic : IsProtected<T, State, I> ? AccessProtected : AccessPrivate;
        }
        f &= static_cast<uint>(~AccessPrivate); // Because QMetaMethod::Private is 0, but not AccessPrivate;
        return f;
    }
};

/// compute if type T is a builtin QMetaType
template<class T>
concept BuiltinMetaTypeId = bool{QMetaTypeId2<T>::IsBuiltIn};

/// Helper to generate the type information of type 'T':
/// If T is a builtin QMetaType, its meta type id need to be added in the state.
/// If it's not builtin, then it would be an reference to T's name.
template<typename T, typename State, typename TypeStr = int> constexpr void handleType(State& s, TypeStr v = {}) {
    (void)v;
    if constexpr (BuiltinMetaTypeId<T>) {
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

template<class State, size_t L, class T> struct PropertyGenerator {
    State& s;

    template<class Prop, size_t Idx> constexpr void operator()(const Prop& prop, Index<Idx>) {
        s.addString(prop.name);
        handleType<typename Prop::PropertyType>(s, prop.typeStr);

        constexpr int moreFlags =
            QtPrivate::IsQEnumHelper<typename Prop::PropertyType>::Value ? 0 | PropertyFlags::EnumOrFlag : 0;
        constexpr int finalFlag = std::is_final<T>::value ? 0 | PropertyFlags::Final : 0;
        constexpr int defaultFlags = 0 | PropertyFlags::Stored | PropertyFlags::Scriptable | PropertyFlags::Designable;
        s.addInts(Prop::flags | moreFlags | finalFlag | defaultFlags);
        using Notify = decltype(prop.notify);
        if constexpr (std::is_same_v<Notify, Empty>) {
            s.addInts(-1);
        }
        else {
            using O = typename QtPrivate::FunctionPointer<Notify>::Object;
            constexpr int signalIndex = resolveNotifySignal<L, Idx, T**, O**>();
            static_assert(signalIndex >= 0, "NOTIFY signal in parent class not registered as a W_SIGNAL");
            if constexpr (std::is_same_v<T, O>) {
                s.addInts(signalIndex); // // Signal in the same class
            }
            else {
                using OPP = O**;
                constexpr auto sig = w_state(index<signalIndex>, SignalStateTag{}, OPP{});
                s.template addTypeString<IsUnresolvedNotifySignal>(sig.name);
            }
        }
        s.addInts(0); // revision
    }
};

template<class State, class T> struct PropertyMetaTypeGenerator {
    State& s;

    template<class Prop> constexpr void operator()(const Prop&) {
        s.template addMetaType<typename Prop::PropertyType>();
    }
};

template<class State> struct EnumGenerator {
    State& s;
    int dataIndex{};

    template<class Enum> constexpr void operator()(const Enum& e) {
        auto nameIndex = s.stringCount; // required for MSVC-2019
        (void)nameIndex;
        s.addString(e.name); // name
        if constexpr (Enum::hasAlias)
            s.addString(e.alias); // alias
        else
            s.addInts(nameIndex);
        s.addInts(Enum::flags, (uint)Enum::count, dataIndex);
        dataIndex += Enum::count * 2;
    }
};

template<class State> struct EnumValuesGenerator {
    State& s;

    template<class Enum> constexpr void operator()(const Enum& e) {
        generateAll(typename Enum::Values{}, e.names, make_index_sequence<Enum::count>{});
    }

private:
    template<class Enum, Enum... Values, typename Names, size_t... Is>
    constexpr void generateAll(enum_sequence<Enum, Values...>, const Names& names, const index_sequence<Is...>&) {
        ((s.addString(names[Is]), s.addInts(static_cast<uint>(Values))), ...);
    }
};

template<size_t I, size_t N> constexpr auto stringFetch(const StringViewArray<N>& s) {
    if constexpr (I < N) {
        return s[I];
    }
    else {
        (void)s;
        struct _ {};
        return _{};
    }
}

template<class Arg, class State, class TypeName> constexpr void handleArgType(State& ss, const TypeName& typeName) {
    using Type = typename QtPrivate::RemoveConstRef<Arg>::Type;
    // This way, the overload of result will not pick the StringView one if it is a tuple (because registered types have
    // the priority)
    auto typeName2 = std::conditional_t<std::is_same<Arg, Type>::value, TypeName, std::tuple<TypeName>>{typeName};
    handleType<Type>(ss, typeName2);
}
template<class... Args, class State, class TypeNames, size_t... Is>
constexpr void handleArgTypes(State& ss, const TypeNames& typeNames, const index_sequence<Is...>&) {
    (handleArgType<Args>(ss, stringFetch<Is>(typeNames)), ...);
}

template<size_t ArgCount, class State, size_t NameCount>
constexpr void handleArgNames(State& ss, const StringViewArray<NameCount>& paramNames) {
    auto i = size_t{};
    for (; i < ArgCount && i < NameCount; ++i) ss.addString(paramNames[i]);
    for (; i < ArgCount; ++i) ss.addInts(1);
}

template<class State> struct MethodParametersGenerator {
    State& s;

    template<class Method> constexpr void operator()(const Method& method) {
        using FP = QtPrivate::FunctionPointer<typename Method::Func>;
        handleType<typename FP::ReturnType>(s);
        using ArgsPtr = typename FP::Arguments*;
        [this, &method]<class... Args>(QtPrivate::List<Args...>*) {
            handleArgTypes<Args...>(s, method.paramTypes, make_index_sequence<sizeof...(Args)>{});
        }(ArgsPtr{});
        handleArgNames<FP::ArgumentCount>(s, method.paramNames);
    }
};

template<class State> struct ConstructorParametersGenerator {
    State& s;

    template<typename... Args> constexpr void operator()(const MetaConstructorInfo<Args...>&) {
        s.addInts(IsUnresolvedType | 1);
        handleArgTypes<Args...>(s, StringViewArray<>{}, make_index_sequence<sizeof...(Args)>{});
        s.addInts(((void)sizeof(Args), 1)...); // all the names are 1 (for "\0")
    }
};

/// Given method, a binary::tree containing information about methods or constructor,
/// return the amount of item it will add in the int array.
template<size_t L, class T> consteval int methodsParamOffset() {
    return []<size_t... Is>(const index_sequence<Is...>&)->int {
        return (
            0 + ... +
            int(1 +
                QtPrivate::FunctionPointer<typename decltype(ObjectInfo<T, L>::method(
                        index<Is>))::Func>::ArgumentCount *
                    2));
    }
    (make_index_sequence<ObjectInfo<T, L>::methodCount>{});
}

template<size_t L, class T> consteval int constructorParamOffset() {
    using TPP = T**;
    return []<size_t... Is>(const index_sequence<Is...>&)->int {
        return (0 + ... + int(1 + w_state(index<Is>, ConstructorStateTag{}, TPP{}).argCount * 2));
    }
    (make_index_sequence<ObjectInfo<T, L>::constructorCount>{});
}

template<class T, size_t N> using RawArray = T[N];

struct LayoutBuilder {
    size_t stringSize{};
    uint stringCount{};
    uint intCount{};
    uint metaTypeCount{};

    constexpr void addString(const StringView& s) {
        stringSize += s.size() + 1;
        stringCount += 1;
        intCount += 1;
    }
    constexpr void addStringUntracked(const StringView& s) {
        stringSize += s.size() + 1;
        stringCount += 1;
    }
    template<uint Flag = IsUnresolvedType> constexpr void addTypeString(const StringView& s) {
        stringSize += s.size() + 1;
        stringCount += 1;
        intCount += 1;
    }
    template<class... Ts> constexpr void addInts(Ts...) { intCount += sizeof...(Ts); }
    template<class T, bool = false> constexpr void addMetaType() { metaTypeCount += 1; }
};

template<class T>
concept IsCompleteType = sizeof(T) >= 0;

struct OffsetLenPair {
    uint offset{};
    uint length{};
};
template<class MetaData, size_t initStringOffset> struct DataBuilder {
    char* stringCharP{};
    OffsetLenPair* offsetLenPairP{};
    uint* intP{};
    const QtPrivate::QMetaTypeInterface** metaTypeP{};
    uint stringCount{};
    uint intCount{};
    uint metaTypeCount{};

    constexpr DataBuilder(MetaData& r)
        : stringCharP(r.stringChars)
        , offsetLenPairP(r.stringOffsetLens)
        , intP(r.data)
        , metaTypeP(r.metaTypes) {
        offsetLenPairP->offset = initStringOffset; // offset + len==0 mark the end
    }

    constexpr void addString(const StringView& s) {
        for (auto c : s) *stringCharP++ = c;
        *stringCharP++ = '\0';
        offsetLenPairP->length = s.size();
        offsetLenPairP[1].offset = offsetLenPairP->offset + 1 + s.size();
        ++offsetLenPairP;
        *intP++ = stringCount;
        stringCount += 1;
        intCount += 1;
    }
    constexpr void addStringUntracked(const StringView& s) {
        for (auto c : s) *stringCharP++ = c;
        *stringCharP++ = '\0';
        offsetLenPairP->length = s.size();
        offsetLenPairP[1].offset = offsetLenPairP->offset + 1 + s.size();
        ++offsetLenPairP;
        stringCount += 1;
    }

    template<uint Flag = IsUnresolvedType> constexpr void addTypeString(const StringView& s) {
        for (auto c : s) *stringCharP++ = c;
        *stringCharP++ = '\0';
        offsetLenPairP->length = s.size();
        offsetLenPairP[1].offset = offsetLenPairP->offset + 1 + s.size();
        ++offsetLenPairP;
        *intP++ = Flag | stringCount;
        stringCount += 1;
        intCount += 1;
    }
    template<class... Ts> constexpr void addInts(Ts... vs) {
        ((*intP++ = vs), ...);
        intCount += sizeof...(Ts);
    }

    template<class T, bool forceComplete = false> constexpr void addMetaType() {
        // mirrors behaviour of QtPrivate::qTryMetaTypeInterfaceForType (qmetatype.h)
        // - but uses C++20 to be faster
        if constexpr (forceComplete || IsCompleteType<QtPrivate::qRemovePointerLike_t<std::remove_cvref_t<T>>>) {
            *metaTypeP++ = &QtPrivate::QMetaTypeInterfaceWrapper<std::remove_cvref_t<T>>::metaType;
        }
        else {
            *metaTypeP++ = nullptr;
        }
        metaTypeCount += 1;
    }
};

template<class TP>
concept HasExplicitName = requires(StringView result) {
    result = StringView{w_explicitObjectName(TP{})};
};

/// fold ObjectInfo into State
template<class T, class Result, class Builder> consteval auto generateDataPass() -> Result {
    auto result = Result{};
    Builder builder{result};
    using ObjI = ObjectInfo<T>;
    constexpr size_t L = ObjI::counter;
    constexpr bool hasNotify = hasNotifySignal<L, T**>();
    constexpr int classInfoOffset = 14;
    constexpr int methodOffset = classInfoOffset + ObjI::classInfoCount * 2;
    constexpr int propertyOffset = methodOffset + ObjI::methodCount * (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) ? 6 : 5);
    constexpr int enumOffset =
        propertyOffset + ObjI::propertyCount * (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) ? 5 : (hasNotify ? 4 : 3));
    constexpr int constructorOffset = enumOffset + ObjI::enumCount * 5;
    constexpr int paramIndex =
        constructorOffset + ObjI::constructorCount * (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) ? 6 : 5);
    constexpr int constructorParamIndex = paramIndex + methodsParamOffset<L, T>();
    constexpr int enumValueOffset = constructorParamIndex + constructorParamOffset<L, T>();

    builder.addInts(QT_VERSION >= QT_VERSION_CHECK(6, 2, 0) ? 10 : 9); // revision
    using TP = T*;
    if constexpr (HasExplicitName<TP>) {
        builder.addString(w_explicitObjectName(TP{}));
    }
    else {
        builder.addString(T::W_MetaObjectCreatorHelper::fullName);
    }
    builder.addStringUntracked(viewLiteral(""));
    builder.addInts(
        ObjI::classInfoCount,
        classInfoOffset, // classinfo
        ObjI::methodCount,
        methodOffset, // methods
        ObjI::propertyCount,
        propertyOffset, // properties
        ObjI::enumCount,
        enumOffset, // enums/sets
        ObjI::constructorCount,
        constructorOffset, // constructors
        0x4 /* PropertyAccessInStaticMetaCall */, // flags
        ObjI::signalCount);

    using TPP = T**;
    constexpr auto fold = []<class State>(State, auto&& f) {
        [&f]<size_t... Is>(const index_sequence<Is...>&) { (f(w_state(index<Is>, State{}, TPP{})), ...); }
        (make_index_sequence<stateCount<L, State, TPP>()>{});
    };
    constexpr auto foldIndex = []<class State>(State, auto&& f) {
        [&f]<size_t... Is>(const index_sequence<Is...>&) { (f(w_state(index<Is>, State{}, TPP{}), index<Is>), ...); }
        (make_index_sequence<stateCount<L, State, TPP>()>{});
    };
    constexpr auto foldMethod = [](auto&& f) {
        [&f]<size_t... Is>(const index_sequence<Is...>&) { (f(ObjI::method(index<Is>)), ...); }
        (make_index_sequence<ObjI::methodCount>{});
    };
    constexpr auto foldMethodIndex = [](auto&& f) {
        constexpr auto foldStateIndex = []<class State>(State, auto&& f) {
            [&f]<size_t... Is>(const index_sequence<Is...>&) {
                (f(w_state(index<Is>, State{}, TPP{}), State{}, index<Is>), ...);
            }
            (make_index_sequence<stateCount<ObjI::counter, State, TPP>()>{});
        };
        foldStateIndex(SignalStateTag{}, f);
        foldStateIndex(SlotStateTag{}, f);
        foldStateIndex(MethodStateTag{}, f);
    };

    // if (state.intCount != classInfoOffset) throw "offset mismatch!";
    fold(ClassInfoStateTag{}, ClassInfoGenerator<Builder>{builder});
    fold(PropertyStateTag{}, PropertyMetaTypeGenerator<Builder, T>{builder});

#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
    builder.template addMetaType<T, true>();
#endif

    // if (state.intCount != methodOffset) throw "offset mismatch!";
    foldMethodIndex(MethodGenerator<Builder, T>{builder, paramIndex});

    // if (state.intCount != propertyOffset) throw "offset mismatch!";
    foldIndex(PropertyStateTag{}, PropertyGenerator<Builder, L, T>{builder});

    // if (state.intCount != enumOffset) throw "offset mismatch!";
    fold(EnumStateTag{}, EnumGenerator<Builder>{builder, enumValueOffset});

    // if (state.intCount != constructorOffset) throw "offset mismatch!";
    fold(ConstructorStateTag{}, MethodGenerator<Builder, T>{builder, constructorParamIndex});

    // if (state.intCount != paramIndex) throw "offset mismatch!";
    foldMethod(MethodParametersGenerator<Builder>{builder});

    // if (state.intCount != constructorParamIndex) throw "offset mismatch!";
    fold(ConstructorStateTag{}, ConstructorParametersGenerator<Builder>{builder});

    // if (state.intCount != enumValueOffset) throw "offset mismatch!";
    fold(EnumStateTag{}, EnumValuesGenerator<Builder>{builder});

    return result;
}

/// Final data holder
template<class T> static constexpr auto buildMetaData() {
    constexpr auto dataLayout = generateDataPass<T, LayoutBuilder, LayoutBuilder&>();
    constexpr auto metaTypeCount = dataLayout.metaTypeCount == 0 ? 1 : dataLayout.metaTypeCount;
    struct MetaData {
        RawArray<OffsetLenPair, dataLayout.stringCount + 1> stringOffsetLens;
        RawArray<char, dataLayout.stringSize> stringChars;
        RawArray<uint, dataLayout.intCount> data;
        RawArray<const QtPrivate::QMetaTypeInterface*, metaTypeCount> metaTypes;
    };
#ifndef Q_CC_MSVC
    constexpr qptrdiff stringOffset = offsetof(MetaData, stringChars);
#else // offsetof does not work with MSVC
    constexpr qptrdiff stringOffset = sizeof(MetaData::stringOffsetLens);
#endif

    constexpr auto result = generateDataPass<T, MetaData, DataBuilder<MetaData, stringOffset>>();
    return result;
};
template<class T> static constexpr auto metaData = buildMetaData<T>();

/// Returns the QMetaObject* of the base type
template<typename T>
concept WithParentMetaObject = requires(const QMetaObject* result) {
    result = &T::W_BaseType::staticMetaObject;
};
template<typename T> static consteval const QMetaObject* parentMetaObject() {
    if constexpr (WithParentMetaObject<T>) {
        return &T::W_BaseType::staticMetaObject;
    }
    else {
        return nullptr;
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
// note: Qt 6.3 introduced a check here that allows only QObjects - but we need it for Gadgets as well
template<typename, typename, typename, typename> struct FunctorCall;
template<size_t... II, typename... Args, typename R, typename Function>
struct FunctorCall<std::index_sequence<II...>, QtPrivate::List<Args...>, R, Function> {
    static void call(Function f, void** arg) {
        f((*reinterpret_cast<std::remove_reference_t<Args>*>(arg[II + 1]))...), QtPrivate::ApplyReturnValue<R>(arg[0]);
    }
};
template<size_t... II, typename... Args, typename R, typename... SlotArgs, typename SlotRet, class Obj>
struct FunctorCall<std::index_sequence<II...>, QtPrivate::List<Args...>, R, SlotRet (Obj::*)(SlotArgs...)> {
    static void call(SlotRet (Obj::*f)(SlotArgs...), Obj* o, void** arg) {
        (o->*f)((*reinterpret_cast<std::remove_reference_t<Args>*>(arg[II + 1]))...),
            QtPrivate::ApplyReturnValue<R>(arg[0]);
    }
};
template<size_t... II, typename... Args, typename R, typename... SlotArgs, typename SlotRet, class Obj>
struct FunctorCall<std::index_sequence<II...>, QtPrivate::List<Args...>, R, SlotRet (Obj::*)(SlotArgs...) const> {
    static void call(SlotRet (Obj::*f)(SlotArgs...) const, Obj* o, void** arg) {
        (o->*f)((*reinterpret_cast<std::remove_reference_t<Args>*>(arg[II + 1]))...),
            QtPrivate::ApplyReturnValue<R>(arg[0]);
    }
};
template<size_t... II, typename... Args, typename R, typename... SlotArgs, typename SlotRet, class Obj>
struct FunctorCall<std::index_sequence<II...>, QtPrivate::List<Args...>, R, SlotRet (Obj::*)(SlotArgs...) noexcept> {
    static void call(SlotRet (Obj::*f)(SlotArgs...) noexcept, Obj* o, void** arg) {
        (o->*f)((*reinterpret_cast<std::remove_reference_t<Args>*>(arg[II + 1]))...),
            QtPrivate::ApplyReturnValue<R>(arg[0]);
    }
};
template<size_t... II, typename... Args, typename R, typename... SlotArgs, typename SlotRet, class Obj>
struct FunctorCall<
    std::index_sequence<II...>,
    QtPrivate::List<Args...>,
    R,
    SlotRet (Obj::*)(SlotArgs...) const noexcept> {
    static void call(SlotRet (Obj::*f)(SlotArgs...) const noexcept, Obj* o, void** arg) {
        (o->*f)((*reinterpret_cast<std::remove_reference_t<Args>*>(arg[II + 1]))...),
            QtPrivate::ApplyReturnValue<R>(arg[0]);
    }
};
#endif

template<size_t N, class TPP> using InterfacePtr = decltype(w_state(index<N>, InterfaceStateTag{}, TPP{}));

struct FriendHelper {
    template<typename T> static consteval QMetaObject createMetaObject() {
        return {{
            .superdata = parentMetaObject<T>(),
            .stringdata = &metaData<T>.stringOffsetLens[0].offset,
            .data = metaData<T>.data,
            .static_metacall = T::qt_static_metacall,
            .relatedMetaObjects = nullptr,
            .metaTypes = metaData<T>.metaTypes,
            .extradata = nullptr,
        }};
    }

    template<typename T> static int qt_metacall_impl(T* _o, QMetaObject::Call _c, int _id, void** _a) {
        using ObjI = ObjectInfo<T>;
        _id = _o->T::W_BaseType::qt_metacall(_c, _id, _a);
        if (_id < 0) return _id;
        if (_c == QMetaObject::InvokeMetaMethod || _c == QMetaObject::RegisterMethodArgumentMetaType) {
            constexpr int methodCount = ObjI::methodCount;
            if (_id < methodCount) T::qt_static_metacall(_o, _c, _id, _a);
            _id -= methodCount;
        }
        else if (isPropertyMetacall(_c)) {
            constexpr int propertyCount = ObjI::propertyCount;
            if (_id < propertyCount) T::qt_static_metacall(_o, _c, _id, _a);
            _id -= propertyCount;
        }
        return _id;
    }

    QT_WARNING_PUSH
    QT_WARNING_DISABLE_GCC("-Waddress")
    /// Helper for implementation of qt_static_metacall for QMetaObject::IndexOfMethod
    /// T is the class, and I is the index of a method.
    /// Returns I if the argument is equal to the pointer to member function of the signal of index 'I'
    template<class ObjI, size_t I> static bool isSignalMethod(void** func) {
        static constexpr auto method = ObjI::method(index<I>);
        if constexpr ((method.flags & 0xc) == MethodSignal) {
            using Func = typename decltype(method)::Func;
            return (method.getFunc() == *reinterpret_cast<Func*>(func));
        }
        else {
            return false;
        }
    }
    QT_WARNING_POP

    /// Helper for implementation of qt_static_metacall for QMetaObject::InvokeMetaMethod
    /// T is the class, and I is the index of a method.
    template<class T, int I> static void invokeMethod(T* _o, void** _a) {
        static constexpr auto method = ObjectInfo<T>::method(index<I>);
        using Func = typename decltype(method)::Func;
        using FP = QtPrivate::FunctionPointer<Func>;
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
        if constexpr (FP::IsPointerToMemberFunction) {
            FunctorCall<make_index_sequence<FP::ArgumentCount>, typename FP::Arguments, typename FP::ReturnType, Func>::
                call(method.getFunc(), _o, _a);
        }
        else {
            (void)_o;
            FunctorCall<make_index_sequence<FP::ArgumentCount>, typename FP::Arguments, typename FP::ReturnType, Func>::
                call(method.getFunc(), _a);
        }
#else
        FP::template call<typename FP::Arguments, typename FP::ReturnType>(method.getFunc(), _o, _a);
#endif
    }

    /// Helper for implementation of qt_static_metacall for QMetaObject::RegisterMethodArgumentMetaType
    /// T is the class, and I is the index of a method.
    template<typename T, int I> static void registerMethodArgumentType(void** _a) {
        using Func = typename decltype(ObjectInfo<T>::method(index<I>))::Func;
        using P = QtPrivate::FunctionPointer<Func>;
        auto _t = QtPrivate::ConnectionTypes<typename P::Arguments>::types();
        uint arg = *reinterpret_cast<uint*>(_a[1]);
        *reinterpret_cast<QMetaType*>(_a[0]) = _t && arg < P::ArgumentCount ? QMetaType(_t[arg]) : QMetaType();
    }

    /// Helper for implementation of qt_static_metacall for any of the operations in a property
    /// T is the class, and I is the index of a property.
    template<typename T, int I> static void propertyOperation(T* _o, QMetaObject::Call _c, void** _a) {
        using TPP = T**;
        constexpr auto p = w_state(index<I>, PropertyStateTag{}, TPP{});
        using Type = typename decltype(p)::PropertyType;
        switch (+_c) {
        case QMetaObject::ReadProperty:
            if constexpr (p.getter != nullptr) {
                auto& t = *reinterpret_cast<Type*>(_a[0]);
                if constexpr (requires { t = (_o->*p.getter)(); }) {
                    t = (_o->*p.getter)();
                }
                else {
                    throw "wrong getter!";
                }
            }
            else if constexpr (p.member != nullptr) {
                auto& t = *reinterpret_cast<Type*>(_a[0]);
                if constexpr (requires { t = _o->*p.member; }) {
                    t = _o->*p.member;
                }
                else {
                    throw "wrong member!";
                }
            }
            break;
        case QMetaObject::WriteProperty:
            if constexpr (p.setter != nullptr) {
                const auto& t = *reinterpret_cast<Type*>(_a[0]);
                if constexpr (requires { (_o->*p.setter)(t); }) {
                    (_o->*p.setter)(t);
                }
                else {
                    throw "wrong setter!";
                }
            }
            else if constexpr (p.member != nullptr) {
                const auto& t = *reinterpret_cast<Type*>(_a[0]);
                if constexpr (requires { _o->*p.member = t; }) {
                    _o->*p.member = t;
                }
                else {
                    throw "wrong member!";
                }
                if constexpr (p.notify != nullptr) {
                    if constexpr (requires { (_o->*p.notify)(_o->*p.member); }) {
                        (_o->*p.notify)(_o->*p.member);
                    }
                    else if constexpr (requires { (_o->*p.notify)(); }) {
                        (_o->*p.notify)();
                    }
                    else {
                        throw "wrong notify!";
                    }
                }
            }
            break;
        case QMetaObject::ResetProperty:
            if constexpr (p.reset != nullptr) {
                if constexpr (requires { (_o->*p.reset)(); }) {
                    (_o->*p.reset)();
                }
                else {
                    throw "wrong reset!";
                }
            }
            break;
        case QMetaObject::RegisterPropertyMetaType:
            *reinterpret_cast<int*>(_a[0]) = QtPrivate::QMetaTypeIdHelper<Type>::qt_metatype_id();
        }
    }

    /// Helper for implementation of qt_static_metacall for QMetaObject::CreateInstance
    /// T is the class, and I is the index of a constructor.
    template<typename T, int I> static void createInstance(void** _a) {
        using TPP = T**;
        constexpr auto m = w_state(index<I>, ConstructorStateTag{}, TPP{});
        [&]<class... Args, size_t... Is>(const MetaConstructorInfo<Args...>&, const index_sequence<Is...>&) {
            *reinterpret_cast<T**>(_a[0]) = new T(*reinterpret_cast<std::remove_reference_t<Args>*>(_a[Is + 1])...);
        }(m, make_index_sequence<m.argCount>{});
    }

    template<typename T, class O>
    requires(
        std::is_same_v<T, O> ||
        (std::is_same_v<QObject, O> &&
         std::is_base_of_v<
             QObject,
             T>)) static void qt_static_metacall_impl(O* _o, QMetaObject::Call _c, int _id, void** _a) {
        Q_UNUSED(_id)
        Q_UNUSED(_o)
        Q_UNUSED(_a)
        using ObjI = ObjectInfo<T>;
        [&]<size_t... MethI, size_t... ConsI, size_t... PropI>(
            const std::index_sequence<MethI...>&,
            const std::index_sequence<ConsI...>&,
            const std::index_sequence<PropI...>&) {
            if (_c == QMetaObject::InvokeMetaMethod) {
                ((_id == MethI ? invokeMethod<T, MethI>(static_cast<T*>(_o), _a) : (void)0), ...);
            }
            else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
                ((_id == MethI ? registerMethodArgumentType<T, MethI>(_a) : (void)0), ...);
            }
            else if (_c == QMetaObject::IndexOfMethod) {
                if constexpr (std::is_same_v<QObject, O>) {
                    (void)((isSignalMethod<ObjI, MethI>(reinterpret_cast<void**>(_a[1])) ? (*reinterpret_cast<int*>(_a[0]) = MethI, true) : false) || ...);
                }
                else {
                    Q_ASSERT_X(false, "qt_static_metacall", "IndexOfMethod called on a Q_GADGET");
                }
            }
            else if (_c == QMetaObject::CreateInstance) {
                ((_id == ConsI ? createInstance<T, ConsI>(_a) : (void)0), ...);
            }
            else if (isPropertyMetacall(_c)) {
                ((_id == PropI ? propertyOperation<T, PropI>(static_cast<T*>(_o), _c, _a) : (void)0), ...);
            }
        }
        (make_index_sequence<ObjI::methodCount>{},
         make_index_sequence<ObjI::constructorCount>{},
         make_index_sequence<ObjI::propertyCount>{});
    }

    /// implementation of qt_metacast
    template<typename T> static void* qt_metacast_impl(T* o, const char* _clname) {
        if (!_clname) return nullptr;
        if (!strcmp(_clname, metaData<T>.stringChars)) return o;
        void* result = {};
        [&]<size_t... Is>(const std::index_sequence<Is...>&) {
            (((qobject_interface_iid<InterfacePtr<Is, T**>>() &&
               0 == strcmp(_clname, qobject_interface_iid<InterfacePtr<Is, T**>>()))
                  ? (void)(result = static_cast<InterfacePtr<Is, T**>>(o))
                  : (void)0),
             ...);
        }
        (make_index_sequence<ObjectInfo<T>::interfaceCount>{});
        if (!result) return o->T::W_BaseType::qt_metacast(_clname);
        return result;
    }

}; // FriendHelper

} // namespace w_internal

// W_OBJECT_IMPL was designed to take advantage of the GNU extension that ... can have zero arguments.
// So we need to work around that to extract the template stuff which may not exist or be composed
// of several macro arguments: If the first argument has parentheses, there must be at least  two
// arguments, so just do a tail. Otherwise, there should be only one or two argument, so take the second.
#define W_MACRO_TEMPLATE_STUFF(...)                                                                                    \
    W_MACRO_CONCAT(                                                                                                    \
        W_MACRO_TEMPLATE_STUFF_HELPER,                                                                                 \
        W_MACRO_DELAY(W_MACRO_TEMPLATE_STUFF_QUERY, W_MACRO_TEMPLATE_STUFF_HELPER __VA_ARGS__))                        \
    (__VA_ARGS__)
#define W_MACRO_TEMPLATE_STUFF_QUERY(...) W_MACRO_DELAY2(W_MACRO_FIRST, W_MACRO_TEMPLATE_STUFF_HELPER_##__VA_ARGS__)
#define W_MACRO_TEMPLATE_STUFF_HELPER(...) YES
#define W_MACRO_TEMPLATE_STUFF_HELPER_YES TAIL,
#define W_MACRO_TEMPLATE_STUFF_HELPER_W_MACRO_TEMPLATE_STUFF_HELPER SECOND,
#define W_MACRO_TEMPLATE_STUFF_HELPER_TAIL(...) W_MACRO_MSVC_EXPAND(W_MACRO_TAIL(__VA_ARGS__))
#define W_MACRO_TEMPLATE_STUFF_HELPER_SECOND(...)                                                                      \
    W_MACRO_MSVC_EXPAND(W_MACRO_TEMPLATE_STUFF_HELPER_SECOND2(__VA_ARGS__, , ))
#define W_MACRO_TEMPLATE_STUFF_HELPER_SECOND2(A, B, ...) B
#define W_MACRO_FIRST_REMOVEPAREN(...) W_MACRO_REMOVEPAREN(W_MACRO_FIRST(__VA_ARGS__))

#define W_OBJECT_IMPL_COMMON(INLINE, ...)                                                                              \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) struct W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::W_MetaObjectCreatorHelper {     \
        static constexpr auto fullName =                                                                               \
            w_internal::viewLiteral(W_MACRO_STRIGNIFY(W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)));                        \
    };                                                                                                                 \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__)                                                                                \
    INLINE const QMetaObject W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::staticMetaObject =                                \
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
#define W_OBJECT_IMPL(...)                                                                                             \
    W_OBJECT_IMPL_COMMON(W_MACRO_EMPTY, __VA_ARGS__)                                                                   \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__)                                                                                \
    void W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_static_metacall(                                                   \
        QObject* _o, QMetaObject::Call _c, int _id, void** _a) {                                                       \
        w_internal::FriendHelper::qt_static_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(_o, _c, _id, _a);    \
    }                                                                                                                  \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__)                                                                                \
    const QMetaObject* W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::metaObject() const { return &staticMetaObject; }        \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__)                                                                                \
    void* W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_metacast(const char* _clname) {                                   \
        return w_internal::FriendHelper::qt_metacast_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(this, _clname);      \
    }                                                                                                                  \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__)                                                                                \
    int W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_metacall(QMetaObject::Call _c, int _id, void** _a) {                \
        return w_internal::FriendHelper::qt_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(this, _c, _id, _a);  \
    }

/// \macro W_GADGET_IMPL(TYPE [, TEMPLATE_STUFF])
/// Same as W_OBJECT_IMPL, but for a W_GADGET
#define W_GADGET_IMPL(...)                                                                                             \
    W_OBJECT_IMPL_COMMON(W_MACRO_EMPTY, __VA_ARGS__)                                                                   \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__)                                                                                \
    void W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_static_metacall(                                                   \
        QObject* _o, QMetaObject::Call _c, int _id, void** _a) {                                                       \
        w_internal::FriendHelper::qt_static_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(                     \
            reinterpret_cast<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)*>(_o), _c, _id, _a);                               \
    }

/// \macro W_NAMESPACE_IMPL(...)
/// Same as W_OBJECT_IMPL, but for a W_NAMESPACE
#define W_NAMESPACE_IMPL(...) W_OBJECT_IMPL_COMMON(W_MACRO_EMPTY, __VA_ARGS__)

/// \macro W_OBJECT_IMPL_INLINE(TYPE [, TEMPLATE_STUFF])
/// Same as W_OBJECT_IMPL, but to be used in a header
/// (Requires support for c++17 inline variables)
#define W_OBJECT_IMPL_INLINE(...)                                                                                      \
    W_OBJECT_IMPL_COMMON(inline, __VA_ARGS__)                                                                          \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__)                                                                                \
    inline void W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_static_metacall(                                            \
        QObject* _o, QMetaObject::Call _c, int _id, void** _a) {                                                       \
        w_internal::FriendHelper::qt_static_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(_o, _c, _id, _a);    \
    }                                                                                                                  \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__)                                                                                \
    inline const QMetaObject* W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::metaObject() const { return &staticMetaObject; } \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__)                                                                                \
    inline void* W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_metacast(const char* _clname) {                            \
        return w_internal::FriendHelper::qt_metacast_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(this, _clname);      \
    }                                                                                                                  \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__)                                                                                \
    inline int W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_metacall(QMetaObject::Call _c, int _id, void** _a) {         \
        return w_internal::FriendHelper::qt_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(this, _c, _id, _a);  \
    }

/// \macro W_GADGET_IMPL_INLINE(TYPE [, TEMPLATE_STUFF])
/// Same as W_GADGET_IMPL, but to be used in a header
/// (Requires support for c++17 inline variables)
#define W_GADGET_IMPL_INLINE(...)                                                                                      \
    W_OBJECT_IMPL_COMMON(inline, __VA_ARGS__)                                                                          \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__)                                                                                \
    inline void W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::qt_static_metacall(                                            \
        QObject* _o, QMetaObject::Call _c, int _id, void** _a) {                                                       \
        w_internal::FriendHelper::qt_static_metacall_impl<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)>(                     \
            reinterpret_cast<W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)*>(_o), _c, _id, _a);                               \
    }

/// \macro W_NAMESPACE_IMPL_INLINE(...)
/// Same as W_NAMESPACE_IMPL, but to be used in a header
/// (Requires support for c++17 inline variables)
#define W_NAMESPACE_IMPL_INLINE(...) W_OBJECT_IMPL_COMMON(inline, __VA_ARGS__)

#else // Q_MOC_RUN
#define W_OBJECT_IMPL(...)
#define W_GADGET_IMPL(...)
#define W_NAMESPACE_IMPL(...)
#define W_OBJECT_IMPL_INLINE(...)
#define W_GADGET_IMPL_INLINE(...)
#define W_NAMESPACE_IMPL_INLINE(...)
#endif
