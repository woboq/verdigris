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

#define W_VERSION 0x010200

namespace w_internal {

using std::index_sequence;
using std::make_index_sequence;

/// faster compile time values
template<size_t> struct I;
template<size_t i> using Index = const I<i>*;
template<size_t i> inline constexpr auto index = Index<i>{};

/// constexpr friendly string_view
struct StringView {
    const char* b{};
    const char* e{};

    constexpr auto size() const -> qptrdiff { return e - b; }
    constexpr auto begin() const { return b; }
    constexpr auto end() const { return e; }
};
// adding this constructor requires more constructors which generates overhead
template<size_t N> constexpr auto viewLiteral(const char (&d)[N]) -> StringView { return {&d[0], &d[N - 1]}; }

/// raw arrays cannot be returned from functions so we wrap it
template<size_t N = 0> struct StringViewArray {
    StringView data[(N > 0 ? N : 1)]{};

    constexpr auto operator[](size_t i) const { return i < N ? data[i] : StringView{}; }
};

template<size_t SN> constexpr auto countParsedLiterals(const char (&s)[SN]) {
    auto r = size_t{};
    auto i = 0u;
    while (true) {
        while (true) {
            if (i >= SN - 1) return r;
            if (s[i] != ' ' && s[i] != ',') break; // start found
            i++;
        }
        r++;
        i++;
        while (true) {
            if (i >= SN - 1) return r;
            if (s[i] == ',') break;
            i++;
        }
        i++;
    }
}

template<size_t N, size_t SN> constexpr auto viewParsedLiterals(const char (&s)[SN]) -> StringViewArray<N> {
    // assumtions:
    // 1. s is generated from a macro
    // 2. each value is seperated by comma
    // 3. exactly N values are generated
    // 4. no second space can follow a space (no tabs or line breaks)
    auto r = StringViewArray<N>{};
    auto ri = size_t{};
    auto i = 0u;
    while (true) {
        while (true) {
            if (i >= SN - 1) return r;
            if (s[i] != ' ' && s[i] != ',') break; // start found
            i++;
        }
        r.data[ri].b = &s[i];
        i++;
        while (true) {
            if (i >= SN - 1) {
                r.data[ri].e = &s[i];
                return r; // text - end
            }
            if (s[i] == ' ') {
                if (i + 1 >= SN - 1) {
                    r.data[ri].e = &s[i];
                    return r; // text - whitespace - end
                }
                if (s[i + 1] == ',') {
                    r.data[ri].e = &s[i];
                    i += 2;
                    break; // text - whitespace - comma
                }
                i += 2;
                continue; // text - whitespace - text
            }
            if (s[i] == ',') {
                r.data[ri].e = &s[i];
                i++;
                break; // text - comma
            }
            i++;
        }
        ri++;
    }
}

template<size_t N, size_t SN> constexpr auto viewScopedLiterals(const char (&s)[SN]) -> StringViewArray<N> {
    auto r = StringViewArray<N>{};
    auto ri = size_t{};
    auto i = 0u;
    while (true) {
        while (true) {
            if (i >= SN - 1) return r;
            if (s[i] != ' ' && s[i] != ',' && s[i] != ':') break; // start found
            i++;
        }
        r.data[ri].b = &s[i];
        i++;
        while (true) {
            if (i >= SN - 1) {
                r.data[ri].e = &s[i];
                return r; // text - end
            }
            if (s[i] == ':') {
                i++;
                if (s[i] == ' ') i++;
                r.data[ri].b = &s[i];
                continue;
            }
            if (s[i] == ' ') {
                if (i + 1 >= SN - 1) {
                    r.data[ri].e = &s[i];
                    return r; // text - whitespace - end
                }
                if (s[i + 1] == ',') {
                    r.data[ri].e = &s[i];
                    i += 2;
                    break; // text - whitespace - comma
                }
                i += 2;
                continue; // text - whitespace - text
            }
            if (s[i] == ',') {
                r.data[ri].e = &s[i];
                i++;
                break; // text - comma
            }
            i++;
        }
        ri++;
    }
}

//-----------

// From qmetaobject_p.h
enum class PropertyFlags : uint {
    Invalid = 0x00000000,
    Readable = 0x00000001,
    Writable = 0x00000002,
    Resettable = 0x00000004,
    EnumOrFlag = 0x00000008,
    Alias = 0x00000010,
    // Reserved for future usage = 0x00000020,
    StdCppSet = 0x00000100,
    //     Override = 0x00000200,
    Constant = 0x00000400,
    Final = 0x00000800,
    Designable = 0x00001000,
    Scriptable = 0x00004000,
    Stored = 0x00010000,
    User = 0x00100000,
    Required = 0x01000000,
    Bindable = 0x02000000,

    // legacy
    Notify = 0x00400000,
};
static constexpr uint operator|(uint a, PropertyFlags b) { return a | static_cast<uint>(b); }
inline constexpr uint defaultPropertyFlags() {
    return 0u | PropertyFlags::Designable | PropertyFlags::Scriptable | PropertyFlags::Stored;
}

template<PropertyFlags> struct SetPropertyFlagT;
template<PropertyFlags F> using SetPropertyFlag = const SetPropertyFlagT<F>*;

template<uint> struct MethodFlagT;
template<uint I> using MethodFlag = const MethodFlagT<I>*;
template<uint I> constexpr uint methodFlagValue(MethodFlag<I>) { return I; }
using EmptyFlag = MethodFlag<0>;

// From qmetaobject_p.h
enum : uint {
    // Mirror of MethodFlags::Access*
    // From qmetaobject_p.h MethodFlags
    AccessPrivate = 0x1000, // Note: Private has a higher number to differentiate it from the default
    AccessProtected = 0x01,
    AccessPublic = 0x02,
    AccessMask = 0x03, // mask

    // Mirror of MethodFlags::MethodType*
    // From qmetaobject_p.h MethodFlags
    MethodMethod = 0x00,
    MethodSignal = 0x04,
    MethodSlot = 0x08,
    MethodConstructor = 0x0c,
    // MethodTypeMask = 0x0c,

    // Mirror of MethodFlags::Extras
    MethodCompatibility = 0x10,
    MethodCloned = 0x20,
    MethodScriptable = 0x40,
    MethodRevisioned = 0x80,
    MethodIsConst = 0x100, // no use case for volatile so far
};

using Notify = const struct NotifyTag*;
using Reset = const struct ResetTag*;

struct Bindable {};
template<typename, typename T> struct Tagged {
    T value;
    constexpr Tagged(T v)
        : value{v} {}
};

template<typename Ret, typename Host>
constexpr auto operator&(Tagged<Bindable, Bindable>, Ret (Host::*s)() const)
    -> Tagged<Bindable, Ret (Host::*)() const> {
    return {s};
}
template<typename Ret, typename Host>
constexpr auto operator&(Bindable, Ret (Host::*s)()) -> Tagged<Bindable, Ret (Host::*)()> {
    return {s};
}

#if defined(__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510
template<typename Ret, typename Host>
constexpr auto operator&(Tagged<Bindable, Bindable>, Ret (Host::*s)() const noexcept)
    -> Tagged<Bindable, Ret (Host::*)() const noexcept> {
    return {s};
}
template<typename Ret, typename Host>
constexpr auto operator&(Bindable, Ret (Host::*s)() noexcept) -> Tagged<Bindable, Ret (Host::*)() noexcept> {
    return {s};
}
#endif

} // namespace w_internal

