#pragma once
#include "wobjectdefs.h"

namespace w_cpp {

using w_internal::StringView;
using w_internal::viewLiteral;

template<typename T>
constexpr auto makeProperty(StringView name, StringView type) {
    return w_internal::MetaPropertyInfo<T>{ name, type, {}, {}, {}, {}, {} };
}

template<class F, int Flags, class IC = int, class ParamTypes = w_internal::StringViewArray<>, class ParamNames = w_internal::StringViewArray<>>
struct MetaMethodInfoBuilder {
    StringView name;
    F func;
    ParamTypes paramTypes{};
    ParamNames paramNames{};
    static constexpr int flags = Flags;
    using IntegralConstant = IC;

    template<class... Args, class = std::enable_if_t<(std::is_same_v<std::decay_t<Args>, StringView> && ...)>>
    constexpr auto setParamTypes(Args... paramTypes) const
        -> MetaMethodInfoBuilder<F, Flags, IC, w_internal::StringViewArray<sizeof... (Args)>, ParamNames> {
        return {name, func, {paramTypes...}, paramNames};
    }
    template<class... Args, class = std::enable_if_t<(std::is_same_v<std::decay_t<Args>, StringView> && ...)>>
    constexpr auto setParamNames(Args... paramNames) const
        -> MetaMethodInfoBuilder<F, Flags, IC, ParamTypes, w_internal::StringViewArray<sizeof... (Args)>> {
        return {name, func, paramTypes, {paramNames...}};
    }
    template<int... fs>
    constexpr auto addFlags(w_internal::W_MethodFlags<fs>...) const -> MetaMethodInfoBuilder<F, (fs | ... | Flags), IC, ParamTypes, ParamNames> {
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

template<typename F>
constexpr auto makeSignalBuilder(StringView name, F func) {
    return MetaMethodInfoBuilder<F, W_MethodType::Signal.value>{name, func};
}


} // namespace w_cpp

/// \macro W_CPP_PROPERTY(callback)
/// create properties from a constexpr callback method.
///
/// example usage:
///     template<size_t I, class = std::enable_if_t<(I<2)>>
///     static constexpr auto myProperties() {
///         using namespace w_cpp;
///         if constexpr (I == 0) return makeProperty<QString>(viewLiteral("name"), viewLiteral("QString")).setGetter(&Example::getName);
///         else return makeProperty<int>("age", "int").setMember(&Example::age);
///     }
///     W_CPP_PROPERTY(myProperties)
///
/// NOTE: you have to ensure that your callback limist valid I.
#define W_CPP_PROPERTY(a) \
    static constexpr size_t W_MACRO_CONCAT(a,_O) = w_internal::stateCount<__COUNTER__, w_internal::PropertyStateTag, W_ThisType**>; \
    template<size_t I> \
    friend constexpr auto w_state(w_internal::Index<I>, w_internal::PropertyStateTag, W_ThisType**) -> decltype(a<I-W_MACRO_CONCAT(a,_O)>()) { \
        return a<I-W_MACRO_CONCAT(a,_O)>(); }

#define W_CPP_SIGNAL(a) \
    static constexpr size_t W_MACRO_CONCAT(a,_O) = w_internal::stateCount<__COUNTER__, w_internal::SignalStateTag, W_ThisType**>; \
    template<size_t I> \
    friend constexpr auto w_state(w_internal::Index<I>, w_internal::SignalStateTag, W_ThisType**) W_RETURN((a<I-W_MACRO_CONCAT(a,_O)>()))

#define W_CPP_SIGNAL_IMPL(type, a, i, ...) { \
    constexpr int index = W_ThisType::W_MACRO_CONCAT(a,_O) + i; \
    return w_internal::SignalImplementation<type, index>{this}(__VA_ARGS__); \
}
