/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtCore>
#include <qtest.h>
#include "object.h"
#include <qcoreapplication.h>
#include <qdatetime.h>

enum {
    CreationDeletionBenckmarkConstant = 34567,
    SignalsAndSlotsBenchmarkConstant = 456789
};

class QObjectBenchmark : public QObject
{
Q_OBJECT
private slots:
    void signal_slot_benchmark();
    void signal_slot_benchmark_data();

    void connect_disconnect_benchmark_data();
    void connect_disconnect_benchmark();
};

struct Functor {
    void operator()(){}
};

void QObjectBenchmark::signal_slot_benchmark_data()
{
    QTest::addColumn<int>("type");
    QTest::addColumn<bool>("w");
    QTest::newRow("simple function") << 0    << false;
    QTest::newRow("single signal/slot") << 1 << false;
    QTest::newRow("multi signal/slot") << 2  << false;
    QTest::newRow("unconnected signal") << 3 << false;
    QTest::newRow("single signal/ptr") << 4  << false;
    QTest::newRow("functor") << 5            << false;

    QTest::newRow("w simple function") << 0    << true;
    QTest::newRow("w single signal/slot") << 1 << true;
    QTest::newRow("w multi signal/slot") << 2  << true;
    QTest::newRow("w unconnected signal") << 3 << true;
    QTest::newRow("w single signal/ptr") << 4  << true;
    QTest::newRow("w functor") << 5            << true;

}

template<typename Object>
void signal_slot_benchmark()
{
    QFETCH(int, type);

    Object singleObject;
    Object multiObject;
    Functor functor;
    singleObject.setObjectName("single");
    multiObject.setObjectName("multi");

    if (type == 5) {
        QObject::connect(&singleObject, &Object::signal0, functor);
    } else if (type == 4) {
        QObject::connect(&singleObject, &Object::signal0, &singleObject, &Object::slot0);
    } else {
        singleObject.connect(&singleObject, SIGNAL(signal0()), SLOT(slot0()));
    }

    multiObject.connect(&multiObject, SIGNAL(signal0()), SLOT(slot0()));
    // multiObject.connect(&multiObject, SIGNAL(signal0()), SLOT(signal1()));
    multiObject.connect(&multiObject, SIGNAL(signal1()), SLOT(slot1()));
    // multiObject.connect(&multiObject, SIGNAL(signal0()), SLOT(signal2()));
    multiObject.connect(&multiObject, SIGNAL(signal2()), SLOT(slot2()));
    // multiObject.connect(&multiObject, SIGNAL(signal0()), SLOT(signal3()));
    multiObject.connect(&multiObject, SIGNAL(signal3()), SLOT(slot3()));
    // multiObject.connect(&multiObject, SIGNAL(signal0()), SLOT(signal4()));
    multiObject.connect(&multiObject, SIGNAL(signal4()), SLOT(slot4()));
    // multiObject.connect(&multiObject, SIGNAL(signal0()), SLOT(signal5()));
    multiObject.connect(&multiObject, SIGNAL(signal5()), SLOT(slot5()));
    // multiObject.connect(&multiObject, SIGNAL(signal0()), SLOT(signal6()));
    multiObject.connect(&multiObject, SIGNAL(signal6()), SLOT(slot6()));
    // multiObject.connect(&multiObject, SIGNAL(signal0()), SLOT(signal7()));
    multiObject.connect(&multiObject, SIGNAL(signal7()), SLOT(slot7()));
    // multiObject.connect(&multiObject, SIGNAL(signal0()), SLOT(signal8()));
    multiObject.connect(&multiObject, SIGNAL(signal8()), SLOT(slot8()));
    // multiObject.connect(&multiObject, SIGNAL(signal0()), SLOT(signal9()));
    multiObject.connect(&multiObject, SIGNAL(signal9()), SLOT(slot9()));

    if (type == 0) {
        QBENCHMARK {
            singleObject.slot0();
        }
    } else if (type == 1) {
        QBENCHMARK {
            singleObject.emitSignal0();
        }
    } else if (type == 2) {
        QBENCHMARK {
            multiObject.emitSignal0();
        }
    } else if (type == 3) {
        QBENCHMARK {
            singleObject.emitSignal1();
        }
    } else if (type == 4 || type == 5) {
        QBENCHMARK {
            singleObject.emitSignal0();
        }
    }
}