namespace W_Access {

using Private = w_internal::MethodFlag<w_internal::AccessPrivate>;
using Protected = w_internal::MethodFlag<w_internal::AccessProtected>;
using Public = w_internal::MethodFlag<w_internal::AccessPublic>;

} // namespace W_Access

using W_Compat = w_internal::MethodFlag<w_internal::MethodCompatibility>;
using W_Scriptable = w_internal::MethodFlag<w_internal::MethodScriptable>;

namespace w_internal {

/// Holds information about a method
template<typename F, typename ParamTypes, typename ParamNames = StringViewArray<>> struct MetaMethodInfo {
    F getFunc{};
    using Func = decltype(F{}());
    uint flags{};
    StringView name{};
    ParamTypes paramTypes{};
    ParamNames paramNames{};
};

// Called from the W_SLOT macro
template<typename F, typename ParamTypes, uint... Flags>
constexpr auto makeMetaSlotInfo(F f, StringView name, const ParamTypes& paramTypes, MethodFlag<Flags>...)
    -> MetaMethodInfo<F, ParamTypes> {
    return {.getFunc = f, .flags = (Flags | ... | MethodSlot), .name = name, .paramTypes = paramTypes};
}

// Called from the W_METHOD macro
template<typename F, typename ParamTypes, uint... Flags>
constexpr auto makeMetaMethodInfo(F f, StringView name, const ParamTypes& paramTypes, MethodFlag<Flags>...)
    -> MetaMethodInfo<F, ParamTypes> {
    return {.getFunc = f, .flags = (Flags | ... | MethodMethod), .name = name, .paramTypes = paramTypes};
}

// Called from the W_SIGNAL macro
template<typename F, typename ParamTypes, typename ParamNames, uint... Flags>
constexpr auto makeMetaSignalInfo(
    F f, StringView name, const ParamTypes& paramTypes, const ParamNames& paramNames, MethodFlag<Flags>...)
    -> MetaMethodInfo<F, ParamTypes, ParamNames> {
    return {
        .getFunc = f,
        .flags = (Flags | ... | MethodSignal),
        .name = name,
        .paramTypes = paramTypes,
        .paramNames = paramNames,
    };
}

/// Holds information about a constructor
template<typename... Args> struct MetaConstructorInfo {
    static constexpr size_t argCount = sizeof...(Args);
    StringView name;
};
// called from the W_CONSTRUCTOR macro
template<typename... Args> constexpr auto makeMetaConstructorInfo(StringView name) -> MetaConstructorInfo<Args...> {
    return {name};
}

struct Empty {
    constexpr bool operator!=(std::nullptr_t) const { return false; }
};

/// Holds information about a property
template<
    typename Type,
    typename Getter = Empty,
    typename Setter = Empty,
    typename Member = Empty,
    typename Notify = Empty,
    typename Reset = Empty,
    typename Bindable = Empty>
struct MetaPropertyInfo {
    using PropertyType = Type;
    StringView name;
    StringView typeStr;
    Getter getter = {};
    StringView getterStr{};
    Setter setter = {};
    StringView setterStr{};
    Member member = {};
    StringView memberStr{};
    Notify notify = {};
    StringView notifyStr{};
    Reset reset = {};
    StringView resetStr{};
    Bindable bindable = {};
    StringView bindableStr{};
    uint flags = defaultPropertyFlags();

