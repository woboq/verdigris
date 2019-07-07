#pragma once
#include "wobjectdefs.h"

namespace w_internal {
#if __cplusplus <= 201700L
template<bool... Bs>
constexpr bool all() {
    bool b = true;
    ordered2<bool>({(b = b && Bs)...});
    return b;
}
#endif

template<class F, int Flags, class IC = int, class ParamTypes = w_internal::StringViewArray<>, class ParamNames = w_internal::StringViewArray<>>
struct MetaMethodInfoBuilder {
    StringView name;
    F func;
    ParamTypes paramTypes{};
    ParamNames paramNames{};
    static constexpr int flags = Flags;
    using IntegralConstant = IC;

#if __cplusplus > 201700L
    template<class... Args, class = std::enable_if_t<(std::is_same_v<std::decay_t<Args>, StringView> && ...)>>
#else
    template<class... Args, class = std::enable_if_t<w_internal::all<std::is_same<std::decay_t<Args>, StringView>::value...>()>>
#endif
    constexpr auto setParamTypes(Args... paramTypes) const
        -> MetaMethodInfoBuilder<F, Flags, IC, w_internal::StringViewArray<sizeof... (Args)>, ParamNames> {
        return {name, func, {paramTypes...}, paramNames};
    }
#if __cplusplus > 201700L
    template<class... Args, class = std::enable_if_t<(std::is_same_v<std::decay_t<Args>, StringView> && ...)>>
#else
    template<class... Args, class = std::enable_if_t<w_internal::all<std::is_same<std::decay_t<Args>, StringView>::value...>()>>
#endif
    constexpr auto setParamNames(Args... paramNames) const
        -> MetaMethodInfoBuilder<F, Flags, IC, ParamTypes, w_internal::StringViewArray<sizeof... (Args)>> {
        return {name, func, paramTypes, {paramNames...}};
    }
    template<int... fs>
    constexpr auto addFlags(w_internal::W_MethodFlags<fs>...) const
#if __cplusplus > 201700L
        -> MetaMethodInfoBuilder<F, (fs | ... | Flags), IC, ParamTypes, ParamNames> {
#else
        -> MetaMethodInfoBuilder<F, w_internal::summed<fs ..., Flags>, IC, ParamTypes, ParamNames> {
#endif
            return {name, func, paramTypes, paramNames};
    }
    template<class IC2>
    constexpr auto setIntegralConstant() const -> MetaMethodInfoBuilder<F, Flags, IC2, ParamTypes, ParamNames> {
        return {name, func, paramTypes, paramNames};
    }

    constexpr auto build() const -> w_internal::MetaMethodInfo<F, Flags, IC, ParamTypes, ParamNames> {
        return {func, name, paramTypes, paramNames};
    }
};

} // namespace w_internal

namespace w_cpp {

/// Very simple constexpr version of std::string_view
///
/// example usage:
///     constexpr char text[6] = "Hello"; // if you have a string literal use `viewLiteral` below.
///     constexpr auto view = w_cpp::StringView{&text[0], &text[6]};
///
/// \note the end pointer has to point behind the \0
using w_internal::StringView;

/// Generate a constexpr StringView from a C++ string literal.
///
/// example usage:
///     constexpr auto view & w_cpp::StringView{"Hello"};
using w_internal::viewLiteral;

/// create a compile time property description for registraton usage
///
/// \arg T the type of the property
/// \arg name the compile time StringView for the property name
/// \arg type the compile time StringView for the type of the property
///
/// Following methods may be called in chain:
/// * .setGetter(F) - set the member function that returns the value of the property `() -> T`
/// * .setSetter(F) - set the member function that changes the value of the property `(T) -> void`
/// * .setMember(M) - set the class attribute to read and write the value of the property
/// * .setNotify(F) - set the member function that represents the changed signal for the property
/// * .setReset(F) - set the member function that resets the property value to it's default
/// * .addFlag<F>() - add one or multiple flags for the property
template<typename T>
constexpr auto makeProperty(StringView name, StringView type) {
    return w_internal::MetaPropertyInfo<T>{ name, type, {}, {}, {}, {}, {} };
}

/// create a builder for a signal description
///
/// \arg name the compile time StringView for the function name
/// \arg func the member function that implements the signal
///
/// Following methods may be call in chain:
/// * .setParamNames(StringView...) - set the names for all the function parameters (optional)
/// * .setParamTypes(StringView...) - set the names of for all parameter types
/// * .addFlag(Flags...) - add some methods flags
/// * .setIntegralConstant<T>() - set a compile time integral value type as a unique identifier
/// * .build() - build the final MethodInfo for this signal
template<typename F>
constexpr auto makeSignalBuilder(StringView name, F func) {
    return w_internal::MetaMethodInfoBuilder<F, W_MethodType::Signal.value>{name, func};
}

} // namespace w_cpp

/// \macro W_CPP_PROPERTY(callback)
/// allows to create multiple properties from a templated structure using regular C++.
///
/// example usage:
///     template<size_t I>
///     struct MyProperties;
///     template<>
///     struct MyProperties<0> {
///         constexpr static auto property = w_cpp::makeProperty<QString>(w_cpp::viewLiteral("name"), w_cpp::viewLiteral("QString"))
///             .setGetter(&Example::getName);
///     };
///     W_CPP_PROPERTY(MyProperties)
///
/// NOTE: you have to ensure that the struct is only valid for some `I`s.
#define W_CPP_PROPERTY(a) \
    static constexpr size_t W_MACRO_CONCAT(a,_O) = w_internal::stateCount<__COUNTER__, w_internal::PropertyStateTag, W_ThisType**>; \
    template<size_t I> \
    friend constexpr auto w_state(w_internal::Index<I>, w_internal::PropertyStateTag, W_ThisType**) W_RETURN((a<I-W_MACRO_CONCAT(a,_O)>::property))

/// \macro W_CPP_SIGNAL(callback)
/// allows to register multiple signals from a templated structure using regular C++.
///
/// example usage:
///     template<size_t I>
///     struct MySignals;
///     template<>
///     struct MySignals<0> {
///         constexpr static auto signal = w_cpp::makeSignalBuilder(w_cpp::viewLiteral("nameChanged"), &Example::nameChanged).build();
///     };
///     W_CPP_SIGNAL(MySignals)
///
/// NOTE: you have to ensure that the struct is only valid for some `I`s.
#define W_CPP_SIGNAL(a) \
    static constexpr size_t W_MACRO_CONCAT(a,_O) = w_internal::stateCount<__COUNTER__, w_internal::SignalStateTag, W_ThisType**>; \
    template<size_t I> \
    friend constexpr auto w_state(w_internal::Index<I>, w_internal::SignalStateTag, W_ThisType**) W_RETURN((a<I-W_MACRO_CONCAT(a,_O)>::signal))

/// \macro W_CPP_SIGNAL_IMPL(type, callback, index)
/// allows to implement a signal
///
/// example usage:
///     template<size_t I>
///     void notifyPropertyChanged() W_CPP_SIGNAL_IMPL(decltype (&tst_CppApi::notifyPropertyChanged<I>), MySignals, I, 0)
///
#define W_CPP_SIGNAL_IMPL(type, a, i, ...) { \
    constexpr int index = W_ThisType::W_MACRO_CONCAT(a,_O) + i; \
    return w_internal::SignalImplementation<type, index>{this}(__VA_ARGS__); \
}
