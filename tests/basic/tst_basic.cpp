#include <QtTest/QtTest>
#include <nickelspace.h>

#undef Q_PROPERTY
#define WRITE , &ThisType::
#define READ , &ThisType::
#define NOTIFY , &ThisType::
#define MEMBER , &ThisType::

#define Q_PROPERTY(...)  Q_PROPERTY_IMPL(__VA_ARGS__)  /* expands the WRITE and READ macro */
#define Q_PROPERTY_IMPL(...) W_PROPERTY(__VA_ARGS__)


class tst_Basic : public QObject
{
    W_OBJECT(tst_Basic)

private slots:
    void firstTest();
    W_SLOT_2(firstTest, W_Access::Private)
};

W_OBJECT_IMPL(tst_Basic)

void tst_Basic::firstTest()
{
    qWarning() << "yay";
}


QTEST_MAIN(tst_Basic)