    template<typename S>
    constexpr auto setGetter(const S& s, const StringView& sv = {}) const
        -> MetaPropertyInfo<Type, S, Setter, Member, Notify, Reset, Bindable> {
        return {
            .name = name,
            .typeStr = typeStr,
            .getter = s,
            .getterStr = sv,
            .setter = setter,
            .setterStr = setterStr,
            .member = member,
            .memberStr = memberStr,
            .notify = notify,
            .notifyStr = notifyStr,
            .reset = reset,
            .resetStr = resetStr,
            .bindable = bindable,
            .bindableStr = bindableStr,
            .flags = flags | PropertyFlags::Readable,
        };
    }
    template<typename S>
    constexpr auto setSetter(const S& s, const StringView& sv = {}) const
        -> MetaPropertyInfo<Type, Getter, S, Member, Notify, Reset, Bindable> {
        return {
            .name = name,
            .typeStr = typeStr,
            .getter = getter,
            .getterStr = getterStr,
            .setter = s,
            .setterStr = sv,
            .member = member,
            .memberStr = memberStr,
            .notify = notify,
            .notifyStr = notifyStr,
            .reset = reset,
            .resetStr = resetStr,
            .bindable = bindable,
            .bindableStr = bindableStr,
            .flags = flags | PropertyFlags::Writable,
        };
    }
    template<typename S>
    constexpr auto setMember(const S& s, const StringView& sv = {}) const
        -> MetaPropertyInfo<Type, Getter, Setter, S, Notify, Reset, Bindable> {
        return {
            .name = name,
            .typeStr = typeStr,
            .getter = getter,
            .getterStr = getterStr,
            .setter = setter,
            .setterStr = setterStr,
            .member = s,
            .memberStr = sv,
            .notify = notify,
            .notifyStr = notifyStr,
            .reset = reset,
            .resetStr = resetStr,
            .bindable = bindable,
            .bindableStr = bindableStr,
            .flags = flags | PropertyFlags::Writable | PropertyFlags::Readable,
        };
    }
    template<typename S>
    constexpr auto setNotify(const S& s, const StringView& sv = {}) const
        -> MetaPropertyInfo<Type, Getter, Setter, Member, S, Reset, Bindable> {
        return {
            .name = name,
            .typeStr = typeStr,
            .getter = getter,
            .getterStr = getterStr,
            .setter = setter,
            .setterStr = setterStr,
            .member = member,
            .memberStr = memberStr,
            .notify = s,
            .notifyStr = sv,
            .reset = reset,
            .resetStr = resetStr,
            .bindable = bindable,
            .bindableStr = bindableStr,
            .flags = flags | PropertyFlags::Notify,
        };
    }
    template<typename S>
    constexpr auto setReset(const S& s, const StringView& sv = {}) const
        -> MetaPropertyInfo<Type, Getter, Setter, Member, Notify, S, Bindable> {
        return {
            .name = name,
            .typeStr = typeStr,
            .getter = getter,
            .getterStr = getterStr,
            .setter = setter,
            .setterStr = setterStr,
            .member = member,
            .memberStr = memberStr,
            .notify = notify,
            .notifyStr = notifyStr,
            .reset = s,
            .resetStr = sv,
            .bindable = bindable,
            .bindableStr = bindableStr,
            .flags = flags | PropertyFlags::Resettable,
        };
    }
    template<typename S>
    constexpr auto setBindable(const S& s, const StringView& sv = {}) const
        -> MetaPropertyInfo<Type, Getter, Setter, Member, Notify, Reset, S> {
        return {
            .name = name,
            .typeStr = typeStr,
            .getter = getter,
            .getterStr = getterStr,
            .setter = setter,
            .setterStr = setterStr,
            .member = member,
            .memberStr = memberStr,
            .notify = notify,
            .notifyStr = notifyStr,
            .reset = reset,
            .resetStr = resetStr,
            .bindable = s,
            .bindableStr = sv,
            .flags = flags | PropertyFlags::Bindable,
        };
    }
    constexpr auto setFlag(uint flag, bool enabled = true) const -> MetaPropertyInfo {
        auto r = *this;
        if (flag) {
            if (enabled) {
                r.flags |= flag;
            }
            else {
                r.flags &= ~flag;
            }
        }
        return r;
    }
};

/// Parse a property and fill a MetaPropertyInfo (called from W_PROPERTY macro)
// base case
template<size_t I, typename PropInfo, size_t N>
constexpr auto parseProperty(const PropInfo& p, const StringViewArray<N>&) -> PropInfo {
    return p;
}
// setter
template<size_t I, typename PropInfo, size_t N, typename Obj, typename Arg, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo& p, const StringViewArray<N>& svs, Ret (Obj::*s)(Arg), Tail... t) {
    return parseProperty<I + 1>(p.setSetter(s, svs[I]), svs, t...);
}
#if defined(__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510
template<size_t I, typename PropInfo, size_t N, typename Obj, typename Arg, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo& p, const StringViewArray<N>& svs, Ret (Obj::*s)(Arg) noexcept, Tail... t) {
    return parseProperty<I + 1>(p.setSetter(s, svs[I]), svs, t...);
}
#endif
// getter
template<size_t I, typename PropInfo, size_t N, typename Obj, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo& p, const StringViewArray<N>& svs, Ret (Obj::*s)(), Tail... t) {
    return parseProperty<I + 1>(p.setGetter(s, svs[I]), svs, t...);
}
template<size_t I, typename PropInfo, size_t N, typename Obj, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo& p, const StringViewArray<N>& svs, Ret (Obj::*s)() const, Tail... t) {
    return parseProperty<I + 1>(p.setGetter(s, svs[I]), svs, t...);
}
#if defined(__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510
template<size_t I, typename PropInfo, size_t N, typename Obj, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo& p, const StringViewArray<N>& svs, Ret (Obj::*s)() noexcept, Tail... t) {
    return parseProperty<I + 1>(p.setGetter(s, svs[I]), svs, t...);
}
template<size_t I, typename PropInfo, size_t N, typename Obj, typename Ret, typename... Tail>
constexpr auto parseProperty(
    const PropInfo& p, const StringViewArray<N>& svs, Ret (Obj::*s)() const noexcept, Tail... t) {
    return parseProperty<I + 1>(p.setGetter(s, svs[I]), svs, t...);
}
#endif
// member
template<size_t I, typename PropInfo, size_t N, typename Obj, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo& p, const StringViewArray<N>& svs, Ret Obj::*s, Tail... t) {
    return parseProperty<I + 1>(p.setMember(s, svs[I]), svs, t...);
}
// notify
template<size_t I, typename PropInfo, size_t N, typename F, typename... Tail>
constexpr auto parseProperty(const PropInfo& p, const StringViewArray<N>& svs, Notify, F f, Tail... t) {
    return parseProperty<I + 2>(p.setNotify(f, svs[I + 1]), svs, t...);
}
// reset
template<size_t I, typename PropInfo, size_t N, typename Obj, typename Ret, typename... Tail>
constexpr auto parseProperty(const PropInfo& p, const StringViewArray<N>& svs, Reset, Ret (Obj::*s)(), Tail... t) {
    return parseProperty<I + 2>(p.setReset(s, svs[I + 1]), svs, t...);
}
#if defined(__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510
template<size_t I, typename PropInfo, size_t N, typename Obj, typename Ret, typename... Tail>
constexpr auto parseProperty(
    const PropInfo& p, const StringViewArray<N>& svs, Reset, Ret (Obj::*s)() noexcept, Tail... t) {
    return parseProperty<I + 2>(p.setReset(s, svs[I + 1]), svs, t...);
}
#endif
// bindable
template<size_t I, typename PropInfo, size_t N, typename S, typename... Tail>
constexpr auto parseProperty(const PropInfo& p, const StringViewArray<N>& svs, Tagged<Bindable, S> s, Tail... t) {
    return parseProperty<I + 1>(p.setBindable(s.value, svs[I]), svs, t...);
}
// property flag
template<size_t I, typename PropInfo, size_t N, typename... Tail>
constexpr auto parseProperty(const PropInfo& p, const StringViewArray<N>& svs, PropertyFlags flag, Tail... t) {
    return parseProperty<I + 1>(p.setFlag(static_cast<uint>(flag)), svs, t...);
}
template<size_t I, typename PropInfo, size_t N, PropertyFlags Flag, typename... Tail>
constexpr auto parseProperty(
    const PropInfo& p, const StringViewArray<N>& svs, SetPropertyFlag<Flag>, bool value, Tail... t) {
    return parseProperty<I + 2>(p.setFlag(static_cast<uint>(Flag), value), svs, t...);
}

template<typename T, typename... Args, size_t N>
constexpr auto makeMetaPropertyInfo(StringView name, StringView type, StringViewArray<N> defineStrs, Args... args) {
    auto propInfo = MetaPropertyInfo<T>{.name = name, .typeStr = type};
    return parseProperty<0>(propInfo, defineStrs, args...);
}

template<typename T, typename = void> struct EnumIsScoped {
    enum { Value = std::is_convertible<T, int>::value ? 0 : 2 };
};
template<typename T> struct EnumIsScoped<QFlags<T>, void> : EnumIsScoped<typename QFlags<T>::enum_type> {};

template<typename T> using EnumPtr = T*;
template<typename T> auto extractFlagEnum(T*) -> T;
template<typename T> auto extractFlagEnum(QFlags<T>*) -> T;
template<typename T> using ExtractFlagEnum = decltype(extractFlagEnum(EnumPtr<T>{}));

