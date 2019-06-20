#include <wobjectcpp.h>
#include <wobjectimpl.h>
#include <QtCore/QMetaObject>
#include <QtTest/QtTest>

class tst_CppApi : public QObject
{
    W_OBJECT(tst_CppApi)

#if !defined(Q_CC_GNU) || defined(Q_CC_CLANG) // GCC refuses to properly resolve registered signals for properties

private slots:
    void firstTest();
    W_SLOT(firstTest, W_Access::Private)
    void notifyTest();
    W_SLOT(notifyTest, W_Access::Private)

private:
    QString m_name{};
public:
    int m_age{};

public:
    QString getName() const { return m_name; }

    template<size_t I>
    void notifyPropertyChanged() W_CPP_SIGNAL_IMPL(decltype (&tst_CppApi::notifyPropertyChanged<I>), MySignals, I, 0)

#if __cplusplus > 201700L
    template<size_t I>
    static constexpr auto signalName() {
        if constexpr (I == 0) return w_cpp::viewLiteral("nameChanged");
        else if constexpr (I == 1) return w_cpp::viewLiteral("ageChanged");
    }
    template<size_t I, class = std::enable_if_t<(I < 2)>>
    struct MySignals {
        constexpr static auto signal = w_cpp::makeSignalBuilder(signalName<I>(), &tst_CppApi::notifyPropertyChanged<I>).build();
    };
#else
    template<size_t I>
    struct MySignals;
    template<>
    struct MySignals<0> {
        constexpr static auto signal = w_cpp::makeSignalBuilder(w_cpp::viewLiteral("nameChanged"), &tst_CppApi::notifyPropertyChanged<0>).build();
    };
    template<>
    struct MySignals<1> {
        constexpr static auto signal = w_cpp::makeSignalBuilder(w_cpp::viewLiteral("ageChanged"), &tst_CppApi::notifyPropertyChanged<1>).build();
    };
#endif
    W_CPP_SIGNAL(MySignals)

private:
#if __cplusplus > 201700L
    template<size_t I, class = std::enable_if_t<(I < 2)>>
    struct MyProperties {
        constexpr static auto property = []{
            using namespace w_cpp;
            if constexpr (I == 0) {
                return makeProperty<QString>(viewLiteral("name"), viewLiteral("QString"))
                    .setGetter(&tst_CppApi::getName)
                    .setNotify(&tst_CppApi::notifyPropertyChanged<0>);
            }
            else if constexpr (I == 1) {
                return makeProperty<int>(viewLiteral("age"), viewLiteral("int"))
                    .setMember(&tst_CppApi::m_age)
                    .setNotify(&tst_CppApi::notifyPropertyChanged<1>);
            }
        }();
    };
#else
    template<size_t I>
    struct MyProperties;
    template<>
    struct MyProperties<0> {
        constexpr static auto property = w_cpp::makeProperty<QString>(w_cpp::viewLiteral("name"), w_cpp::viewLiteral("QString"))
                .setGetter(&tst_CppApi::getName)
                .setNotify(&tst_CppApi::notifyPropertyChanged<0>);
    };
    template<>
    struct MyProperties<1> {
        constexpr static auto property = w_cpp::makeProperty<int>(w_cpp::viewLiteral("age"), w_cpp::viewLiteral("int"))
                .setMember(&tst_CppApi::m_age)
                .setNotify(&tst_CppApi::notifyPropertyChanged<1>);
    };
#endif
    W_CPP_PROPERTY(MyProperties)
};

void tst_CppApi::firstTest()
{
    const QMetaObject *mo = metaObject();

    QMetaProperty prop = mo->property(mo->indexOfProperty("name"));
    QVERIFY(prop.isValid());
    QVERIFY(!prop.isConstant());

    m_name = "Hello";
    auto v = prop.read(this);
    QCOMPARE(QVariant(m_name), v);

    prop = mo->property(mo->indexOfProperty("age"));
    QVERIFY(prop.isValid());
    QVERIFY(!prop.isConstant());

    prop.write(this, 23);
    QCOMPARE(23, m_age);
}

void tst_CppApi::notifyTest()
{
    bool notified0 = false;
    auto conn0 = connect(this, &tst_CppApi::notifyPropertyChanged<0>, [&](){
        notified0 = true;
    });
    notifyPropertyChanged<0>();

    QVERIFY(notified0);

    bool notified1 = false;
    auto conn1 = connect(this, &tst_CppApi::notifyPropertyChanged<1>, [&](){
        notified1 = true;
    });
    setProperty("age", 100);
    QVERIFY(notified1);

    disconnect(conn0);
    disconnect(conn1);
}

#else // Q_CC_GNU
};

#endif

W_OBJECT_IMPL(tst_CppApi)

QTEST_MAIN(tst_CppApi)
