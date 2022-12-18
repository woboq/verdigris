#pragma once
#include <QtQuick/QQuickItem>

#include <wqmlelement.h>

class DemoInterface {
public:
    virtual ~DemoInterface() = default;
};

QT_BEGIN_NAMESPACE

#define DemoInterface_iid "demo.DemoInterface"
Q_DECLARE_INTERFACE(DemoInterface, DemoInterface_iid)

QT_END_NAMESPACE

class MyObject : public QQuickItem, public DemoInterface
{
    W_OBJECT(MyObject)
    W_QML_ELEMENT;
    W_INTERFACE(DemoInterface);

    int m_demoProp{};

public:
    MyObject();
    W_CONSTRUCTOR();

    enum class DemoEnum { One, Two, Three = 4 };
    W_ENUM(DemoEnum, DemoEnum::One, DemoEnum::Two, DemoEnum::Three);

    void mySlot(const QString &name) { qDebug("hello %s", qPrintable(name)); }
    W_SLOT(mySlot, (const QString&));

    void mySignal(const QString &name)
    W_SIGNAL(mySignal, name);

    int demoProp() const;
    void setDemoProp(int newDemoProp);
    void demoPropChanged() W_SIGNAL(demoPropChanged);
    W_PROPERTY(int, demoProp READ demoProp WRITE setDemoProp NOTIFY demoPropChanged);
};
