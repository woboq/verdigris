/**
  This file presents Verdigris C++ API.

  This allows you to add multiple properties with a single macro invocation.

  You will create a basic PropertyHolder template that allows you to very easily make
  Properties available to QML.
 */

/** ******************************************************************************************** **/
/** INTRODUCTION **/

// Include the C++ API for Verdigris
#include <wobjectcpp.h>

// To implement the template QObjects you need the Verdigris implementation as well
#include <wobjectimpl.h>

// And because you will inherit from QObject you also need to QObject header
#include <QObject>

// To simplify matters your properties make use of QVariant
#include <QVariant>

// std::tuple will handle storage for your properties
#include <tuple>

// You want to represent each property as a strong type
// note: This is a very basic strong type template but sufficient for this tutorial
template<class V, class... Tags>
struct Strong {
    V v;
};

// Use the Strong type template to define the person properties
using Name = Strong<QString, struct NameTag>;
using BirthYear = Strong<int, struct BirthYearTag>;

// This wrapper allows you to build constexpr functions that depend on non constexpr types.
// For example a `QString` would not be instantiatable at compile time, but `Wrap<QString>` is.
template<class T>
struct Wrap {};

// Define names for the properties as constexpr functions.
// These names are used by QML to access the properties
constexpr auto getPropertyName(Wrap<Name>) { return w_cpp::viewLiteral("name"); }
constexpr auto getPropertyName(Wrap<BirthYear>) { return w_cpp::viewLiteral("birthYear"); }

// Also define the names for the changed signals as constexpr functions.
// Hint: You might generate these names programmatically from the PropertyName at compile time.
constexpr auto getPropertyChangedName(Wrap<Name>) { return w_cpp::viewLiteral("nameChanged"); }
constexpr auto getPropertyChangedName(Wrap<BirthYear>) { return w_cpp::viewLiteral("birthYearChanged"); }

// Now it's time to build the GenericPropertyHolder as a templated QObject.
// Thanks to Verdigris it accepts any number of strong typed properties as template arguments.
template<class... Properties>
class GenericPropertyHolder : public QObject {
    // W_OBJECT macro as usual for Verdigris
    W_OBJECT(GenericPropertyHolder)

    // To keep matters simple you store the properties as a std::tuple for now
    using Storage = std::tuple<Properties...>;
    Storage m_storage{};

    // You can also use std::tuple to resolve the property type for a given index.
    template<size_t I>
    using PropertyAt = std::remove_reference_t<decltype (std::get<I>(std::declval<Storage>()))>;

public:
    // A constructor that fills the intial values for all the properties
    GenericPropertyHolder(Properties... properties, QObject* parent = {})
        : QObject(parent), m_storage(std::move(properties)...) {}

private:
    // Now you need to implement Signals, Getter, Setter and registrations for all properties.
    // To prevent repetition you can simply create indexed template functions and use the C++ Verdigris API.

    // Create a property changed signal implemenation for all properties in one template
    template<size_t I>
    void propertyChanged() {
        W_CPP_SIGNAL_IMPL(decltype (&GenericPropertyHolder::propertyChanged<I>), PropertyChangedSignals, I, 0);
    }

    // Register a property changed signal for all properties
    // Verdigris increments I starting with 0 until this template is invalid.
    // So you should limit the instantiation to the number of Properties.
    template<size_t I> requires(I < sizeof...(Properties))
    struct PropertyChangedSignals {
        using Property = PropertyAt<I>;
        constexpr static auto signalName = getPropertyChangedName(Wrap<Property>{});
        constexpr static auto getSignal() {
            return &GenericPropertyHolder::propertyChanged<I>;
        }
        // signal is the only thing used by the W_CPP_SIGNAL macro.
        constexpr static auto signal =
            w_cpp::makeSignalBuilder(signalName, &PropertyChangedSignals::getSignal).build();
    };
    // The W_CPP_SIGNAL macro registers our PropertyChangedSignals template to Verdigris
    W_CPP_SIGNAL(PropertyChangedSignals);

    // Next you should implement the getter and setter functions for all properties.
    // QML does not care about our C++ types so you can convert them directly to QVariant.
    // notice: For useful C++ Api you would probably want to use your strong types instead of indices anyways.
    template<size_t I>
    auto getPropertyValue() const -> QVariant {
        const auto& property = std::get<I>(m_storage);
        return QVariant::fromValue(property.v);
    }
    template<size_t I>
    void setPropertyValue(QVariant variant) {
        auto& property = std::get<I>(m_storage);
        // Before you can set the property value you have to extract the right type from the QVariant
        using Property = PropertyAt<I>;
        using ValueType = decltype (std::declval<Property>().v);
        property.v = variant.value<ValueType>();
        propertyChanged<I>(); // send notification for the property
    }

    // Finally you have everything to register all your properties
    // Again Verdigris increments I starting with 0 until this template is no longer valid.
    template<size_t I> requires(I < sizeof...(Properties))
    struct RegisterProperties {
        using Property = PropertyAt<I>;
        constexpr static auto name = getPropertyName(Wrap<Property>{});
        // property is the only thing used by the W_CPP_PROPERTY macro.
        constexpr static auto property =
            w_cpp::makeProperty<QVariant>(name, w_cpp::viewLiteral("QVariant"))
                .setGetter(&GenericPropertyHolder::getPropertyValue<I>)
                .setSetter(&GenericPropertyHolder::setPropertyValue<I>)
                .setNotify(&GenericPropertyHolder::propertyChanged<I>);
    };
    W_CPP_PROPERTY(RegisterProperties);
};

// Generate the QObject implementation for all instances of the template
W_OBJECT_IMPL(GenericPropertyHolder<Properties...>, template<class... Properties>)
// note: consider using W_OBJECT_IMPL_INLINE to prevent linker issues when you use the same instances from multiple compile units

// For your person example you can now instanciate our template
using PersonHolder = GenericPropertyHolder<Name, BirthYear>;

constexpr auto w_explicitObjectName(PersonHolder*) {
    return w_cpp::viewLiteral("PersonHolder");
}


// Implement a QML Quick application
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[]) {
    QGuiApplication app{argc, argv};

    QQmlApplicationEngine engine{};

    // register your PersonHolder for QML as uncreatable type
    qmlRegisterUncreatableType<PersonHolder>("CppTutorial", 1, 0, "PersonHolder", "");

    // create a famous person of your liking
    PersonHolder person{Name{"Albert Einstein"}, BirthYear{1879}, {}};

    // make this person accessable as global context property in QML
    auto context = engine.rootContext();
    context->setContextProperty("person", &person);

    // now you can use it like any other QObject with properties from QML
    engine.load(QStringLiteral("qrc:/main.qml"));
    return app.exec();
}

// Notice that with the GenericPropertyHolder template in place, you can add more properties very easy.