void QObjectBenchmark::signal_slot_benchmark()
{
    QFETCH(bool, w);
    if (w) ::signal_slot_benchmark<ObjectW>();
    else ::signal_slot_benchmark<Object>();
}

void QObjectBenchmark::connect_disconnect_benchmark_data()
{
    QTest::addColumn<int>("type");
    QTest::addColumn<bool>("w");

    QTest::newRow("normalized signature") << 0          << false;
    QTest::newRow("unormalized signature") << 1         << false;
    QTest::newRow("function pointer") << 2              << false;
    QTest::newRow("normalized signature/handle") << 3   << false;
    QTest::newRow("unormalized signature/handle") << 4  << false;
    QTest::newRow("function pointer/handle") << 5       << false;
    QTest::newRow("functor/handle") << 6                << false;

    QTest::newRow("w normalized signature") << 0          << true;
    QTest::newRow("w unormalized signature") << 1         << true;
    QTest::newRow("w function pointer") << 2              << true;
    QTest::newRow("w normalized signature/handle") << 3   << true;
    QTest::newRow("w unormalized signature/handle") << 4  << true;
    QTest::newRow("w function pointer/handle") << 5       << true;
    QTest::newRow("w functor/handle") << 6                << true;

}

template<typename Object>
void connect_disconnect_benchmark()
{
    QFETCH(int, type);
    switch (type) {
        case 0: {
            Object obj;
            QBENCHMARK {
                QObject::connect   (&obj, SIGNAL(signal5()), &obj, SLOT(slot5()));
                QObject::disconnect(&obj, SIGNAL(signal5()), &obj, SLOT(slot5()));
            }
        } break;
        case 1: {
            Object obj;
            QBENCHMARK {
                QObject::connect   (&obj, SIGNAL(signal5(  )), &obj, SLOT(slot5(  ))); // sic: non-normalised
                QObject::disconnect(&obj, SIGNAL(signal5(  )), &obj, SLOT(slot5(  ))); // sic: non-normalised
            }
        } break;
        case 2: {
            Object obj;
            QBENCHMARK {
                QObject::connect   (&obj, &Object::signal5, &obj, &Object::slot5);
                QObject::disconnect(&obj, &Object::signal5, &obj, &Object::slot5);
            }
        } break;
        case 3: {
            Object obj;
            QBENCHMARK {
                QObject::disconnect(QObject::connect(&obj, SIGNAL(signal5()), &obj, SLOT(slot5())));
            }
        } break;
        case 4: {
            Object obj;
            QBENCHMARK {
                QObject::disconnect(QObject::connect(&obj, SIGNAL(signal5(  )), &obj, SLOT(slot5(  )))); // sic: non-normalised
            }
        } break;
        case 5: {
            Object obj;
            QBENCHMARK {
                QObject::disconnect(QObject::connect(&obj, &Object::signal5, &obj, &Object::slot5));
            }
        } break;
        case 6: {
            Object obj;
            Functor functor;
            QBENCHMARK {
                QObject::disconnect(QObject::connect(&obj, &Object::signal5, functor));
            }
        } break;
    }
}

void QObjectBenchmark::connect_disconnect_benchmark()
{
    QFETCH(bool, w);
    if (w) ::connect_disconnect_benchmark<ObjectW>();
    else ::connect_disconnect_benchmark<Object>();
}

QTEST_MAIN(QObjectBenchmark)

#include "main.moc"