/// Holds information about an enum
template<bool HasAlias, size_t Count_, typename Values_, typename Names, int Flags> struct MetaEnumInfo {
    StringView name;
    StringView alias;
    Names names;
    using Values = Values_;
    static constexpr uint flags = Flags;
    static constexpr auto count = Count_;

    static constexpr auto hasAlias = HasAlias;
};
template<typename Enum, Enum... Value> struct enum_sequence {};
// called from W_ENUM and W_FLAG
template<typename Enum, int Flag, Enum... Values, typename Names>
constexpr auto makeMetaEnumInfo(StringView name, int, enum_sequence<Enum, Values...>, Names names)
    -> MetaEnumInfo<false, sizeof...(Values), enum_sequence<Enum, Values...>, Names, Flag | EnumIsScoped<Enum>::Value> {
    return {name, viewLiteral(""), names};
}
template<typename Enum, int Flag, Enum... Values, typename Names>
constexpr auto makeMetaEnumInfo(StringView name, StringView alias, enum_sequence<Enum, Values...>, Names names)
    -> MetaEnumInfo<true, sizeof...(Values), enum_sequence<Enum, Values...>, Names, Flag | EnumIsScoped<Enum>::Value> {
    return {name, alias, names};
}

// Helper function to remove the scope in a scoped enum name. (so "Foo::Bar" -> Bar)
template<int N> constexpr int removedScopeSize(const char (&name)[N]) {
    // Find the position of the last colon (or 0 if it is not there)
    int p = N - 1;
    while (p > 0 && name[p] != ':') p--;
    if (name[p] == ':') p++;
    return N - p;
}

/// Helper for the implementation of a signal.
/// Called from the signal implementation within the W_SIGNAL macro.
///
/// 'Func' is the type of the signal. 'Idx' is the signal index (relative).
/// It is implemented as a functor so the operator() has exactly the same amount of argument of the
/// signal so the __VA_ARGS__ works also if there is no arguments (no leading commas)
///
/// There is specialization for const and non-const,  and for void and non-void signals.
///
/// the last argument of the operator() is an int, to workaround the ",0" required in the W_SIGNAL
/// macro to make sure there is at least one argument for the ...
template<typename Func, int Idx> struct SignalImplementation {};
template<typename Obj, typename Ret, typename... Args, int Idx>
struct SignalImplementation<Ret (Obj::*)(Args...), Idx> {
    Obj* this_;
    Ret operator()(Args... args, int) const {
        Ret r{};
        const void* a[] = {std::addressof(r), std::addressof(args)...};
        QMetaObject::activate(this_, &Obj::staticMetaObject, Idx, const_cast<void**>(a));
        return r;
    }
};
template<typename Obj, typename... Args, int Idx> struct SignalImplementation<void (Obj::*)(Args...), Idx> {
    Obj* this_;
    void operator()(Args... args, int) {
        const void* a[] = {nullptr, std::addressof(args)...};
        QMetaObject::activate(this_, &Obj::staticMetaObject, Idx, const_cast<void**>(a));
    }
};
template<typename Obj, typename Ret, typename... Args, int Idx>
struct SignalImplementation<Ret (Obj::*)(Args...) const, Idx> {
    const Obj* this_;
    Ret operator()(Args... args, int) const {
        Ret r{};
        const void* a[] = {std::addressof(r), std::addressof(args)...};
        QMetaObject::activate(const_cast<Obj*>(this_), &Obj::staticMetaObject, Idx, const_cast<void**>(a));
        return r;
    }
};
template<typename Obj, typename... Args, int Idx> struct SignalImplementation<void (Obj::*)(Args...) const, Idx> {
    const Obj* this_;
    void operator()(Args... args, int) {
        const void* a[] = {nullptr, std::addressof(args)...};
        QMetaObject::activate(const_cast<Obj*>(this_), &Obj::staticMetaObject, Idx, const_cast<void**>(a));
    }
};

/// Used in the W_OBJECT macro to compute the base type.
/// Used like this:
///  using W_BaseType = std::remove_reference_t<decltype(getParentObjectHelper(&W_ThisType::qt_metacast))>;
/// Since qt_metacast for W_ThisType will be declared later, the pointer to member function will be
/// pointing to the qt_metacast of the base class, so T will be deduced to the base class type.
///
/// Returns a reference so this work if T is an abstract class.
template<typename T> T& getParentObjectHelper(void* (T::*)(const char*));

// helper class that can access the private member of any class with W_OBJECT
struct FriendHelper;

template<typename T> struct Interface {
    using Pointer = T*;
    StringView name;
};

} // namespace w_internal

#if defined(Q_CC_MSVC) && !defined(Q_CC_CLANG)
#if _MSC_VER >= 1928
#if defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL
  // MSVC in /Zc:preprocessor- mode (default)
#define W_CONFORMING_PREPROCESSOR 0
#else
  // MSVC in /Zc:preprocessor mode
#define W_CONFORMING_PREPROCESSOR 1
#endif
#else
  // Older versions of MSVC without the /Zc:preprocessor flag
#define W_CONFORMING_PREPROCESSOR 0
#endif
#else
  // Other compilers, generally conforming
#define W_CONFORMING_PREPROCESSOR 1
#endif

#if !W_CONFORMING_PREPROCESSOR
// Workaround for MSVC: expension rules are different so we need some extra macro.
#define W_MACRO_MSVC_EXPAND(...) __VA_ARGS__
#define W_MACRO_MSVC_DELAY(X, ...) W_MACRO_MSVC_EXPAND(X(__VA_ARGS__))
#define W_MACRO_MSVC_EMPTY / ##**## /
#else
#define W_MACRO_MSVC_EXPAND(...) __VA_ARGS__
#define W_MACRO_MSVC_DELAY(X, ...) X(__VA_ARGS__)
#define W_MACRO_MSVC_EMPTY
#endif

// Private macro helpers for  macro programming
#define W_MACRO_EMPTY
#define W_MACRO_EVAL(...) __VA_ARGS__
#define W_MACRO_DELAY(X, ...) W_MACRO_MSVC_EXPAND(X(__VA_ARGS__))
#define W_MACRO_DELAY2(X, ...) W_MACRO_MSVC_EXPAND(X(__VA_ARGS__))
#define W_MACRO_TAIL(A, ...) __VA_ARGS__
#define W_MACRO_FIRST(...) W_MACRO_MSVC_EXPAND(W_MACRO_FIRST2(__VA_ARGS__, ))
#define W_MACRO_FIRST2(A, ...) A
#define W_MACRO_STRIGNIFY(...) W_MACRO_STRIGNIFY2(__VA_ARGS__)
#define W_MACRO_STRIGNIFY2(...) #__VA_ARGS__
#define W_MACRO_CONCAT(A, B) W_MACRO_CONCAT2(A, B)
#define W_MACRO_CONCAT2(A, B) A##_##B

