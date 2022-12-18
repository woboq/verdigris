#pragma once
#include "wobjectimpl.h"

#include <variant>

// ___ decl ___
namespace w_internal {

struct QmlFriendHelper;

} // namespace w_internal

// mirrors supported macros from qqmlintegration.h
#define W_QML_ELEMENT                                                                                                  \
    W_CLASSINFO("QML.Element", "auto");                                                                                \
    friend struct w_internal::QmlFriendHelper;                                                                         \
    static auto w_qmlElementData()->const void*;                                                                       \
    static constexpr auto W_Header = w_internal::viewLiteral(__FILE__)

#define W_QML_ANONYMOUS                                                                                                \
    W_CLASSINFO("QML.Element", "anonymous");                                                                           \
    friend struct w_internal::QmlFriendHelper;                                                                         \
    static auto w_qmlElementData()->const void*;                                                                       \
    static constexpr auto W_Header = w_internal::viewLiteral(__FILE__)

#define W_QML_NAMED_ELEMENT(NAME)                                                                                      \
    W_CLASSINFO("QML.Element", #NAME);                                                                                 \
    friend struct w_internal::QmlFriendHelper;                                                                         \
    static auto w_qmlElementData()->const void*;                                                                       \
    static constexpr auto W_Header = w_internal::viewLiteral(__FILE__)

#define W_QML_UNCREATABLE(REASON)                                                                                      \
    W_CLASSINFO("QML.Creatable", "false")                                                                              \
    W_CLASSINFO("QML.UncreatableReason", REASON)

#define W_QML_VALUE_TYPE(NAME)                                                                                         \
    W_CLASSINFO("QML.Element", #NAME)                                                                                  \
    friend struct w_internal::QmlFriendHelper;                                                                         \
    static auto w_qmlElementData()->const void*;                                                                       \
    static constexpr auto W_Header = w_internal::viewLiteral(__FILE__);                                                \
    W_QML_UNCREATABLE("Value types cannot be created.")

// ___ impl ___
#if defined(Q_CC_MSVC) && !defined(Q_CC_CLANG)
#pragma section("qmltypes", read, discard)
#define W_QML_TYPES_SECTION_DECLSPEC __declspec(allocate("qmltypes"))
#define W_QML_TYPES_SECTION_ATTRIBUTE
#else
#define W_QML_TYPES_SECTION_DECLSPEC
#define W_QML_TYPES_SECTION_ATTRIBUTE __attribute__((section("qmltypes")))
#endif

namespace w_internal {

template<typename... Ts> struct Overload : Ts... {
    using Ts::operator()...;
};
template<typename... Ts> Overload(Ts...) -> Overload<Ts...>;

struct QuotedStringView {
    StringView wrapped;
};

struct JsonValue;
using JsonArray = std::vector<JsonValue>;
struct JsonKeyValue;
using JsonObject = std::vector<JsonKeyValue>;
struct JsonValue {
    using Data = std::variant<std::monostate, bool, int, StringView, QuotedStringView, JsonArray, JsonObject>;
    Data data;

    constexpr JsonValue() = default;
    constexpr JsonValue(bool v);
    constexpr JsonValue(int v);
    constexpr JsonValue(StringView sv);
    constexpr JsonValue(QuotedStringView qsv);
    constexpr JsonValue(JsonArray ja);
    constexpr JsonValue(JsonObject jo);

    static constexpr auto makeArray() -> JsonValue;
    static constexpr auto makeObject() -> JsonValue;

    constexpr bool hasValue() const;
    constexpr auto encodeSize() const -> size_t;
    constexpr void encodeTo(char*& p) const;

    constexpr void addKeyValue(StringView key, JsonValue value);
    constexpr void append(JsonValue value);

private:
    static constexpr auto trueSv = viewLiteral("true");
    static constexpr auto falseSv = viewLiteral("false");
    static constexpr auto intSize(int value) -> size_t {
        auto sum = size_t{};
        if (value < 0) sum += 1;
        auto abs = value > 0 ? value : -value;
        while (abs >= 10) {
            sum += 1;
            abs /= 10;
        }
        return sum + 1;
    }
    static constexpr auto escapedStringSize(const StringView& sv) -> size_t {
        auto sum = static_cast<size_t>(sv.size());
        for (auto c : sv) {
            if (c == '\\' || c == '"') sum++;
        }
        return sum;
    }
};
struct JsonKeyValue {
    JsonValue key;
    JsonValue value;

    constexpr JsonKeyValue() = default;
    constexpr JsonKeyValue(JsonValue key, JsonValue value)
        : key{std::move(key)}
        , value{std::move(value)} {}
};
constexpr JsonValue::JsonValue(bool v)
    : data{v} {}
constexpr JsonValue::JsonValue(int v)
    : data{v} {}
constexpr JsonValue::JsonValue(StringView sv)
    : data{sv} {}
constexpr JsonValue::JsonValue(QuotedStringView qsv)
    : data{qsv} {}
constexpr JsonValue::JsonValue(JsonArray ja)
    : data{ja} {}
constexpr JsonValue::JsonValue(JsonObject jo)
    : data{jo} {}

constexpr auto JsonValue::makeArray() -> JsonValue { return {JsonArray{}}; }
constexpr auto JsonValue::makeObject() -> JsonValue { return {JsonObject{}}; }

constexpr bool JsonValue::hasValue() const {
    auto result = false;
    std::visit(
        Overload{
            [&](std::monostate) {},
            [&](bool) { result = true; },
            [&](int) { result = true; },
            [&](const StringView& sv) { result = sv.size() > 0; },
            [&](const QuotedStringView& qsv) { result = qsv.wrapped.size() > 0; },
            [&](const JsonArray& arr) { result = arr.size() > 0; },
            [&](const JsonObject& obj) {
                for (const auto& entry : obj) {
                    if (entry.key.hasValue() && entry.value.hasValue()) {
                        result = true;
                        break;
                    }
                }
            },
        },
        data);
    return result;
}
constexpr auto JsonValue::encodeSize() const -> size_t {
    auto sum = size_t{};
    std::visit(
        Overload{
            [&](std::monostate) {},
            [&](bool v) { sum = v ? trueSv.size() : falseSv.size(); },
            [&](int v) { sum = intSize(v); },
            [&](const StringView& sv) { sum = 1 + escapedStringSize(sv) + 1; },
            [&](const QuotedStringView& qsv) { sum = 3 + escapedStringSize(qsv.wrapped) + 3; },
            [&](const JsonArray& arr) {
                sum = 1;
                for (auto& entry : arr) {
                    if (sum != 0) sum += 1;
                    sum += entry.encodeSize();
                }
                sum += 1;
            },
            [&](const JsonObject& obj) {
                sum = 1;
                for (const auto& entry : obj) {
                    if (entry.key.hasValue() && entry.value.hasValue()) {
                        if (sum != 0) sum += 1;
                        sum += entry.key.encodeSize() + 1 + entry.value.encodeSize();
                    }
                }
                sum += 1;
            },
        },
        data);
    return sum;
}
constexpr void JsonValue::encodeTo(char*& p) const {
    std::visit(
        Overload{
            [&](std::monostate) {},
            [&](bool v) {
                for (auto c : (v ? trueSv : falseSv)) *p++ = c;
            },
            [&](int v) {
                auto e = p + intSize(v);
                if (v < 0) *p = '-';
                p = e--;
                auto abs = v > 0 ? v : -v;
                while (abs >= 10) {
                    *e-- = '0' + (abs % 10);
                    abs /= 10;
                }
                *e = '0' + abs;
            },
            [&](const StringView& sv) {
                *p++ = '"';
                for (auto c : sv) {
                    if (c == '\\' || c == '"') *p++ = '\\';
                    *p++ = c;
                }
                *p++ = '"';
            },
            [&](const QuotedStringView& qsv) {
                *p++ = '"';
                *p++ = '\\';
                *p++ = '"';
                for (auto c : qsv.wrapped) {
                    if (c == '\\' || c == '"') *p++ = '\\';
                    *p++ = c;
                }
                *p++ = '\\';
                *p++ = '"';
                *p++ = '"';
            },
            [&](const JsonArray& arr) {
                *p++ = '[';
                const auto b = p;
                for (auto& entry : arr) {
                    if (p != b) *p++ = ',';
                    entry.encodeTo(p);
                }
                *p++ = ']';
            },
            [&](const JsonObject& obj) {
                *p++ = '{';
                const auto b = p;
                for (auto& entry : obj) {
                    if (entry.key.hasValue() && entry.value.hasValue()) {
                        if (p != b) *p++ = ',';
                        entry.key.encodeTo(p);
                        *p++ = ':';
                        entry.value.encodeTo(p);
                    }
                }
                *p++ = '}';
            },
        },
        data);
}

constexpr void JsonValue::addKeyValue(StringView key, JsonValue value) {
    std::get<JsonObject>(data).emplace_back(key, std::move(value));
}

constexpr void JsonValue::append(JsonValue value) { std::get<JsonArray>(data).emplace_back(std::move(value)); }

static_assert(JsonValue{68}.encodeSize() == 2);
static_assert(JsonValue{viewLiteral("ab")}.encodeSize() == 4);

template<size_t N> struct StringArray {
    RawArray<char, N == 0 ? 1 : N> data;
};
template<size_t N, typename Json> constexpr auto jsonToStringArray(const Json& json) -> StringArray<N> {
    auto result = StringArray<N>{};
    auto* p = &result.data[0];
    json.encodeTo(p);
    return result;
}

template<typename T>
concept is_qgadget = requires(T& gad) { gad.qt_check_for_QGADGET_macro(); };

template<typename T>
concept is_qobject = !
is_qgadget<T>&& requires(T& obj, const char* name, void* result) { result = obj.T::qt_metacast(name); };

template<typename T>
concept has_className = requires(StringView out) { out = T::W_UnscopedName; };

static constexpr uint operator&(uint a, PropertyFlags b) { return a & static_cast<uint>(b); }

struct QmlFriendHelper {
    template<typename T, size_t... Indices> static constexpr auto createClassInfos(std::index_sequence<Indices...>) {
        using TPP = T**;
        auto result = JsonValue::makeArray();
        constexpr auto createEntry = [&]<size_t Idx>(Index<Idx>) {
            constexpr auto data = w_state(index<Idx>, ClassInfoStateTag{}, TPP{});
            auto json = JsonValue::makeObject();
            json.addKeyValue(viewLiteral("name"), JsonValue{data.first});
            json.addKeyValue(viewLiteral("value"), JsonValue{data.second});
            result.append(std::move(json));
        };
        (createEntry(index<Indices>), ...);
        return std::move(result);
    }

    template<size_t N> static constexpr auto createEnumValues(const StringViewArray<N>& names) {
        auto result = JsonValue::makeArray();
        for (auto i = size_t{}; i < N; i++) {
            result.append(names[i]);
        }
        return std::move(result);
    }

    template<typename T, size_t... Indices> static constexpr auto createEnums(std::index_sequence<Indices...>) {
        using TPP = T**;
        auto result = JsonValue::makeArray();
        constexpr auto createEntry = [&]<size_t Idx>(Index<Idx>) {
            constexpr auto data = w_state(index<Idx>, EnumStateTag{}, TPP{});
            auto json = JsonValue::makeObject();
            json.addKeyValue(viewLiteral("name"), JsonValue{data.name});
            if (data.hasAlias) {
                json.addKeyValue(viewLiteral("alias"), JsonValue{data.alias});
            }
            json.addKeyValue(viewLiteral("isFlag"), (0 != (data.flags & 1)));
            json.addKeyValue(viewLiteral("isClass"), (0 != (data.flags & 2)));
            json.addKeyValue(viewLiteral("values"), createEnumValues(data.names));
            result.append(std::move(json));
        };
        (createEntry(index<Indices>), ...);
        return std::move(result);
    }

    template<typename T> static constexpr auto createSuperClasses() {
        auto result = JsonValue::makeArray();
        constexpr const QMetaObject* meta = parentMetaObject<T>();
        if constexpr (meta) {
            using Base = typename T::W_BaseType;
            if constexpr (has_className<Base>) {
                auto json = JsonValue::makeObject();
                json.addKeyValue(viewLiteral("name"), JsonValue{Base::W_UnscopedName});
                result.append(std::move(json));
            }
            else {
                auto json = JsonValue::makeObject();
                json.addKeyValue(viewLiteral("name"), viewTypeName<Base>());
                json.addKeyValue(viewLiteral("access"), viewLiteral("public"));
                result.append(std::move(json));
            }
        }
        return std::move(result);
    }

    template<typename T, typename TypeStr = int> static constexpr auto viewTypeName(TypeStr v = {}) {
        if constexpr (BuiltinMetaTypeId<T>) {
            constexpr auto& name = QMetaTypeId2<T>::nameAsArray;
            return StringView{name.data(), name.data() + name.size() - 1};
        }
        else if constexpr (std::is_same_v<std::decay_t<TypeStr>, StringView>) {
            return v;
        }
        else if constexpr (QMetaTypeId<T>::Defined || QMetaTypeId<T*>::Defined) {
            auto b = QMetaType::fromType<T>().name();
            auto e = b;
            while (*e != 0) e++;
            return StringView{b, e};
        }
        else {
            static_assert(W_TypeRegistery<T>::registered, "Please Register T with W_REGISTER_ARGTYPE");
            return W_TypeRegistery<T>::name;
        }
    }

    template<class Arg, class TypeName>
    static constexpr auto createArgument(const TypeName& typeName, const auto& name) {
        auto json = JsonValue::makeObject();
        using Type = typename QtPrivate::RemoveConstRef<Arg>::Type;
        auto typeName2 = std::conditional_t<std::is_same<Arg, Type>::value, TypeName, std::tuple<TypeName>>{typeName};
        json.addKeyValue(viewLiteral("type"), viewTypeName<Arg>(typeName2));
        if (name.size() != 0) {
            json.addKeyValue(viewLiteral("name"), name);
        }
        return std::move(json);
    };
    template<class... Args, class TypeNames, size_t NameCount, size_t... Is>
    static constexpr auto createArguments(
        const TypeNames& typeNames, const StringViewArray<NameCount>& paramNames, const index_sequence<Is...>&) {
        auto result = JsonValue::makeArray();
        (result.append(createArgument<Args>(stringFetch<Is>(typeNames), paramNames[Is])), ...);
        return std::move(result);
    }

    static constexpr auto viewAccess(int flags) -> JsonValue {
        if ((flags & AccessPublic) != 0) {
            return viewLiteral("public");
        }
        if ((flags & AccessPrivate) != 0) {
            return viewLiteral("private");
        }
        if ((flags & AccessProtected) != 0) {
            return viewLiteral("protected");
        }
        return {};
    }

    template<typename T, typename StateTag, size_t... Indices>
    static constexpr auto createFunctions(std::index_sequence<Indices...>) {
        using TPP = T**;
        using ObjI = ObjectInfo<T>;
        constexpr size_t L = ObjI::counter;
        auto result = JsonValue::makeArray();
        constexpr auto createEntry = [&]<class Method, size_t I>(const Method& method, Index<I>) {
            using FP = QtPrivate::FunctionPointer<typename Method::Func>;
            auto json = JsonValue::makeObject();
            json.addKeyValue(viewLiteral("name"), JsonValue{method.name});
            // json.addKeyValue(viewLiteral("tag"), ??);
            json.addKeyValue(viewLiteral("returnType"), viewTypeName<FP::ReturnType>());
            using ArgsPtr = typename FP::Arguments*;
            [&json, &method]<class... Args>(QtPrivate::List<Args...>*) {
                json.addKeyValue(
                    viewLiteral("arguments"),
                    createArguments<Args...>(
                        method.paramTypes, method.paramNames, make_index_sequence<sizeof...(Args)>{}));
            }(ArgsPtr{});
            auto flags = method.flags;
            if (!(flags & (AccessProtected | AccessPrivate | AccessPublic))) {
                // Auto-detect the access specifier
                flags |= IsPublic<T, StateTag, I> ? AccessPublic
                    : IsProtected<T, StateTag, I> ? AccessProtected
                                                  : AccessPrivate;
            }
            json.addKeyValue(viewLiteral("access"), viewAccess(flags));
            // json.addKeyValue(viewLiteral("revision"), ??);
            result.append(std::move(json));
        };
        (createEntry(w_state(index<Indices>, StateTag{}, TPP{}), index<Indices>), ...);
        return std::move(result);
    }

    template<typename T, size_t... Indices> static constexpr auto createConstructor(std::index_sequence<Indices...>) {
        using TPP = T**;
        using ObjI = ObjectInfo<T>;
        constexpr size_t L = ObjI::counter;
        auto result = JsonValue::makeArray();
        constexpr auto createEntry = [&]<class... Args>(const MetaConstructorInfo<Args...>& method) {
            auto json = JsonValue::makeObject();
            json.addKeyValue(viewLiteral("name"), method.name);
            // json.addKeyValue(viewLiteral("tag"), ??);
            json.addKeyValue(viewLiteral("returnType"), viewLiteral(""));
            json.addKeyValue(
                viewLiteral("arguments"),
                createArguments<Args...>(
                    StringViewArray<>{}, StringViewArray<>{}, make_index_sequence<sizeof...(Args)>{}));
            json.addKeyValue(viewLiteral("access"), viewLiteral("public"));
            // json.addKeyValue(viewLiteral("revision"), ??);
            result.append(std::move(json));
        };
        (createEntry(w_state(index<Indices>, ConstructorStateTag{}, TPP{})), ...);
        return std::move(result);
    }

    template<typename T, size_t... Indices> static constexpr auto createProperties(std::index_sequence<Indices...>) {
        using TPP = T**;
        using ObjI = ObjectInfo<T>;
        constexpr size_t L = ObjI::counter;
        auto result = JsonValue::makeArray();
        constexpr auto createEntry = [&]<class Prop, size_t I>(const Prop& prop, Index<I>) {
            auto json = JsonValue::makeObject();
            json.addKeyValue(viewLiteral("name"), prop.name);
            json.addKeyValue(viewLiteral("type"), viewTypeName<typename Prop::PropertyType>(prop.typeStr));

            if (prop.memberStr.size() > 0) json.addKeyValue(viewLiteral("member"), prop.memberStr);
            if (prop.getterStr.size() > 0) json.addKeyValue(viewLiteral("read"), prop.getterStr);
            if (prop.setterStr.size() > 0) json.addKeyValue(viewLiteral("write"), prop.setterStr);
            if (prop.bindableStr.size() > 0) json.addKeyValue(viewLiteral("bindable"), prop.bindableStr);
            if (prop.notifyStr.size() > 0) json.addKeyValue(viewLiteral("notify"), prop.notifyStr);
            // json.addKeyValue(viewLiteral("privateClass"), ??);

            json.addKeyValue(viewLiteral("designable"), (prop.flags & PropertyFlags::Designable) != 0);
            json.addKeyValue(viewLiteral("scriptable"), (prop.flags & PropertyFlags::Scriptable) != 0);
            json.addKeyValue(viewLiteral("stored"), (prop.flags & PropertyFlags::Stored) != 0);
            json.addKeyValue(viewLiteral("user"), (prop.flags & PropertyFlags::User) != 0);

            json.addKeyValue(viewLiteral("constant"), (prop.flags & PropertyFlags::Constant) != 0);
            json.addKeyValue(viewLiteral("final"), (prop.flags & PropertyFlags::Final) != 0);
            json.addKeyValue(viewLiteral("required"), (prop.flags & PropertyFlags::Required) != 0);
            json.addKeyValue(viewLiteral("index"), static_cast<int>(I));
            // json.addKeyValue(viewLiteral("revision"), ??);

            result.append(std::move(json));
        };
        (createEntry(w_state(index<Indices>, PropertyStateTag{}, TPP{}), index<Indices>), ...);
        return std::move(result);
    }

    template<typename T, size_t... Indices> static constexpr auto createInterfaces(std::index_sequence<Indices...>) {
        using TPP = T**;

        auto result = JsonValue::makeArray();
        constexpr auto createEntry = [&]<class A>(const Interface<A>& iface) {
            auto json = JsonValue::makeObject();
            auto* b = qobject_interface_iid<A*>();
            auto* e = b;
            while (*e != 0) e++;
            json.addKeyValue(viewLiteral("id"), QuotedStringView{StringView{b, e}});
            json.addKeyValue(viewLiteral("className"), iface.name);
            auto outerJson = JsonValue::makeArray();
            outerJson.append(std::move(json));
            result.append(std::move(outerJson));
        };
        (createEntry(w_state(index<Indices>, InterfaceStateTag{}, TPP{})), ...);
        return std::move(result);
    }

#ifdef QML_ELEMENT_BASE_PATH
    static constexpr auto basePath = viewLiteral(QML_ELEMENT_BASE_PATH);
#endif
    static constexpr auto shortPathView(const StringView& path) -> StringView {
#ifdef QML_ELEMENT_BASE_PATH
        if (path.size() > basePath.size()) {
            auto p = path.b;
            for (auto c : basePath) {
                auto pc = *p;
                if (pc == c || (pc == '\\' && c == '/')) {
                    p++;
                }
                else {
                    break;
                }
            }
            if (*p == '/' || *p == '\\') p++;
            return StringView{p, path.e};
        }
#endif
        return path;
    }

    template<typename T> static constexpr auto createQmlMetaTypes() {
        using TPP = T**;
        using ObjI = ObjectInfo<T>;
        constexpr size_t L = ObjI::counter;

        auto jsonClass = JsonValue::makeObject();
        jsonClass.addKeyValue(viewLiteral("className"), JsonValue{T::W_UnscopedName});
        jsonClass.addKeyValue(viewLiteral("qualifiedClassName"), JsonValue{T::W_MetaObjectCreatorHelper::fullName});
        jsonClass.addKeyValue(
            viewLiteral("classInfos"),
            createClassInfos<T>(std::make_index_sequence<stateCount<L, ClassInfoStateTag, TPP>()>{}));

        jsonClass.addKeyValue(
            viewLiteral("signals"),
            createFunctions<T, SignalStateTag>(std::make_index_sequence<stateCount<L, SignalStateTag, TPP>()>{}));
        jsonClass.addKeyValue(
            viewLiteral("slots"),
            createFunctions<T, SlotStateTag>(std::make_index_sequence<stateCount<L, SlotStateTag, TPP>()>{}));
        jsonClass.addKeyValue(
            viewLiteral("constructors"),
            createConstructor<T>(std::make_index_sequence<stateCount<L, ConstructorStateTag, TPP>()>{}));
        jsonClass.addKeyValue(
            viewLiteral("methods"),
            createFunctions<T, MethodStateTag>(std::make_index_sequence<stateCount<L, MethodStateTag, TPP>()>{}));

        jsonClass.addKeyValue(
            viewLiteral("properties"),
            createProperties<T>(std::make_index_sequence<stateCount<L, PropertyStateTag, TPP>()>{}));

        if constexpr (is_qobject<T>) {
            jsonClass.addKeyValue(viewLiteral("object"), true);
        }
        if constexpr (is_qgadget<T>) {
            jsonClass.addKeyValue(viewLiteral("gadget"), true);
        }

        jsonClass.addKeyValue(
            viewLiteral("enums"), createEnums<T>(std::make_index_sequence<stateCount<L, EnumStateTag, TPP>()>{}));

        jsonClass.addKeyValue(viewLiteral("superClasses"), createSuperClasses<T>());

        jsonClass.addKeyValue(
            viewLiteral("interfaces"),
            createInterfaces<T>(std::make_index_sequence<stateCount<L, InterfaceStateTag, TPP>()>{}));

        auto json = JsonValue::makeObject();
        json.addKeyValue(viewLiteral("outputRevision"), JsonValue{68});
        json.addKeyValue(viewLiteral("inputFile"), shortPathView(T::W_Header));
        auto classesArray = JsonValue::makeArray();
        classesArray.append(std::move(jsonClass));
        json.addKeyValue(viewLiteral("classes"), std::move(classesArray));
        return std::move(json);
    }

    template<typename T> static constexpr auto createQmlTypes() {
        constexpr auto resultSize = createQmlMetaTypes<T>().encodeSize();
        constexpr auto result = jsonToStringArray<resultSize>(createQmlMetaTypes<T>());
        return result;
    }

    template<typename T> static auto qmlElementData() -> const void* {
        W_QML_TYPES_SECTION_DECLSPEC static constinit const auto qmltypes W_QML_TYPES_SECTION_ATTRIBUTE =
            w_internal::QmlFriendHelper::createQmlTypes<T>();
        return &qmltypes.data;
    }
};

} // namespace w_internal

#define W_QML_ELEMENT_IMPL(...)                                                                                        \
    W_OBJECT_IMPL(__VA_ARGS__)                                                                                         \
    W_MACRO_TEMPLATE_STUFF(__VA_ARGS__) auto W_MACRO_FIRST_REMOVEPAREN(__VA_ARGS__)::w_qmlElementData()->const void* { \
        return w_internal::QmlFriendHelper::qmlElementData<typename W_MACRO_FIRST_REMOVEPAREN(                         \
            __VA_ARGS__)::W_ThisType>();                                                                               \
    }
