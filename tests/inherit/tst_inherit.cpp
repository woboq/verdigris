#include "object.h"

#include <wobjectdefs.h>
#include <QtTest/QtTest>

class tst_Inherit : public QObject
{
    W_OBJECT(tst_Inherit)

private slots:
    void connect1();
    W_SLOT(connect1);

    void connect2();
    W_SLOT(connect2);

    void connect3();
    W_SLOT(connect3);
};

void tst_Inherit::connect1()
{
    auto obj = Object{};
    auto eventCount1 = int{};

    connect(&obj, Object::myPtr(), this, [&]() {
        eventCount1++;
    });
    obj.emit1();
    QCOMPARE(eventCount1, 1);
    obj.emit2();
    QCOMPARE(eventCount1, 1);
    obj.emit3();
    QCOMPARE(eventCount1, 1);
}

void tst_Inherit::connect2()
{
    auto obj = Object{};
    auto eventCount2 = int{};

    connect(&obj, &Object::handleEvent2, this, [&]() {
        eventCount2++;
    });
    obj.emit1();
    QCOMPARE(eventCount2, 0);
    obj.emit2();
    QCOMPARE(eventCount2, 1);
    obj.emit3();
    QCOMPARE(eventCount2, 1);
}

void tst_Inherit::connect3()
{
    auto obj = Object{};
    auto eventCount3 = int{};

    connect(&obj, &Object::handleEvent3, this, [&]() {
        eventCount3++;
    });
    obj.emit1();
    QCOMPARE(eventCount3, 0);
    obj.emit2();
    QCOMPARE(eventCount3, 0);
    obj.emit3();
    QCOMPARE(eventCount3, 1);
}

QTEST_MAIN(tst_Inherit)

#include "wobjectimpl.h"

W_OBJECT_IMPL(tst_Inherit)