// strignify and make a StaticStringList out of an array of arguments
#define W_PARAM_TOSTRING(...) W_MACRO_MSVC_EMPTY W_MACRO_MSVC_DELAY(W_PARAM_TOSTRING2, __VA_ARGS__)
#define W_PARAM_TOSTRING2(...)                                                                                         \
    w_internal::viewParsedLiterals<w_internal::countParsedLiterals("" #__VA_ARGS__)>("" #__VA_ARGS__)

#define W_PARAM_TOSTRING_REMOVE_SCOPE(...)                                                                             \
    W_MACRO_MSVC_EMPTY W_MACRO_MSVC_DELAY(W_PARAM_TOSTRING2_REMOVE_SCOPE, __VA_ARGS__)
#define W_PARAM_TOSTRING2_REMOVE_SCOPE(...)                                                                            \
    w_internal::viewScopedLiterals<w_internal::countParsedLiterals("" #__VA_ARGS__)>("" #__VA_ARGS__)

// remove the surrounding parentheses
#define W_MACRO_REMOVEPAREN(A) W_MACRO_DELAY(W_MACRO_REMOVEPAREN2, W_MACRO_REMOVEPAREN_HELPER A)
#define W_MACRO_REMOVEPAREN2(...) W_MACRO_DELAY2(W_MACRO_TAIL, W_MACRO_REMOVEPAREN_HELPER_##__VA_ARGS__)
#define W_MACRO_REMOVEPAREN_HELPER(...) _, __VA_ARGS__
#define W_MACRO_REMOVEPAREN_HELPER_W_MACRO_REMOVEPAREN_HELPER ,

// if __VA_ARGS__ is "(types), foobar"   then return just the types, otherwise return nothing
#define W_OVERLOAD_TYPES(...) W_MACRO_DELAY(W_OVERLOAD_TYPES2, W_OVERLOAD_TYPES_HELPER __VA_ARGS__, )
#define W_OVERLOAD_TYPES2(A, ...) W_MACRO_DELAY2(W_OVERLOAD_TYPES3, W_OVERLOAD_TYPES_HELPER_##A...)
#define W_OVERLOAD_TYPES3(A, ...) W_MACRO_EVAL A
#define W_OVERLOAD_TYPES_HELPER(...) YES(__VA_ARGS__)
#define W_OVERLOAD_TYPES_HELPER_W_OVERLOAD_TYPES_HELPER (),
#define W_OVERLOAD_TYPES_HELPER_YES(...) (__VA_ARGS__),

#define W_OVERLOAD_RESOLVE(...) W_MACRO_DELAY(W_OVERLOAD_RESOLVE2, W_OVERLOAD_RESOLVE_HELPER __VA_ARGS__, )
#define W_OVERLOAD_RESOLVE2(A, ...) W_MACRO_DELAY2(W_MACRO_FIRST, W_OVERLOAD_RESOLVE_HELPER_##A)
#define W_OVERLOAD_RESOLVE_HELPER(...) YES(qOverload<__VA_ARGS__>)
#define W_OVERLOAD_RESOLVE_HELPER_YES(...) (__VA_ARGS__)
#define W_OVERLOAD_RESOLVE_HELPER_W_OVERLOAD_RESOLVE_HELPER ,

// remove the first argument if it is in parentheses"
#define W_OVERLOAD_REMOVE(...) W_MACRO_DELAY(W_OVERLOAD_REMOVE2, W_OVERLOAD_REMOVE_HELPER __VA_ARGS__)
#define W_OVERLOAD_REMOVE2(...) W_MACRO_DELAY2(W_MACRO_TAIL, W_OVERLOAD_REMOVE_HELPER_##__VA_ARGS__)

#define W_OVERLOAD_REMOVE_HELPER(...) _
#define W_OVERLOAD_REMOVE_HELPER_W_OVERLOAD_REMOVE_HELPER ,

namespace w_internal {

/// We store state in overloads for this method.
/// This overload is found if no better overload was found.
/// All overloads are found using ADL in the QObject T
template<size_t L, typename State, typename TPP, size_t N>
concept HasState = requires {
                       L >= 0;
                       w_state(index<N>, State{}, TPP{});
                   };

template<size_t L, typename State, typename TPP, size_t N, size_t M, size_t X = (N + M) / 2>
consteval size_t stateCountBetween() {
    if constexpr (N == X) {
        if constexpr (HasState<L, State, TPP, X>) {
            return M;
        }
        else {
            return N;
        }
    }
    else if constexpr (HasState<L, State, TPP, X>) {
        return stateCountBetween<L, State, TPP, X, M>();
    }
    else {
        return stateCountBetween<L, State, TPP, N, X>();
    }
}
template<size_t L, typename State, typename TPP, size_t N = 1> consteval size_t stateCount() {
    if constexpr (HasState<L, State, TPP, N>) {
        return stateCount<L, State, TPP, N * 2>();
    }
    else {
        return stateCountBetween<L, State, TPP, N / 2, N>();
    }
}

using SlotStateTag = const struct SlotState*;
using SignalStateTag = const struct SignalState*;
using MethodStateTag = const struct MethodState*;
using ConstructorStateTag = const struct ConstructorState*;
using PropertyStateTag = const struct PropertyState*;
using EnumStateTag = const struct EnumState*;
using ClassInfoStateTag = const struct ClassInfoState*;
using InterfaceStateTag = const struct InterfaceState*;

} // namespace w_internal

#define W_OBJECT_COMMON(TYPE)                                                                                          \
    using W_ThisType = TYPE;                                                                                           \
    static constexpr auto W_UnscopedName =                                                                             \
        w_internal::viewLiteral(#TYPE); /* so we don't repeat it in W_CONSTRUCTOR */                                   \
    friend struct w_internal::FriendHelper;                                                                            \
    static inline constexpr int w_flagAlias(void*) { return 0; }                                                       \
                                                                                                                       \
public:                                                                                                                \
    struct W_MetaObjectCreatorHelper

#define W_STATE_COUNT(STATE) w_internal::stateCount<__COUNTER__, w_internal::STATE##Tag, W_ThisType**>()

#define W_STATE_STORE(STATE, COUNT, ...)                                                                               \
    friend constexpr auto w_state(w_internal::Index<COUNT>, w_internal::STATE##Tag, W_ThisType**)                      \
        ->decltype((__VA_ARGS__)) {                                                                                    \
        return (__VA_ARGS__);                                                                                          \
    }                                                                                                                  \
    friend struct w_internal::FriendHelper

#define W_STATE_STORE_NS(STATE, COUNT, ...)                                                                            \
    static constexpr auto w_state(w_internal::Index<COUNT>, w_internal::STATE##Tag, W_ThisType**)                      \
        ->decltype((__VA_ARGS__)) {                                                                                    \
        return (__VA_ARGS__);                                                                                          \
    }

#define W_STATE_APPEND(STATE, ...) W_STATE_STORE(STATE, W_STATE_COUNT(STATE), __VA_ARGS__)
#define W_STATE_APPEND_NS(STATE, ...) W_STATE_STORE_NS(STATE, W_STATE_COUNT(STATE), __VA_ARGS__)

#define W_ACCESS_HELPER(STATE, COUNT)                                                                                  \
    static void w_accessHelper(w_internal::Index<COUNT>, w_internal::STATE##Tag, W_ThisType**)

// public macros

/// \macro W_OBJECT(TYPE)
/// Like the Q_OBJECT macro, this declare that the object might have signals, slots or properties.
/// Must contains the class name as a parameter and need to be put before any other W_ macro in the class.
#define W_OBJECT(TYPE)                                                                                                 \
    W_OBJECT_COMMON(TYPE);                                                                                             \
                                                                                                                       \
public:                                                                                                                \
    using W_BaseType = std::remove_reference_t<decltype(w_internal::getParentObjectHelper(&W_ThisType::qt_metacast))>; \
    Q_OBJECT                                                                                                           \
    QT_ANNOTATE_CLASS(qt_fake, "")

/// \macro W_GADGET(TYPE)
/// Like the Q_GADGET macro, this declare that the object might have properties.
/// Must contains the class name as a parameter and need to be put before any other W_ macro in the class.
#define W_GADGET(TYPE)                                                                                                 \
    W_OBJECT_COMMON(TYPE);                                                                                             \
    Q_GADGET                                                                                                           \
    QT_ANNOTATE_CLASS(qt_fake, "")

/// \macro W_NAMESPACE(NAMESPACE)
/// Like the Q_GADGET macro, this declare that the object might have properties.
/// Must contains the class name as a parameter and need to be put before any other W_ macro in the class.
#define W_NAMESPACE(NAMESPACE)                                                                                         \
    Q_NAMESPACE                                                                                                        \
    struct W_MetaObjectCreatorHelper;                                                                                  \
    struct W_ThisType {                                                                                                \
        using W_MetaObjectCreatorHelper = NAMESPACE::W_MetaObjectCreatorHelper;                                        \
        static constexpr auto qt_static_metacall = nullptr;                                                            \
    };                                                                                                                 \
    static constexpr auto W_UnscopedName = w_internal::viewLiteral(#NAMESPACE);                                        \
    Q_DECL_UNUSED static inline constexpr int w_flagAlias(void*) {                                                     \
        Q_UNUSED(W_UnscopedName)                                                                                       \
        return 0;                                                                                                      \
    }                                                                                                                  \
    QT_ANNOTATE_CLASS(qt_fake, "")

/// \macro W_SLOT( <slot name> [, (<parameters types>) ]  [, <flags>]* )
///
/// The W_SLOT macro needs to be put after the slot declaration.
///
/// The W_SLOT macro can optionally have a list of parameter types as second argument to disambiguate
/// overloads or use types that are not registered with W_REGISTER_ARGTYPE. The list of parameter
/// need to be within parentheses (even if there is 0 or 1 argument).
///
/// The W_SLOT macro can have flags:
/// - Specifying the the access: W_Access::Protected{}, W_Access::Private{}
///   or W_Access::Public{}. (By default, it is auto-detected from the location of this macro.)
/// - W_Compat: for deprecated methods (equivalent of Q_MOC_COMPAT)
#define W_SLOT(...) W_MACRO_MSVC_EXPAND(W_SLOT2(__LINE__, __VA_ARGS__, w_internal::EmptyFlag{}))
#define W_SLOT_N(...) W_MACRO_MSVC_EXPAND(W_SLOT2(__VA_ARGS__, w_internal::EmptyFlag{}))
#define W_SLOT2(N, NAME, ...)                                                                                          \
    static constexpr int W_MACRO_CONCAT(w_slotIndex_##NAME, N) = W_STATE_COUNT(SlotState);                             \
    static constexpr auto W_MACRO_CONCAT(w_slotFunc_##NAME, N)()                                                       \
        ->std::remove_reference_t<decltype(W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME))> {                      \
        return W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME);                                                     \
    }                                                                                                                  \
    W_STATE_STORE(                                                                                                     \
        SlotState,                                                                                                     \
        W_MACRO_CONCAT(w_slotIndex_##NAME, N),                                                                         \
        w_internal::makeMetaSlotInfo(                                                                                  \
            &W_ThisType::W_MACRO_CONCAT(w_slotFunc_##NAME, N),                                                         \
            w_internal::viewLiteral(#NAME),                                                                            \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)),                                                           \
            W_OVERLOAD_REMOVE(__VA_ARGS__)));                                                                          \
    W_ACCESS_HELPER(SlotState, W_MACRO_CONCAT(w_slotIndex_##NAME, N))

/// \macro W_INVOKABLE( <slot name> [, (<parameters types>) ]  [, <flags>]* )
/// Exactly like W_SLOT but for Q_INVOKABLE methods.
#define W_INVOKABLE(...) W_MACRO_MSVC_EXPAND(W_INVOKABLE2(__LINE__, __VA_ARGS__, w_internal::EmptyFlag{}))
#define W_INVOKABLE_N(...) W_MACRO_MSVC_EXPAND(W_INVOKABLE2(__VA_ARGS__, w_internal::EmptyFlag{}))
#define W_INVOKABLE2(N, NAME, ...)                                                                                     \
    static constexpr int W_MACRO_CONCAT(w_invokableIndex_##NAME, N) = W_STATE_COUNT(MethodState);                      \
    static constexpr auto W_MACRO_CONCAT(w_invokableFunc_##NAME, N)()                                                  \
        ->std::remove_reference_t<decltype(W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME))> {                      \
        return W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME);                                                     \
    }                                                                                                                  \
    W_STATE_STORE(                                                                                                     \
        MethodState,                                                                                                   \
        W_MACRO_CONCAT(w_invokableIndex_##NAME, N),                                                                    \
        w_internal::makeMetaMethodInfo(                                                                                \
            &W_ThisType::W_MACRO_CONCAT(w_invokableFunc_##NAME, N),                                                    \
            w_internal::viewLiteral(#NAME),                                                                            \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)),                                                           \
            W_OVERLOAD_REMOVE(__VA_ARGS__)));                                                                          \
    W_ACCESS_HELPER(MethodState, W_MACRO_CONCAT(w_invokableIndex_##NAME, N))

/// <signal signature>
/// \macro W_SIGNAL(<signal name> [, (<parameter types>) ] , <parameter names> )
///
/// Unlike W_SLOT, W_SIGNAL must be placed directly after the signal signature declaration.
/// There should not be a semi colon between the signal signature and the macro
///
/// Like W_SLOT, there can be the types of the parametter as a second argument, within parentheses.
/// You must then follow with the parameter names
///
/// Note about exported classes: since the signal is inline, GCC won't export it when compiling
/// with -fvisibility-inlines-hidden (which is the default), so connecting using pointer to member
/// functions won't work accross library boundaries. You need to explicitly export the signal with
/// your MYLIB_EXPORT macro in front of the signal declaration.
#define W_SIGNAL(...) W_MACRO_MSVC_EXPAND(W_SIGNAL2(__LINE__, __VA_ARGS__, 0))
#define W_SIGNAL_N(...) W_MACRO_MSVC_EXPAND(W_SIGNAL2(__VA_ARGS__, 0))
#define W_SIGNAL2(N, NAME, ...)                                                                                        \
    { /* W_SIGNAL need to be placed directly after the signal declaration, without semicolon. */                       \
        using w_SignalType = decltype(W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME));                             \
        return w_internal::SignalImplementation<w_SignalType, W_MACRO_CONCAT(w_signalIndex_##NAME, N)>{this}(          \
            W_OVERLOAD_REMOVE(__VA_ARGS__));                                                                           \
    }                                                                                                                  \
    static constexpr int W_MACRO_CONCAT(w_signalIndex_##NAME, N) = W_STATE_COUNT(SignalState);                         \
    static constexpr auto W_MACRO_CONCAT(w_signalFunc_##NAME, N)()                                                     \
        ->std::remove_reference_t<decltype(W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME))> {                      \
        return W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME);                                                     \
    }                                                                                                                  \
    W_STATE_STORE(                                                                                                     \
        SignalState,                                                                                                   \
        W_MACRO_CONCAT(w_signalIndex_##NAME, N),                                                                       \
        w_internal::makeMetaSignalInfo(                                                                                \
            &W_ThisType::W_MACRO_CONCAT(w_signalFunc_##NAME, N),                                                       \
            w_internal::viewLiteral(#NAME),                                                                            \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)),                                                           \
            W_PARAM_TOSTRING(W_OVERLOAD_REMOVE(__VA_ARGS__))));                                                        \
    W_ACCESS_HELPER(SignalState, W_MACRO_CONCAT(w_signalIndex_##NAME, N))

/// \macro W_SIGNAL_COMPAT
/// Same as W_SIGNAL, but set the W_Compat flag
#define W_SIGNAL_COMPAT(...) W_MACRO_MSVC_EXPAND(W_SIGNAL_COMPAT2(__LINE__, __VA_ARGS__, 0))
#define W_SIGNAL_COMPAT_N(...) W_MACRO_MSVC_EXPAND(W_SIGNAL_COMPAT2(__VA_ARGS__, 0))
#define W_SIGNAL_COMPAT2(N, NAME, ...)                                                                                 \
    {                                                                                                                  \
        using w_SignalType = decltype(W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME));                             \
        return w_internal::SignalImplementation<w_SignalType, W_MACRO_CONCAT(w_signalIndex_##NAME, N)>{this}(          \
            W_OVERLOAD_REMOVE(__VA_ARGS__));                                                                           \
    }                                                                                                                  \
    static constexpr int W_MACRO_CONCAT(w_signalIndex_##NAME, N) = W_STATE_COUNT(SignalState);                         \
    static constexpr auto W_MACRO_CONCAT(w_signalFunc_##NAME, N)()                                                     \
        ->std::remove_reference_t<decltype(W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME))> {                      \
        return W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME);                                                     \
    }                                                                                                                  \
    W_STATE_STORE(                                                                                                     \
        SignalState,                                                                                                   \
        W_MACRO_CONCAT(w_signalIndex_##NAME, N),                                                                       \
        w_internal::makeMetaSignalInfo(                                                                                \
            &W_ThisType::W_MACRO_CONCAT(w_signalFunc_##NAME, N),                                                       \
            w_internal::viewLiteral(#NAME),                                                                            \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)),                                                           \
            W_PARAM_TOSTRING(W_OVERLOAD_REMOVE(__VA_ARGS__)),                                                          \
            W_Compat{}));                                                                                              \
    W_ACCESS_HELPER(SignalState, W_MACRO_CONCAT(w_signalIndex_##NAME, N))

/// \macro W_CONSTRUCTOR(<parameter types>)
/// Declares that this class can be constructed with this list of argument.
/// Equivalent to Q_INVOKABLE constructor.
/// One can have W_CONSTRUCTOR() for the default constructor even if it is implicit.
#define W_CONSTRUCTOR(...)                                                                                             \
    W_STATE_APPEND(ConstructorState, w_internal::makeMetaConstructorInfo<__VA_ARGS__>(W_UnscopedName))

/// \macro W_PROPERTY(<type>, <name> [, <flags>]*)
///
/// Declare a property <name> with the type <type>.
/// The flags can be function pointers that are detected to be setter, getters, notify signal or
/// other flags. Use the macro READ, WRITE, MEMBER, ... for the flag so you can write W_PROPERTY
/// just like in a Q_PROPERTY. The only difference with Q_PROPERTY would be the semicolon before the
/// name.
/// W_PROPERTY need to be put after all the setters, getters, signals and members have been declared.
///
/// <type> can optionally be put in parentheses, if you have a type containing a comma
///
/// The macro to be used inside W_PROPERTY can also be prefixed by W_ (W_READ, W_WRITE, ...)
/// If you define the macro W_NO_PROPERTY_MACRO in your build settings, the macro without prefix
/// won't be defined.
#define W_PROPERTY(...) W_MACRO_MSVC_EXPAND(W_PROPERTY2(__VA_ARGS__)) // expands the READ, WRITE, and other sub marcos
#define W_PROPERTY2(TYPE, NAME, ...)                                                                                   \
    W_STATE_APPEND(                                                                                                    \
        PropertyState,                                                                                                 \
        w_internal::makeMetaPropertyInfo<W_MACRO_REMOVEPAREN(TYPE)>(                                                   \
            w_internal::viewLiteral(#NAME),                                                                            \
            w_internal::viewLiteral(W_MACRO_STRIGNIFY(W_MACRO_REMOVEPAREN(TYPE))),                                     \
            W_PARAM_TOSTRING_REMOVE_SCOPE(__VA_ARGS__),                                                                \
            __VA_ARGS__))

#define W_WRITE , &W_ThisType::
#define W_READ , &W_ThisType::
#define W_NOTIFY , w_internal::Notify{}, &W_ThisType::
#define W_RESET , w_internal::Reset{}, &W_ThisType::
#define W_MEMBER , &W_ThisType::
#define W_BINDABLE , w_internal::Bindable{} & &W_ThisType::
#define W_DESIGNABLE , w_internal::SetPropertyFlag<w_internal::PropertyFlags::Designable>{},
#define W_SCRIPTABLE , w_internal::SetPropertyFlag<w_internal::PropertyFlags::Scriptable>{},
#define W_STORED , w_internal::SetPropertyFlag<w_internal::PropertyFlags::Stored>{},
#define W_USER , w_internal::SetPropertyFlag<w_internal::PropertyFlags::User>{},
#define W_CONSTANT , w_internal::PropertyFlags::Constant
#define W_FINAL , w_internal::PropertyFlags::Final
#define W_REQUIRED , w_internal::PropertyFlags::Required

#ifndef W_NO_PROPERTY_MACRO
#define WRITE W_WRITE
#define READ W_READ
#define NOTIFY W_NOTIFY
#define RESET W_RESET
#define MEMBER W_MEMBER
#define BINDABLE W_BINDABLE
#define STORED W_STORED
#define CONSTANT W_CONSTANT
#define FINAL W_FINAL
#define REQUIRED W_REQUIRED
#endif

#undef Q_PRIVATE_PROPERTY // the official macro is not a variadic macro, and the coma in READ would break it
#define Q_PRIVATE_PROPERTY(...)

/// \macro W_ENUM(<name>, <values>)
/// Similar to Q_ENUM, but one must also manually write all the values.
#define W_ENUM(NAME, ...)                                                                                              \
    Q_ENUM(NAME)                                                                                                       \
    W_STATE_APPEND(                                                                                                    \
        EnumState,                                                                                                     \
        w_internal::makeMetaEnumInfo<NAME, false>(                                                                     \
            w_internal::viewLiteral(#NAME),                                                                            \
            w_flagAlias(w_internal::EnumPtr<NAME>{}),                                                                  \
            w_internal::enum_sequence<NAME, __VA_ARGS__>{},                                                            \
            W_PARAM_TOSTRING_REMOVE_SCOPE(__VA_ARGS__)))

/// \macro W_ENUM_NS(<name>, <values>)
/// Similar to Q_ENUM_NS, like W_ENUM
#define W_ENUM_NS(NAME, ...)                                                                                           \
    Q_ENUM_NS(NAME)                                                                                                    \
    W_STATE_APPEND_NS(                                                                                                 \
        EnumState,                                                                                                     \
        w_internal::makeMetaEnumInfo<NAME, false>(                                                                     \
            w_internal::viewLiteral(#NAME),                                                                            \
            w_flagAlias(w_internal::EnumPtr<NAME>{}),                                                                  \
            w_internal::enum_sequence<NAME, __VA_ARGS__>{},                                                            \
            W_PARAM_TOSTRING_REMOVE_SCOPE(__VA_ARGS__)))

/// \macro W_FLAG(<name>, <values>)
/// Similar to Q_FLAG, but one must also manually write all the values.
#define W_FLAG(NAME, ...)                                                                                              \
    Q_FLAG(NAME)                                                                                                       \
    W_STATE_APPEND(                                                                                                    \
        EnumState,                                                                                                     \
        w_internal::makeMetaEnumInfo<w_internal::ExtractFlagEnum<NAME>, true>(                                         \
            w_internal::viewLiteral(#NAME),                                                                            \
            w_flagAlias(w_internal::EnumPtr<NAME>{}),                                                                  \
            w_internal::enum_sequence<w_internal::ExtractFlagEnum<NAME>, __VA_ARGS__>{},                               \
            W_PARAM_TOSTRING_REMOVE_SCOPE(__VA_ARGS__)))

/// \macro W_FLAG_NS(<name>, <values>)
/// Similar to Q_FLAG_NS, like W_FLAG.
#define W_FLAG_NS(NAME, ...)                                                                                           \
    Q_FLAG_NS(NAME)                                                                                                    \
    W_STATE_APPEND_NS(                                                                                                 \
        EnumState,                                                                                                     \
        w_internal::makeMetaEnumInfo<w_internal::ExtractFlagEnum<NAME>, true>(                                         \
            w_internal::viewLiteral(#NAME),                                                                            \
            w_flagAlias(w_internal::EnumPtr<NAME>{}),                                                                  \
            w_internal::enum_sequence<w_internal::ExtractFlagEnum<NAME>, __VA_ARGS__>{},                               \
            W_PARAM_TOSTRING_REMOVE_SCOPE(__VA_ARGS__)))

/// Same as Q_CLASSINFO.  Note, Q_CLASSINFO_NS is required for namespace
#define W_CLASSINFO(A, B)                                                                                              \
    W_STATE_APPEND(                                                                                                    \
        ClassInfoState,                                                                                                \
        std::pair<w_internal::StringView, w_internal::StringView>{                                                     \
            w_internal::viewLiteral(A), w_internal::viewLiteral(B)})

/// Same as Q_CLASSINFO, but within a namespace
#define W_CLASSINFO_NS(A, B)                                                                                           \
    W_STATE_APPEND_NS(                                                                                                 \
        ClassInfoState,                                                                                                \
        std::pair<w_internal::StringView, w_internal::StringView>{                                                     \
            w_internal::viewLiteral(A), w_internal::viewLiteral(B)})

/// Same as Q_INTERFACE
#define W_INTERFACE(A) W_STATE_APPEND(InterfaceState, w_internal::Interface<A>{w_internal::viewLiteral(#A)})

/// Same as Q_DECLARE_FLAGS
#define W_DECLARE_FLAGS(Flags, Enum)                                                                                   \
    using Flags = QFlags<Enum>;                                                                                        \
    static inline constexpr w_internal::StringView w_flagAlias(Flags*) { return w_internal::viewLiteral(#Enum); }      \
    friend struct w_internal::FriendHelper

/// \macro W_REGISTER_ARGTYPE(TYPE)
/// Registers TYPE so it can be used as a parameter of a signal/slot or return value.
/// The normalized signature must be used.
/// Note: This does not imply Q_DECLARE_METATYPE, and Q_DECLARE_METATYPE does not imply this
namespace w_internal {
template<typename T> struct W_TypeRegistery {
    enum { registered = false };
};
} // namespace w_internal
#define W_REGISTER_ARGTYPE(...)                                                                                        \
    namespace w_internal {                                                                                             \
    template<> struct W_TypeRegistery<__VA_ARGS__> {                                                                   \
        enum { registered = true };                                                                                    \
        static constexpr auto name = viewLiteral(#__VA_ARGS__);                                                        \
    };                                                                                                                 \
    }
W_REGISTER_ARGTYPE(char*)
W_REGISTER_ARGTYPE(const char*)

#else // Q_MOC_RUN
// just to avoid parse errors when moc is run over things that it should ignore
#define W_SIGNAL(...) ;
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
