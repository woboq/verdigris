/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Copyright (C) 2015 Olivier Goffart <ogoffart@woboq.com>
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

#include <QtTest/QtTest>

#include <qcoreapplication.h>
#include <qpointer.h>
#include <qtimer.h>
#include <qregularexpression.h>
#include <qmetaobject.h>
#include <qvariant.h>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QScopedPointer>
#if QT_CONFIG(process)
# include <QProcess>
#endif
#include "qobject.h"
#ifdef QT_BUILD_INTERNAL
#undef QT_BUILD_INTERNAL  // Verdigris don't test the internals
#endif

#include <functional>

#include <math.h>

#include <wobjectimpl.h>

QT_WARNING_DISABLE_GCC("-Wdeprecated-declarations")
QT_WARNING_DISABLE_CLANG("-Wdeprecated-declarations")
QT_WARNING_DISABLE_GCC("-Wdeprecated-copy")


W_REGISTER_ARGTYPE(int&)
W_REGISTER_ARGTYPE(short&)
W_REGISTER_ARGTYPE(bool&)
W_REGISTER_ARGTYPE(QObject)
W_REGISTER_ARGTYPE(QObject&)

class tst_QObject : public QObject
{
    W_OBJECT(tst_QObject)
private slots:
    void disconnect(); W_SLOT(disconnect, W_Access::Private)
    void connectSlotsByName(); W_SLOT(connectSlotsByName, W_Access::Private)
    void connectSignalsToSignalsWithDefaultArguments(); W_SLOT(connectSignalsToSignalsWithDefaultArguments, W_Access::Private)
    void receivers(); W_SLOT(receivers, W_Access::Private)
    void normalize(); W_SLOT(normalize, W_Access::Private)
    void qobject_castTemplate(); W_SLOT(qobject_castTemplate, W_Access::Private)
    void findChildren(); W_SLOT(findChildren, W_Access::Private)
    void connectDisconnectNotify_data(); W_SLOT(connectDisconnectNotify_data, W_Access::Private)
    void connectDisconnectNotify(); W_SLOT(connectDisconnectNotify, W_Access::Private)
    void connectDisconnectNotifyPMF(); W_SLOT(connectDisconnectNotifyPMF, W_Access::Private)
    void disconnectNotify_receiverDestroyed(); W_SLOT(disconnectNotify_receiverDestroyed, W_Access::Private)
    void disconnectNotify_metaObjConnection(); W_SLOT(disconnectNotify_metaObjConnection, W_Access::Private)
    void connectNotify_connectSlotsByName(); W_SLOT(connectNotify_connectSlotsByName, W_Access::Private)
    void connectDisconnectNotify_shadowing(); W_SLOT(connectDisconnectNotify_shadowing, W_Access::Private)
    void connectReferenceToIncompleteTypes(); W_SLOT(connectReferenceToIncompleteTypes, W_Access::Private)
    void emitInDefinedOrder(); W_SLOT(emitInDefinedOrder, W_Access::Private)
    void customTypes(); W_SLOT(customTypes, W_Access::Private)
    void streamCustomTypes(); W_SLOT(streamCustomTypes, W_Access::Private)
    void metamethod(); W_SLOT(metamethod, W_Access::Private)
    void namespaces(); W_SLOT(namespaces, W_Access::Private)
    void threadSignalEmissionCrash(); W_SLOT(threadSignalEmissionCrash, W_Access::Private)
    void thread(); W_SLOT(thread, W_Access::Private)
    void thread0(); W_SLOT(thread0, W_Access::Private)
    void moveToThread(); W_SLOT(moveToThread, W_Access::Private)
    void senderTest(); W_SLOT(senderTest, W_Access::Private)
    void declareInterface(); W_SLOT(declareInterface, W_Access::Private)
    void qpointerResetBeforeDestroyedSignal(); W_SLOT(qpointerResetBeforeDestroyedSignal, W_Access::Private)
    void childDeletesItsSibling(); W_SLOT(childDeletesItsSibling, W_Access::Private)
    void dynamicProperties(); W_SLOT(dynamicProperties, W_Access::Private)
    void floatProperty(); W_SLOT(floatProperty, W_Access::Private)
    void qrealProperty(); W_SLOT(qrealProperty, W_Access::Private)
    void property(); W_SLOT(property, W_Access::Private)
    void recursiveSignalEmission(); W_SLOT(recursiveSignalEmission, W_Access::Private)
    void signalBlocking(); W_SLOT(signalBlocking, W_Access::Private)
    void blockingQueuedConnection(); W_SLOT(blockingQueuedConnection, W_Access::Private)
    void childEvents(); W_SLOT(childEvents, W_Access::Private)
    void installEventFilter(); W_SLOT(installEventFilter, W_Access::Private)
    void deleteSelfInSlot(); W_SLOT(deleteSelfInSlot, W_Access::Private)
    void disconnectSelfInSlotAndDeleteAfterEmit(); W_SLOT(disconnectSelfInSlotAndDeleteAfterEmit, W_Access::Private)
    void dumpObjectInfo(); W_SLOT(dumpObjectInfo, W_Access::Private)
    void connectToSender(); W_SLOT(connectToSender, W_Access::Private)
    void qobjectConstCast(); W_SLOT(qobjectConstCast, W_Access::Private)
    void uniqConnection(); W_SLOT(uniqConnection, W_Access::Private)
    void uniqConnectionPtr(); W_SLOT(uniqConnectionPtr, W_Access::Private)
    void interfaceIid(); W_SLOT(interfaceIid, W_Access::Private)
    void deleteQObjectWhenDeletingEvent(); W_SLOT(deleteQObjectWhenDeletingEvent, W_Access::Private)
    void overloads(); W_SLOT(overloads, W_Access::Private)
    void isSignalConnected(); W_SLOT(isSignalConnected, W_Access::Private)
    void isSignalConnectedAfterDisconnection(); W_SLOT(isSignalConnectedAfterDisconnection, W_Access::Private)
    void qMetaObjectConnect(); W_SLOT(qMetaObjectConnect, W_Access::Private)
    void qMetaObjectDisconnectOne(); W_SLOT(qMetaObjectDisconnectOne, W_Access::Private)
    void sameName(); W_SLOT(sameName, W_Access::Private)
    void connectByMetaMethods(); W_SLOT(connectByMetaMethods, W_Access::Private)
    void connectByMetaMethodSlotInsteadOfSignal(); W_SLOT(connectByMetaMethodSlotInsteadOfSignal, W_Access::Private)
    void connectConstructorByMetaMethod(); W_SLOT(connectConstructorByMetaMethod, W_Access::Private)
    void disconnectByMetaMethod(); W_SLOT(disconnectByMetaMethod, W_Access::Private)
    void disconnectNotSignalMetaMethod(); W_SLOT(disconnectNotSignalMetaMethod, W_Access::Private)
    void autoConnectionBehavior(); W_SLOT(autoConnectionBehavior, W_Access::Private)
    void baseDestroyed(); W_SLOT(baseDestroyed, W_Access::Private)
    void pointerConnect(); W_SLOT(pointerConnect, W_Access::Private)
    void pointerDisconnect(); W_SLOT(pointerDisconnect, W_Access::Private)
    void emitInDefinedOrderPointer(); W_SLOT(emitInDefinedOrderPointer, W_Access::Private)
    void customTypesPointer(); W_SLOT(customTypesPointer, W_Access::Private)
    void connectCxx0x(); W_SLOT(connectCxx0x, W_Access::Private)
    void connectToStaticCxx0x(); W_SLOT(connectToStaticCxx0x, W_Access::Private)
    void connectCxx0xTypeMatching(); W_SLOT(connectCxx0xTypeMatching, W_Access::Private)
    void connectCxx17Noexcept(); W_SLOT(connectCxx17Noexcept, W_Access::Private)
    void connectConvert(); W_SLOT(connectConvert, W_Access::Private)
    void connectWithReference(); W_SLOT(connectWithReference, W_Access::Private)
    void connectManyArguments(); W_SLOT(connectManyArguments, W_Access::Private)
    void connectForwardDeclare(); W_SLOT(connectForwardDeclare, W_Access::Private)
    void connectNoDefaultConstructorArg(); W_SLOT(connectNoDefaultConstructorArg, W_Access::Private)
    void returnValue_data(); W_SLOT(returnValue_data, W_Access::Private)
    void returnValue(); W_SLOT(returnValue, W_Access::Private)
    void returnValue2_data(); W_SLOT(returnValue2_data, W_Access::Private)
    void returnValue2(); W_SLOT(returnValue2, W_Access::Private)
    void connectVirtualSlots(); W_SLOT(connectVirtualSlots, W_Access::Private)
    void connectSlotsVMIClass(); W_SLOT(connectSlotsVMIClass, W_Access::Private)  // VMI = Virtual or Multiple Inheritance
    void connectPrivateSlots(); W_SLOT(connectPrivateSlots, W_Access::Private)
    void connectFunctorArgDifference(); W_SLOT(connectFunctorArgDifference, W_Access::Private)
    void connectFunctorOverloads(); W_SLOT(connectFunctorOverloads, W_Access::Private)
    void connectFunctorQueued(); W_SLOT(connectFunctorQueued, W_Access::Private)
    void connectFunctorWithContext(); W_SLOT(connectFunctorWithContext, W_Access::Private)
    void connectFunctorWithContextUnique(); W_SLOT(connectFunctorWithContextUnique, W_Access::Private)
    void connectFunctorDeadlock(); W_SLOT(connectFunctorDeadlock, W_Access::Private)
    void connectFunctorMoveOnly(); W_SLOT(connectFunctorMoveOnly, W_Access::Private)
    void connectStaticSlotWithObject(); W_SLOT(connectStaticSlotWithObject, W_Access::Private)
    void disconnectDoesNotLeakFunctor(); W_SLOT(disconnectDoesNotLeakFunctor, W_Access::Private)
    void contextDoesNotLeakFunctor(); W_SLOT(contextDoesNotLeakFunctor, W_Access::Private)
    void connectBase(); W_SLOT(connectBase, W_Access::Private)
    void connectWarnings(); W_SLOT(connectWarnings, W_Access::Private)
    void qmlConnect(); W_SLOT(qmlConnect, W_Access::Private)
    void exceptions(); W_SLOT(exceptions, W_Access::Private)
    void noDeclarativeParentChangedOnDestruction(); W_SLOT(noDeclarativeParentChangedOnDestruction, W_Access::Private)
    void deleteLaterInAboutToBlockHandler(); W_SLOT(deleteLaterInAboutToBlockHandler, W_Access::Private)
    void mutableFunctor(); W_SLOT(mutableFunctor, W_Access::Private)
    void checkArgumentsForNarrowing(); W_SLOT(checkArgumentsForNarrowing, W_Access::Private)
    void nullReceiver(); W_SLOT(nullReceiver, W_Access::Private)
    void functorReferencesConnection(); W_SLOT(functorReferencesConnection, W_Access::Private)
    void disconnectDisconnects(); W_SLOT(disconnectDisconnects, W_Access::Private)
    void singleShotConnection(); W_SLOT(singleShotConnection, W_Access::Private)
};

struct QObjectCreatedOnShutdown
{
    QObjectCreatedOnShutdown() {}
    ~QObjectCreatedOnShutdown()
    {
        QObject();
    }
};
static QObjectCreatedOnShutdown s_qobjectCreatedOnShutdown;

class SenderObject : public QObject
{
    W_OBJECT(SenderObject)

public:
    SenderObject() : aPublicSlotCalled(0), recursionCount(0) {}

    void emitSignal1AfterRecursion()
    {
        if (recursionCount++ < 100)
            emitSignal1AfterRecursion();
        else
            emitSignal1();
    }

    void emitSignal1() { emit signal1(); }
    void emitSignal2() { emit signal2(); }
    void emitSignal3() { emit signal3(); }
    void emitSignal4() { emit signal4(); }

signals:
    void signal1() W_SIGNAL(signal1)
    void signal2() W_SIGNAL(signal2)
    void signal3() W_SIGNAL(signal3)
    void signal4() W_SIGNAL(signal4)
    void signal5() W_SIGNAL_COMPAT(signal5)
    void signal6(void) W_SIGNAL(signal6)
    void signal7(int a, const QString &b) W_SIGNAL(signal7,a,b)

public slots:
    void aPublicSlot() { aPublicSlotCalled++; }
    W_SLOT(aPublicSlot)

public:
    void invoke1(){} W_INVOKABLE(invoke1)
    void sinvoke1(){} W_INVOKABLE(sinvoke1, W_Scriptable)
    int aPublicSlotCalled;
protected:
    void invoke2(){} W_INVOKABLE(invoke2,(), W_Compat, W_Access::Protected)
    void invoke2(int){} W_INVOKABLE(invoke2,(int), W_Compat, W_Access::Protected)
    void sinvoke2(){} W_INVOKABLE(sinvoke2, W_Compat, W_Scriptable, W_Access::Protected)
private:
    void invoke3(int hinz = 0, int kunz = 0)
    {
        Q_UNUSED(hinz)
        Q_UNUSED(kunz)
    } W_INVOKABLE(invoke3, W_Access::Private)
    void sinvoke3(){} W_INVOKABLE(sinvoke3, W_Scriptable, W_Access::Private)

    int recursionCount;
};

class ReceiverObject : public QObject
{
    W_OBJECT(ReceiverObject)

public:
    ReceiverObject()
        : sequence_slot1( 0 )
        , sequence_slot2( 0 )
        , sequence_slot3( 0 )
        , sequence_slot4( 0 )
    {}

    void reset()
    {
        sequence_slot4 = 0;
        sequence_slot3 = 0;
        sequence_slot2 = 0;
        sequence_slot1 = 0;
        count_slot1 = 0;
        count_slot2 = 0;
        count_slot3 = 0;
        count_slot4 = 0;
    }

    int sequence_slot1;
    int sequence_slot2;
    int sequence_slot3;
    int sequence_slot4;
    int count_slot1;
    int count_slot2;
    int count_slot3;
    int count_slot4;

    bool called(int slot)
    {
        switch (slot) {
        case 1: return sequence_slot1;
        case 2: return sequence_slot2;
        case 3: return sequence_slot3;
        case 4: return sequence_slot4;
        default: return false;
        }
    }

    static int sequence;

public slots:
    void slot1() { sequence_slot1 = ++sequence; count_slot1++; }
    W_SLOT(slot1)
    void slot2() { sequence_slot2 = ++sequence; count_slot2++; }
    W_SLOT(slot2)
    void slot3() { sequence_slot3 = ++sequence; count_slot3++; }
    W_SLOT(slot3)
    void slot4() { sequence_slot4 = ++sequence; count_slot4++; }
    W_SLOT(slot4)

};

int ReceiverObject::sequence = 0;

static void playWithObjects()
{
    // Do operations that will lock the internal signalSlotLock mutex on many QObjects.
    // The more QObjects, the higher the chance that the signalSlotLock mutex used
    // is already in use. If the number of objects is higher than the number of mutexes in
    // the pool (currently 131), the deadlock should always trigger. Use an even higher number
    // to be on the safe side.
    const int objectCount = 1024;
    SenderObject lotsOfObjects[objectCount];
    for (int i = 0; i < objectCount; ++i) {
        QObject::connect(&lotsOfObjects[i], &SenderObject::signal1,
                         &lotsOfObjects[i], &SenderObject::aPublicSlot);
    }
}

void tst_QObject::disconnect()
{
    SenderObject s;
    ReceiverObject r1;
    ReceiverObject r2;

    connect(&s, SIGNAL(signal1()), &r1, SLOT(slot1()));

    connect(&s, SIGNAL(signal2()), &r1, SLOT(slot2()));
    connect(&s, SIGNAL(signal3()), &r1, SLOT(slot3()));
    connect(&s, SIGNAL(signal4()), &r1, SLOT(slot4()));

    s.emitSignal1();
    s.emitSignal2();
    s.emitSignal3();
    s.emitSignal4();

    QVERIFY(r1.called(1));
    QVERIFY(r1.called(2));
    QVERIFY(r1.called(3));
    QVERIFY(r1.called(4));
    r1.reset();

    // usual disconnect with all parameters given
    bool ret = QObject::disconnect(&s, SIGNAL(signal1()), &r1, SLOT(slot1()));

    s.emitSignal1();

    QVERIFY(!r1.called(1));
    r1.reset();

    QVERIFY(ret);
    ret = QObject::disconnect(&s, SIGNAL(signal1()), &r1, SLOT(slot1()));
    QVERIFY(!ret);

    // disconnect all signals from s from all slots from r1
    QObject::disconnect(&s, 0, &r1, 0);

    s.emitSignal2();
    s.emitSignal3();
    s.emitSignal4();

    QVERIFY(!r1.called(2));
    QVERIFY(!r1.called(3));
    QVERIFY(!r1.called(4));
    r1.reset();

    connect(&s, SIGNAL(signal1()), &r1, SLOT(slot1()));
    connect(&s, SIGNAL(signal1()), &r1, SLOT(slot2()));
    connect(&s, SIGNAL(signal1()), &r1, SLOT(slot3()));
    connect(&s, SIGNAL(signal2()), &r1, SLOT(slot4()));

    // disconnect s's signal1() from all slots of r1
    QObject::disconnect(&s, SIGNAL(signal1()), &r1, 0);

    s.emitSignal1();
    s.emitSignal2();

    QVERIFY(!r1.called(1));
    QVERIFY(!r1.called(2));
    QVERIFY(!r1.called(3));
    QVERIFY(r1.called(4));
    r1.reset();
    // make sure all is disconnected again
    QObject::disconnect(&s, 0, &r1, 0);

    connect(&s, SIGNAL(signal1()), &r1, SLOT(slot1()));
    connect(&s, SIGNAL(signal1()), &r2, SLOT(slot1()));
    connect(&s, SIGNAL(signal2()), &r1, SLOT(slot2()));
    connect(&s, SIGNAL(signal2()), &r2, SLOT(slot2()));
    connect(&s, SIGNAL(signal3()), &r1, SLOT(slot3()));
    connect(&s, SIGNAL(signal3()), &r2, SLOT(slot3()));

    // disconnect signal1() from all receivers
    QObject::disconnect(&s, SIGNAL(signal1()), 0, 0);
    s.emitSignal1();
    s.emitSignal2();
    s.emitSignal3();

    QVERIFY(!r1.called(1));
    QVERIFY(!r2.called(1));
    QVERIFY(r1.called(2));
    QVERIFY(r2.called(2));
    QVERIFY(r1.called(2));
    QVERIFY(r2.called(2));

    r1.reset();
    r2.reset();

    // disconnect all signals of s from all receivers
    QObject::disconnect(&s, 0, 0, 0);

    QVERIFY(!r1.called(2));
    QVERIFY(!r2.called(2));
    QVERIFY(!r1.called(2));
    QVERIFY(!r2.called(2));
}

class AutoConnectSender : public QObject
{
    W_OBJECT(AutoConnectSender)

public:
    AutoConnectSender(QObject *parent)
        : QObject(parent)
    {}

    void emitSignalNoParams() { emit signalNoParams(); }
    void emitSignalWithParams(int i) { emit signalWithParams(i); }
    void emitSignalWithParams(int i, QString string) { emit signalWithParams(i, string); }
    void emitSignalManyParams(int i1, int i2, int i3, QString string, bool onoff) { emit signalManyParams(i1, i2, i3, string, onoff); }
    void emitSignalManyParams(int i1, int i2, int i3, QString string, bool onoff, bool dummy) { emit signalManyParams(i1, i2, i3, string, onoff, dummy); }
    void emitSignalManyParams2(int i1, int i2, int i3, QString string, bool onoff) { emit signalManyParams2(i1, i2, i3, string, onoff); }
    void emitSignalLoopBack() { emit signalLoopBack(); }

signals:
    void signalNoParams() W_SIGNAL(signalNoParams)
    void signalWithParams(int i) W_SIGNAL(signalWithParams,(int),i)
    void signalWithParams(int i, QString string) W_SIGNAL(signalWithParams,(int,QString),i, string)
    void signalManyParams(int i1, int i2, int i3, QString string, bool onoff)
        W_SIGNAL(signalManyParams,(int,int,int,QString,bool),i1,i2,i3,string,onoff)
    void signalManyParams(int i1, int i2, int i3, QString string, bool onoff, bool _)
        W_SIGNAL(signalManyParams,(int,int,int,QString,bool,bool),i1,i2,i3,string,onoff,_)
    void signalManyParams2(int i1, int i2, int i3, QString string, bool onoff)
        W_SIGNAL(signalManyParams2, i1,i2,i3,string,onoff)
    void signalLoopBack() W_SIGNAL(signalLoopBack)
};

class AutoConnectReceiver : public QObject
{
    W_OBJECT(AutoConnectReceiver)

public:
    QList<int> called_slots;

    AutoConnectReceiver()
    {
        connect(this, SIGNAL(on_Sender_signalLoopBack()), this, SLOT(slotLoopBack()));
    }

    void emitSignalNoParams() { emit signalNoParams(); }
    void emit_signal_with_underscore() { emit signal_with_underscore(); }

public slots:
    void on_Sender_signalNoParams() { called_slots << 1; }
    W_SLOT(on_Sender_signalNoParams)
    void on_Sender_signalWithParams(int = 0) { called_slots << 2; }
    W_SLOT(on_Sender_signalWithParams,(int))
    void on_Sender_signalWithParams(int, QString) { called_slots << 3; }
    W_SLOT(on_Sender_signalWithParams,(int, QString))
    void on_Sender_signalManyParams() { called_slots << 4; }
    W_SLOT(on_Sender_signalManyParams,())
    void on_Sender_signalManyParams(int, int, int, QString, bool) { called_slots << 5; }
    W_SLOT(on_Sender_signalManyParams,(int,int,int,QString,bool))
    void on_Sender_signalManyParams(int, int, int, QString, bool, bool) { called_slots << 6;}
    W_SLOT(on_Sender_signalManyParams,(int,int,int,QString,bool,bool))
    void on_Sender_signalManyParams2(int, int, int, QString, bool) { called_slots << 7; }
    W_SLOT(on_Sender_signalManyParams2)
    void slotLoopBack() { called_slots << 8; }
    W_SLOT(slotLoopBack)
    void on_Receiver_signalNoParams() { called_slots << 9; }
    W_SLOT(on_Receiver_signalNoParams)
    void on_Receiver_signal_with_underscore() { called_slots << 10; }
    W_SLOT(on_Receiver_signal_with_underscore)

protected slots:
    void o() { called_slots << -1; }
    W_SLOT(o)
    void on() { called_slots << -1; }
    W_SLOT(on)
    void on_() { called_slots << -1; }
    W_SLOT(on_)
    void on_something() { called_slots << -1; }
    W_SLOT(on_something)
    void on_child_signal() { called_slots << -1; }
    W_SLOT(on_child_signal)

signals:
    void on_Sender_signalLoopBack() W_SIGNAL(on_Sender_signalLoopBack)
    void signalNoParams() W_SIGNAL(signalNoParams)
    void signal_with_underscore() W_SIGNAL(signal_with_underscore)
};

void tst_QObject::connectSlotsByName()
{
    AutoConnectReceiver receiver;
    receiver.setObjectName("Receiver");
    AutoConnectSender sender(&receiver);
    sender.setObjectName("Sender");

    QTest::ignoreMessage(QtWarningMsg, "QMetaObject::connectSlotsByName: No matching signal for on_child_signal()");
    QTest::ignoreMessage(QtWarningMsg, "QMetaObject::connectSlotsByName: Connecting slot on_Sender_signalManyParams() with the first of the following compatible signals: QList(\"signalManyParams(int,int,int,QString,bool)\", \"signalManyParams(int,int,int,QString,bool,bool)\")");
    QMetaObject::connectSlotsByName(&receiver);

    receiver.called_slots.clear();
    sender.emitSignalNoParams();
    QCOMPARE(receiver.called_slots, QList<int>() << 1);

    receiver.called_slots.clear();
    sender.emitSignalWithParams(0);
    QCOMPARE(receiver.called_slots, QList<int>() << 2);

    receiver.called_slots.clear();
    sender.emitSignalWithParams(0, "string");
    QCOMPARE(receiver.called_slots, QList<int>() << 3);

    receiver.called_slots.clear();
    sender.emitSignalManyParams(1, 2, 3, "string", true);
    sender.emitSignalManyParams(1, 2, 3, "string", true, false);
    // the slot '4' (signalManyParams()) will get connected
    // to either of the two signalManyParams(...) overloads
    QCOMPARE(receiver.called_slots, QList<int>() << 4 << 5 << 6);

    receiver.called_slots.clear();
    sender.emitSignalManyParams2(1, 2, 3, "string", true);
    QCOMPARE(receiver.called_slots, QList<int>() << 7);

    receiver.called_slots.clear();
    sender.emitSignalLoopBack();
    QCOMPARE(receiver.called_slots, QList<int>() << 8);

    receiver.called_slots.clear();
    receiver.emitSignalNoParams();
    QCOMPARE(receiver.called_slots, QList<int>() << 9);

    receiver.called_slots.clear();
    receiver.emit_signal_with_underscore();
    QCOMPARE(receiver.called_slots, QList<int>() << 10);
}

void tst_QObject::qobject_castTemplate()
{
    QScopedPointer<QObject> o;
    QVERIFY(!::qobject_cast<QObject*>(o.data()));

    o.reset(new SenderObject);
    QVERIFY(::qobject_cast<SenderObject*>(o.data()));
    QVERIFY(::qobject_cast<QObject*>(o.data()));
    QVERIFY(!::qobject_cast<ReceiverObject*>(o.data()));
}

void tst_QObject::findChildren()
{
    QObject o;
    QObject o1(&o);
    QObject o2(&o);
    QObject o11(&o1);
    QObject o12(&o1);
    QObject o111(&o11);
    QObject unnamed(&o);
    QTimer t1(&o);
    QTimer t121(&o12);
    QTimer emptyname(&o);

    Q_SET_OBJECT_NAME(o);
    Q_SET_OBJECT_NAME(o1);
    Q_SET_OBJECT_NAME(o2);
    Q_SET_OBJECT_NAME(o11);
    Q_SET_OBJECT_NAME(o12);
    Q_SET_OBJECT_NAME(o111);
    Q_SET_OBJECT_NAME(t1);
    Q_SET_OBJECT_NAME(t121);
    emptyname.setObjectName("");

    QObject *op = nullptr;

    op = o.findChild<QObject*>("o1");
    QCOMPARE(op, &o1);
    op = o.findChild<QObject*>("o2");
    QCOMPARE(op, &o2);
    op = o.findChild<QObject*>("o11");
    QCOMPARE(op, &o11);
    op = o.findChild<QObject*>("o12");
    QCOMPARE(op, &o12);
    op = o.findChild<QObject*>("o111");
    QCOMPARE(op, &o111);
    op = o.findChild<QObject*>("t1");
    QCOMPARE(op, static_cast<QObject *>(&t1));
    op = o.findChild<QObject*>("t121");
    QCOMPARE(op, static_cast<QObject *>(&t121));
    op = o.findChild<QTimer*>("t1");
    QCOMPARE(op, static_cast<QObject *>(&t1));
    op = o.findChild<QTimer*>("t121");
    QCOMPARE(op, static_cast<QObject *>(&t121));
    op = o.findChild<QTimer*>("o12");
    QCOMPARE(op, static_cast<QObject *>(0));
    op = o.findChild<QObject*>("o");
    QCOMPARE(op, static_cast<QObject *>(0));
    op = o.findChild<QObject*>("harry");
    QCOMPARE(op, static_cast<QObject *>(0));
    op = o.findChild<QObject*>("o1");
    QCOMPARE(op, &o1);

    QList<QObject*> l;
    QList<QTimer*> tl;

    l = o.findChildren<QObject*>("o1");
    QCOMPARE(l.size(), 1);
    QCOMPARE(l.at(0), &o1);
    l = o.findChildren<QObject*>("o2");
    QCOMPARE(l.size(), 1);
    QCOMPARE(l.at(0), &o2);
    l = o.findChildren<QObject*>("o11");
    QCOMPARE(l.size(), 1);
    QCOMPARE(l.at(0), &o11);
    l = o.findChildren<QObject*>("o12");
    QCOMPARE(l.size(), 1);
    QCOMPARE(l.at(0), &o12);
    l = o.findChildren<QObject*>("o111");
    QCOMPARE(l.size(), 1);
    QCOMPARE(l.at(0), &o111);
    l = o.findChildren<QObject*>("t1");
    QCOMPARE(l.size(), 1);
    QCOMPARE(l.at(0), static_cast<QObject *>(&t1));
    l = o.findChildren<QObject*>("t121");
    QCOMPARE(l.size(), 1);
    QCOMPARE(l.at(0), static_cast<QObject *>(&t121));
    tl = o.findChildren<QTimer*>("t1");
    QCOMPARE(tl.size(), 1);
    QCOMPARE(tl.at(0), &t1);
    tl = o.findChildren<QTimer*>("t121");
    QCOMPARE(tl.size(), 1);
    QCOMPARE(tl.at(0), &t121);
    l = o.findChildren<QObject*>("o");
    QCOMPARE(l.size(), 0);
    l = o.findChildren<QObject*>("harry");
    QCOMPARE(l.size(), 0);
    tl = o.findChildren<QTimer*>("o12");
    QCOMPARE(tl.size(), 0);
    l = o.findChildren<QObject*>("o1");
    QCOMPARE(l.size(), 1);
    QCOMPARE(l.at(0), &o1);

    l = o.findChildren<QObject*>(QRegularExpression("^o.*$"));
    QCOMPARE(l.size(), 5);
    QVERIFY(l.contains(&o1));
    QVERIFY(l.contains(&o2));
    QVERIFY(l.contains(&o11));
    QVERIFY(l.contains(&o12));
    QVERIFY(l.contains(&o111));
    l = o.findChildren<QObject*>(QRegularExpression("t.*"));
    QCOMPARE(l.size(), 2);
    QVERIFY(l.contains(&t1));
    QVERIFY(l.contains(&t121));
    tl = o.findChildren<QTimer*>(QRegularExpression("^.*$"));
    QCOMPARE(tl.size(), 3);
    QVERIFY(tl.contains(&t1));
    QVERIFY(tl.contains(&t121));
    tl = o.findChildren<QTimer*>(QRegularExpression("^o.*$"));
    QCOMPARE(tl.size(), 0);
    l = o.findChildren<QObject*>(QRegularExpression("^harry$"));
    QCOMPARE(l.size(), 0);

    l = o.findChildren<QObject*>(QRegularExpression("o.*"));
    QCOMPARE(l.size(), 5);
    QVERIFY(l.contains(&o1));
    QVERIFY(l.contains(&o2));
    QVERIFY(l.contains(&o11));
    QVERIFY(l.contains(&o12));
    QVERIFY(l.contains(&o111));
    l = o.findChildren<QObject*>(QRegularExpression("t.*"));
    QCOMPARE(l.size(), 2);
    QVERIFY(l.contains(&t1));
    QVERIFY(l.contains(&t121));
    tl = o.findChildren<QTimer*>(QRegularExpression(".*"));
    QCOMPARE(tl.size(), 3);
    QVERIFY(tl.contains(&t1));
    QVERIFY(tl.contains(&t121));
    tl = o.findChildren<QTimer*>(QRegularExpression("o.*"));
    QCOMPARE(tl.size(), 0);
    l = o.findChildren<QObject*>(QRegularExpression("harry"));
    QCOMPARE(l.size(), 0);

    // empty and null string check
    op = o.findChild<QObject*>();
    QCOMPARE(op, &o1);
    op = o.findChild<QObject*>("");
    QCOMPARE(op, &unnamed);
    op = o.findChild<QObject*>("unnamed");
    QCOMPARE(op, static_cast<QObject *>(0));

    l = o.findChildren<QObject*>();
    QCOMPARE(l.size(), 9);
    l = o.findChildren<QObject*>("");
    QCOMPARE(l.size(), 2);
    l = o.findChildren<QObject*>("unnamed");
    QCOMPARE(l.size(), 0);

    tl = o.findChildren<QTimer *>("t1");
    QCOMPARE(tl.size(), 1);
    QCOMPARE(tl.at(0), &t1);

    // Find direct child/children

    op = o.findChild<QObject*>("o1", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, &o1);
    op = o.findChild<QObject*>("o2", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, &o2);
    op = o.findChild<QObject*>("o11", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, static_cast<QObject *>(0));
    op = o.findChild<QObject*>("o12", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, static_cast<QObject *>(0));
    op = o.findChild<QObject*>("o111", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, static_cast<QObject *>(0));
    op = o.findChild<QObject*>("t1", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, static_cast<QObject *>(&t1));
    op = o.findChild<QObject*>("t121", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, static_cast<QObject *>(0));
    op = o.findChild<QTimer*>("t1", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, static_cast<QObject *>(&t1));
    op = o.findChild<QTimer*>("t121", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, static_cast<QObject *>(0));
    op = o.findChild<QTimer*>("o12", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, static_cast<QObject *>(0));
    op = o.findChild<QObject*>("o", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, static_cast<QObject *>(0));
    op = o.findChild<QObject*>("harry", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, static_cast<QObject *>(0));
    op = o.findChild<QObject*>("o1", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, &o1);

    l = o.findChildren<QObject*>("o1", Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 1);
    QCOMPARE(l.at(0), &o1);
    l = o.findChildren<QObject*>("o2", Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 1);
    QCOMPARE(l.at(0), &o2);
    l = o.findChildren<QObject*>("o11", Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 0);
    l = o.findChildren<QObject*>("o12", Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 0);
    l = o.findChildren<QObject*>("o111", Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 0);
    l = o.findChildren<QObject*>("t1", Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 1);
    QCOMPARE(l.at(0), static_cast<QObject *>(&t1));
    l = o.findChildren<QObject*>("t121", Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 0);
    tl = o.findChildren<QTimer*>("t1", Qt::FindDirectChildrenOnly);
    QCOMPARE(tl.size(), 1);
    QCOMPARE(tl.at(0), &t1);
    tl = o.findChildren<QTimer*>("t121", Qt::FindDirectChildrenOnly);
    QCOMPARE(tl.size(), 0);
    l = o.findChildren<QObject*>("o", Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 0);
    l = o.findChildren<QObject*>("harry", Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 0);
    tl = o.findChildren<QTimer*>("o12", Qt::FindDirectChildrenOnly);
    QCOMPARE(tl.size(), 0);
    l = o.findChildren<QObject*>("o1", Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 1);
    QCOMPARE(l.at(0), &o1);

    l = o.findChildren<QObject*>(QRegularExpression("^o.*$"), Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 2);
    QVERIFY(l.contains(&o1));
    QVERIFY(l.contains(&o2));
    l = o.findChildren<QObject*>(QRegularExpression("^t.*$"), Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 1);
    QVERIFY(l.contains(&t1));
    tl = o.findChildren<QTimer*>(QRegularExpression("^.*$"), Qt::FindDirectChildrenOnly);
    QCOMPARE(tl.size(), 2);
    QVERIFY(tl.contains(&t1));
    tl = o.findChildren<QTimer*>(QRegularExpression("^o.*$"), Qt::FindDirectChildrenOnly);
    QCOMPARE(tl.size(), 0);
    l = o.findChildren<QObject*>(QRegularExpression("^harry$"), Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 0);

    // empty and null string check
    op = o.findChild<QObject*>(QString(), Qt::FindDirectChildrenOnly);
    QCOMPARE(op, &o1);
    op = o.findChild<QObject*>("", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, &unnamed);
    op = o.findChild<QObject*>("unnamed", Qt::FindDirectChildrenOnly);
    QCOMPARE(op, static_cast<QObject *>(0));

    l = o.findChildren<QObject*>(QString(), Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 5);
    l = o.findChildren<QObject*>("", Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 2);
    l = o.findChildren<QObject*>("unnamed", Qt::FindDirectChildrenOnly);
    QCOMPARE(l.size(), 0);

    tl = o.findChildren<QTimer *>("t1", Qt::FindDirectChildrenOnly);
    QCOMPARE(tl.size(), 1);
    QCOMPARE(tl.at(0), &t1);
}


class NotifyObject : public SenderObject, public ReceiverObject
{
public:
    NotifyObject() : SenderObject(), ReceiverObject()
    {}

    QList<QMetaMethod> connectedSignals;
    QList<QMetaMethod> disconnectedSignals;
    void clearNotifications()
    {
        connectedSignals.clear();
        disconnectedSignals.clear();
    }
protected:
    void connectNotify(const QMetaMethod &signal) override
    { connectedSignals.append(signal); }
    void disconnectNotify(const QMetaMethod &signal) override
    { disconnectedSignals.append(signal); }
};

void tst_QObject::connectDisconnectNotify_data()
{
    QTest::addColumn<QString>("a_signal");
    QTest::addColumn<QString>("a_slot");

    QTest::newRow("combo1") << SIGNAL( signal1() )        << SLOT( slot1() );
    QTest::newRow("combo2") << SIGNAL( signal2(void) )    << SLOT( slot2(  ) );
    QTest::newRow("combo3") << SIGNAL( signal3(  ) )      << SLOT( slot3(void) );
    QTest::newRow("combo4") << SIGNAL(  signal4( void )  )<< SLOT(  slot4( void )  );
    QTest::newRow("combo5") << SIGNAL( signal6( void ) )  << SLOT( slot4() );
    QTest::newRow("combo6") << SIGNAL( signal6() )        << SLOT( slot4() );
    QTest::newRow("combo7") << SIGNAL( signal7( int , const QString & ) ) << SLOT( slot4() );
}

void tst_QObject::connectDisconnectNotify()
{
    NotifyObject s;
    NotifyObject r;

    QFETCH(QString, a_signal);
    QFETCH(QString, a_slot);

    // Obtaining meta methods
    int signalIndx = ((SenderObject &)s).metaObject()->indexOfSignal(
            QMetaObject::normalizedSignature(a_signal.toLatin1().constData()+1).constData());
    int methodIndx = ((ReceiverObject &)r).metaObject()->indexOfMethod(
            QMetaObject::normalizedSignature(a_slot.toLatin1().constData()+1).constData());
    QMetaMethod signal = ((SenderObject &)s).metaObject()->method(signalIndx);
    QMetaMethod method = ((ReceiverObject &)r).metaObject()->method(methodIndx);
    QVERIFY(signal.isValid());
    QVERIFY(method.isValid());

    // Test connectNotify
    QVERIFY(QObject::connect((SenderObject *)&s, a_signal.toLatin1(),
                             (ReceiverObject *)&r, a_slot.toLatin1()));
    QCOMPARE(s.connectedSignals.size(), 1);
    QCOMPARE(s.connectedSignals.at(0), signal);
    QVERIFY(s.disconnectedSignals.isEmpty());

    // Test disconnectNotify
    QVERIFY(QObject::disconnect((SenderObject *)&s, a_signal.toLatin1(),
                                (ReceiverObject *)&r, a_slot.toLatin1()));
    QCOMPARE(s.disconnectedSignals.size(), 1);
    QCOMPARE(s.disconnectedSignals.at(0), signal);
    QCOMPARE(s.connectedSignals.size(), 1);

    // Reconnect
    s.clearNotifications();
    QVERIFY(QObject::connect((SenderObject *)&s, a_signal.toLatin1(),
                             (ReceiverObject *)&r, a_slot.toLatin1()));
    QCOMPARE(s.connectedSignals.size(), 1);
    QCOMPARE(s.connectedSignals.at(0), signal);
    QVERIFY(s.disconnectedSignals.isEmpty());

    // Test disconnectNotify for a complete disconnect
    QVERIFY(((SenderObject *)&s)->disconnect((ReceiverObject *)&r));
    QCOMPARE(s.disconnectedSignals.size(), 1);
    QCOMPARE(s.disconnectedSignals.at(0), QMetaMethod());
    QCOMPARE(s.connectedSignals.size(), 1);

    // Test connectNotify when connecting by QMetaMethod
    s.clearNotifications();
    QVERIFY(QObject::connect((SenderObject *)&s, signal, (ReceiverObject *)&r, method));
    QCOMPARE(s.connectedSignals.size(), 1);
    QCOMPARE(s.connectedSignals.at(0), signal);
    QVERIFY(s.disconnectedSignals.isEmpty());

    // Test disconnectNotify when disconnecting by QMetaMethod
    QVERIFY(QObject::disconnect((SenderObject *)&s, signal, (ReceiverObject *)&r, method));
    QCOMPARE(s.disconnectedSignals.size(), 1);
    QCOMPARE(s.disconnectedSignals.at(0), signal);
    QCOMPARE(s.connectedSignals.size(), 1);

    // Reconnect
    s.clearNotifications();
    QVERIFY(QObject::connect((SenderObject *)&s, a_signal.toLatin1(),
                             (ReceiverObject *)&r, a_slot.toLatin1()));

    // Test disconnectNotify for a complete disconnect by QMetaMethod
    QVERIFY(QObject::disconnect((SenderObject *)&s, QMetaMethod(), 0, QMetaMethod()));
    QCOMPARE(s.disconnectedSignals.size(), 1);
    QCOMPARE(s.disconnectedSignals.at(0), QMetaMethod());
    QCOMPARE(s.connectedSignals.size(), 1);

    // Test connectNotify when connecting by index
    s.clearNotifications();
    QVERIFY(QMetaObject::connect((SenderObject *)&s, signalIndx, (ReceiverObject *)&r, methodIndx));
    QCOMPARE(s.connectedSignals.size(), 1);
    QCOMPARE(s.connectedSignals.at(0), signal);
    QVERIFY(s.disconnectedSignals.isEmpty());

    // Test disconnectNotify when disconnecting by index
    QVERIFY(QMetaObject::disconnect((SenderObject *)&s, signalIndx,
                                    (ReceiverObject *)&r, methodIndx));
    QCOMPARE(s.disconnectedSignals.size(), 1);
    QCOMPARE(s.disconnectedSignals.at(0), signal);
    QCOMPARE(s.connectedSignals.size(), 1);
}

struct Incomplete;
W_REGISTER_ARGTYPE(Incomplete)

class QObjectWithIncomplete : public QObject {
    W_OBJECT(QObjectWithIncomplete)

public:
    QObjectWithIncomplete(QObject *parent=nullptr) : QObject(parent) {}
signals:
    void signalWithIncomplete(const Incomplete & x) W_SIGNAL(signalWithIncomplete, (const Incomplete &), x)
public slots:
    void slotWithIncomplete(const Incomplete &) {} W_SLOT(slotWithIncomplete,(const Incomplete &))
};

void tst_QObject::connectReferenceToIncompleteTypes() {
    QObjectWithIncomplete withIncomplete;
    auto connection = QObject::connect(&withIncomplete, &QObjectWithIncomplete::signalWithIncomplete,
                                       &withIncomplete, &QObjectWithIncomplete::slotWithIncomplete);
    QVERIFY(connection);
}


static void connectDisconnectNotifyTestSlot() {}

void tst_QObject::connectDisconnectNotifyPMF()
{
    NotifyObject s;
    NotifyObject r;

    QMetaMethod signal = QMetaMethod::fromSignal(&SenderObject::signal1);

    // Test connectNotify
    QVERIFY(QObject::connect((SenderObject *)&s, &SenderObject::signal1,
                             (ReceiverObject *)&r, &ReceiverObject::slot1));
    QCOMPARE(s.connectedSignals.size(), 1);
    QCOMPARE(s.connectedSignals.at(0), signal);
    QVERIFY(s.disconnectedSignals.isEmpty());

    // Test disconnectNotify
    QVERIFY(QObject::disconnect((SenderObject *)&s, &SenderObject::signal1,
                                (ReceiverObject *)&r, &ReceiverObject::slot1));
    QCOMPARE(s.disconnectedSignals.size(), 1);
    QCOMPARE(s.disconnectedSignals.at(0), signal);
    QCOMPARE(s.connectedSignals.size(), 1);

    // Reconnect
    s.clearNotifications();
    QVERIFY(QObject::connect((SenderObject *)&s, &SenderObject::signal1,
                             (ReceiverObject *)&r, &ReceiverObject::slot1));
    QCOMPARE(s.connectedSignals.size(), 1);
    QCOMPARE(s.connectedSignals.at(0), signal);
    QVERIFY(s.disconnectedSignals.isEmpty());

    // Test disconnectNotify with wildcard slot
    QVERIFY(QObject::disconnect((SenderObject *)&s, &SenderObject::signal1,
                                (ReceiverObject *)&r, 0));
    QCOMPARE(s.disconnectedSignals.size(), 1);
    QCOMPARE(s.disconnectedSignals.at(0), signal);
    QCOMPARE(s.connectedSignals.size(), 1);

    // Reconnect
    s.clearNotifications();
    QMetaObject::Connection conn = connect((SenderObject *)&s, &SenderObject::signal1,
                                           (ReceiverObject *)&r, &ReceiverObject::slot1);

    QVERIFY(conn);

    // Test disconnectNotify when disconnecting by QMetaObject::Connection
    QVERIFY(QObject::disconnect(conn));
    QVERIFY(!s.disconnectedSignals.isEmpty());

    // Test connectNotify when connecting by function pointer
    s.clearNotifications();
    QVERIFY(QObject::connect((SenderObject *)&s, &SenderObject::signal1,
                             connectDisconnectNotifyTestSlot));
    QCOMPARE(s.connectedSignals.size(), 1);
    QCOMPARE(s.connectedSignals.at(0), signal);
    QVERIFY(s.disconnectedSignals.isEmpty());
}

void tst_QObject::disconnectNotify_receiverDestroyed()
{
    NotifyObject s;

    {
        NotifyObject r;
        QVERIFY(QObject::connect((SenderObject *)&s, SIGNAL(signal1()),
                                 (ReceiverObject *)&r, SLOT(slot1())));
    }
    QCOMPARE(s.disconnectedSignals.count(), 1);
    QCOMPARE(s.disconnectedSignals.at(0), QMetaMethod::fromSignal(&SenderObject::signal1));

    s.disconnectedSignals.clear();

    {
        NotifyObject r;
        QVERIFY(QObject::connect((SenderObject *)&s, SIGNAL(signal3()),
                                 (ReceiverObject *)&r, SLOT(slot3())));
    }

    QCOMPARE(s.disconnectedSignals.count(), 1);
    QCOMPARE(s.disconnectedSignals.at(0), QMetaMethod::fromSignal(&SenderObject::signal3));

    s.disconnectedSignals.clear();

    {
        NotifyObject r;
        QVERIFY(QObject::connect((SenderObject *)&s, SIGNAL(destroyed()), (ReceiverObject *)&r, SLOT(slot3())));
    }

    QCOMPARE(s.disconnectedSignals.count(), 1);
    QCOMPARE(s.disconnectedSignals.at(0), QMetaMethod::fromSignal(&QObject::destroyed));
}

void tst_QObject::disconnectNotify_metaObjConnection()
{
    NotifyObject s;
    {
        NotifyObject r;

        QMetaObject::Connection c = QObject::connect((SenderObject *)&s, SIGNAL(signal1()),
                                                     (ReceiverObject *)&r, SLOT(slot1()));
        QVERIFY(c);
        QVERIFY(QObject::disconnect(c));

        QCOMPARE(s.disconnectedSignals.count(), 1);
        QCOMPARE(s.disconnectedSignals.at(0), QMetaMethod::fromSignal(&SenderObject::signal1));

        QCOMPARE(s.disconnectedSignals.count(), 1);
    }
}

class ConnectByNameNotifySenderObject : public QObject
{
    W_OBJECT(ConnectByNameNotifySenderObject)
public:
    QList<QMetaMethod> connectedSignals;
    QList<QMetaMethod> disconnectedSignals;
    void clearNotifications()
    {
        connectedSignals.clear();
        disconnectedSignals.clear();
    }
protected:
    void connectNotify(const QMetaMethod &signal) override
    { connectedSignals.append(signal); }
    void disconnectNotify(const QMetaMethod &signal) override
    { disconnectedSignals.append(signal); }
Q_SIGNALS:
    void signal1() W_SIGNAL(signal1)
};

class ConnectByNameNotifyReceiverObject : public QObject
{
    W_OBJECT(ConnectByNameNotifyReceiverObject)
    void createNotifyChild(const char *name)
    {
        QObject *o = new ConnectByNameNotifySenderObject;
        o->setParent(this);
        o->setObjectName(QString::fromLatin1(name));
    }
public:
    ConnectByNameNotifyReceiverObject()
    {
        createNotifyChild("foo");
        createNotifyChild("bar");
        createNotifyChild("baz");
    };

public Q_SLOTS:
    void on_foo_signal1() {}
    W_SLOT(on_foo_signal1)
    void on_bar_signal1() {}
    W_SLOT(on_bar_signal1)
    void on_baz_signal1() {}
    W_SLOT(on_baz_signal1)
};

void tst_QObject::connectNotify_connectSlotsByName()
{
    ConnectByNameNotifyReceiverObject testObject;
    const QList<ConnectByNameNotifySenderObject *> senders =
            testObject.findChildren<ConnectByNameNotifySenderObject *>();
    for (ConnectByNameNotifySenderObject *o : senders) {
        QVERIFY(o->connectedSignals.isEmpty());
        QVERIFY(o->disconnectedSignals.isEmpty());
    }

    QMetaObject::connectSlotsByName(&testObject);

    for (ConnectByNameNotifySenderObject *o : senders) {
        QCOMPARE(o->connectedSignals.size(), 1);
        QCOMPARE(o->connectedSignals.at(0), QMetaMethod::fromSignal(&ConnectByNameNotifySenderObject::signal1));
        QVERIFY(o->disconnectedSignals.isEmpty());
    }
}

class ConnectDisconnectNotifyShadowObject
        : public ConnectByNameNotifySenderObject
{
    W_OBJECT(ConnectDisconnectNotifyShadowObject)
public Q_SLOTS:
    void slot1() {}
    W_SLOT(slot1)
Q_SIGNALS:
    void signal1() W_SIGNAL(signal1)
};

void tst_QObject::connectDisconnectNotify_shadowing()
{
    ConnectDisconnectNotifyShadowObject s;
    // Obtain QMetaMethods
    QMetaMethod shadowedSignal1 = QMetaMethod::fromSignal(&ConnectByNameNotifySenderObject::signal1);
    QMetaMethod redefinedSignal1 = QMetaMethod::fromSignal(&ConnectDisconnectNotifyShadowObject::signal1);
    QVERIFY(shadowedSignal1 != redefinedSignal1);
    int slot1Index = s.metaObject()->indexOfSlot("slot1()");
    QVERIFY(slot1Index != -1);
    QMetaMethod slot1 = s.metaObject()->method(slot1Index);

    // Test connectNotify
#ifndef QT_NO_DEBUG
    const char *warning = "QMetaObject::indexOfSignal: signal signal1() from "
                          "ConnectByNameNotifySenderObject redefined in "
                          "ConnectDisconnectNotifyShadowObject";
    QTest::ignoreMessage(QtWarningMsg, warning);
#endif
    QVERIFY(QObject::connect(&s, SIGNAL(signal1()), &s, SLOT(slot1())));
    QCOMPARE(s.connectedSignals.size(), 1);
    QCOMPARE(s.connectedSignals.at(0), redefinedSignal1);
    QVERIFY(s.disconnectedSignals.isEmpty());

    // Test disconnectNotify
#ifndef QT_NO_DEBUG
    QTest::ignoreMessage(QtWarningMsg, warning);
#endif
    QVERIFY(QObject::disconnect(&s, SIGNAL(signal1()), &s, SLOT(slot1())));
    QCOMPARE(s.disconnectedSignals.size(), 1);
    QCOMPARE(s.disconnectedSignals.at(0), redefinedSignal1);
    QCOMPARE(s.connectedSignals.size(), 1);

    // Test connectNotify when connecting by shadowed QMetaMethod
    s.clearNotifications();
    QVERIFY(QObject::connect(&s, shadowedSignal1, &s, slot1));
    QCOMPARE(s.connectedSignals.size(), 1);
    QCOMPARE(s.connectedSignals.at(0), shadowedSignal1);
    QVERIFY(s.disconnectedSignals.isEmpty());

    // Test disconnectNotify when disconnecting by shadowed QMetaMethod
    QVERIFY(QObject::disconnect(&s, shadowedSignal1, &s, slot1));
    QCOMPARE(s.disconnectedSignals.size(), 1);
    QCOMPARE(s.disconnectedSignals.at(0), shadowedSignal1);
    QCOMPARE(s.connectedSignals.size(), 1);

    // Test connectNotify when connecting by redefined QMetaMethod
    s.clearNotifications();
    QVERIFY(QObject::connect(&s, redefinedSignal1, &s, slot1));
    QCOMPARE(s.connectedSignals.size(), 1);
    QCOMPARE(s.connectedSignals.at(0), redefinedSignal1);
    QVERIFY(s.disconnectedSignals.isEmpty());

    // Test disconnectNotify when disconnecting by redefined QMetaMethod
    QVERIFY(QObject::disconnect(&s, redefinedSignal1, &s, slot1));
    QCOMPARE(s.disconnectedSignals.size(), 1);
    QCOMPARE(s.disconnectedSignals.at(0), redefinedSignal1);
    QCOMPARE(s.connectedSignals.size(), 1);
}

class SequenceObject : public ReceiverObject
{
    W_OBJECT(SequenceObject)

public:
    QObject *next;
    SequenceObject() : next(0) { }

public slots:
    void slot1_disconnectThis()
    {
        slot1();
        disconnect(sender(), SIGNAL(signal1()), this, SLOT(slot1_disconnectThis()));
    }
    W_SLOT(slot1_disconnectThis)

    void slot2_reconnectThis()
    {
        slot2();

        const QObject *s = sender();
        disconnect(s, SIGNAL(signal1()), this, SLOT(slot2_reconnectThis()));
        connect(s, SIGNAL(signal1()), this, SLOT(slot2_reconnectThis()));
    }
    W_SLOT(slot2_reconnectThis)
    void slot1_disconnectNext()
    {
        slot1();
        disconnect(sender(), SIGNAL(signal1()), next, SLOT(slot1()));
    }
    W_SLOT(slot1_disconnectNext)
    void slot2_reconnectNext()
    {
        slot2();

        // modify the connection list in 'this'
        disconnect(sender(), SIGNAL(signal1()), next, SLOT(slot2()));
        connect(sender(), SIGNAL(signal1()), next, SLOT(slot2()));

        // modify the sender list in 'this'
        connect(next, SIGNAL(destroyed()), this, SLOT(deleteLater()));
        connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(deleteLater()));
        disconnect(next, SIGNAL(destroyed()), this, SLOT(deleteLater()));
        disconnect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(deleteLater()));
    }
    W_SLOT(slot2_reconnectNext)
    void slot1_deleteNext()
    {
        slot1();
        delete next;
    }
    W_SLOT(slot1_deleteNext)
    void slot2_deleteSender()
    {
        slot2();
        delete sender();
    }
    W_SLOT(slot2_deleteSender)
};

void tst_QObject::emitInDefinedOrder()
{
    SenderObject sender;
    ReceiverObject receiver1, receiver2, receiver3, receiver4;

    connect(&sender, SIGNAL(signal1()), &receiver1, SLOT(slot1()));
    connect(&sender, SIGNAL(signal1()), &receiver2, SLOT(slot1()));
    connect(&sender, SIGNAL(signal1()), &receiver3, SLOT(slot1()));
    connect(&sender, SIGNAL(signal1()), &receiver4, SLOT(slot1()));
    connect(&sender, SIGNAL(signal1()), &receiver1, SLOT(slot2()));
    connect(&sender, SIGNAL(signal1()), &receiver2, SLOT(slot2()));
    connect(&sender, SIGNAL(signal1()), &receiver3, SLOT(slot2()));
    connect(&sender, SIGNAL(signal1()), &receiver4, SLOT(slot2()));

    int sequence;
    ReceiverObject::sequence = sequence = 0;
    sender.emitSignal1();
    QCOMPARE(receiver1.sequence_slot1, ++sequence);
    QCOMPARE(receiver2.sequence_slot1, ++sequence);
    QCOMPARE(receiver3.sequence_slot1, ++sequence);
    QCOMPARE(receiver4.sequence_slot1, ++sequence);
    QCOMPARE(receiver1.sequence_slot2, ++sequence);
    QCOMPARE(receiver2.sequence_slot2, ++sequence);
    QCOMPARE(receiver3.sequence_slot2, ++sequence);
    QCOMPARE(receiver4.sequence_slot2, ++sequence);

    QObject::disconnect(&sender, SIGNAL(signal1()), &receiver2, SLOT(slot1()));
    connect(&sender, SIGNAL(signal1()), &receiver2, SLOT(slot1()));

    ReceiverObject::sequence = sequence =  0;
    sender.emitSignal1();
    QCOMPARE(receiver1.sequence_slot1, ++sequence);
    QCOMPARE(receiver3.sequence_slot1, ++sequence);
    QCOMPARE(receiver4.sequence_slot1, ++sequence);
    QCOMPARE(receiver1.sequence_slot2, ++sequence);
    QCOMPARE(receiver2.sequence_slot2, ++sequence);
    QCOMPARE(receiver3.sequence_slot2, ++sequence);
    QCOMPARE(receiver4.sequence_slot2, ++sequence);
    QCOMPARE(receiver2.sequence_slot1, ++sequence);

    QObject::disconnect(&sender, SIGNAL(signal1()), &receiver1, SLOT(slot1()));
    connect(&sender, SIGNAL(signal1()), &receiver1, SLOT(slot1()));

    ReceiverObject::sequence = sequence =  0;
    sender.emitSignal1();
    QCOMPARE(receiver3.sequence_slot1, ++sequence);
    QCOMPARE(receiver4.sequence_slot1, ++sequence);
    QCOMPARE(receiver1.sequence_slot2, ++sequence);
    QCOMPARE(receiver2.sequence_slot2, ++sequence);
    QCOMPARE(receiver3.sequence_slot2, ++sequence);
    QCOMPARE(receiver4.sequence_slot2, ++sequence);
    QCOMPARE(receiver2.sequence_slot1, ++sequence);
    QCOMPARE(receiver1.sequence_slot1, ++sequence);

    // ensure emission order even if the connections change during emission
    SenderObject *sender2 = new SenderObject;
    SequenceObject seq1, seq2, *seq3 = new SequenceObject, seq4;
    seq1.next = &seq2;
    seq2.next = seq3;
    seq3->next = &seq4;

    // try 1
    connect(sender2, SIGNAL(signal1()), &seq1, SLOT(slot1_disconnectThis()));
    connect(sender2, SIGNAL(signal1()), &seq2, SLOT(slot1_disconnectNext()));
    connect(sender2, SIGNAL(signal1()), seq3, SLOT(slot1()));
    connect(sender2, SIGNAL(signal1()), &seq4, SLOT(slot1()));
    connect(sender2, SIGNAL(signal1()), &seq1, SLOT(slot2_reconnectThis()));
    connect(sender2, SIGNAL(signal1()), &seq2, SLOT(slot2_reconnectNext()));
    connect(sender2, SIGNAL(signal1()), seq3, SLOT(slot2()));
    connect(sender2, SIGNAL(signal1()), &seq4, SLOT(slot2()));

    SequenceObject::sequence = sequence = 0;
    sender2->emitSignal1();
    QVERIFY(seq1.called(1));
    QVERIFY(seq2.called(1));
    QVERIFY(!seq3->called(1));
    QVERIFY(seq4.called(1));
    QVERIFY(seq1.called(2));
    QVERIFY(seq2.called(2));
    QVERIFY(!seq3->called(2));
    QVERIFY(seq4.called(2));
    QCOMPARE(seq1.sequence_slot1, ++sequence);
    QCOMPARE(seq2.sequence_slot1, ++sequence);
    QCOMPARE(seq4.sequence_slot1, ++sequence);
    QCOMPARE(seq1.sequence_slot2, ++sequence);
    QCOMPARE(seq2.sequence_slot2, ++sequence);
    QCOMPARE(seq4.sequence_slot2, ++sequence);

    QObject::disconnect(sender2, 0, &seq1, 0);
    QObject::disconnect(sender2, 0, &seq2, 0);
    QObject::disconnect(sender2, 0, seq3, 0);
    QObject::disconnect(sender2, 0, &seq4, 0);
    seq1.reset();
    seq2.reset();
    seq3->reset();
    seq4.reset();

    // try 2
    connect(sender2, SIGNAL(signal1()), &seq1, SLOT(slot2_reconnectThis()));
    connect(sender2, SIGNAL(signal1()), &seq2, SLOT(slot2_reconnectNext()));
    connect(sender2, SIGNAL(signal1()), seq3, SLOT(slot2()));
    connect(sender2, SIGNAL(signal1()), &seq4, SLOT(slot2()));
    connect(sender2, SIGNAL(signal1()), &seq1, SLOT(slot1_disconnectThis()));
    connect(sender2, SIGNAL(signal1()), &seq2, SLOT(slot1_disconnectNext()));
    connect(sender2, SIGNAL(signal1()), seq3, SLOT(slot1()));
    connect(sender2, SIGNAL(signal1()), &seq4, SLOT(slot1()));

    SequenceObject::sequence = sequence = 0;
    sender2->emitSignal1();
    QVERIFY(seq1.called(2));
    QVERIFY(seq2.called(2));
    QVERIFY(!seq3->called(2));
    QVERIFY(seq4.called(2));
    QVERIFY(seq1.called(1));
    QVERIFY(seq2.called(1));
    QVERIFY(!seq3->called(1));
    QVERIFY(seq4.called(1));
    QCOMPARE(seq1.sequence_slot2, ++sequence);
    QCOMPARE(seq2.sequence_slot2, ++sequence);
    QCOMPARE(seq4.sequence_slot2, ++sequence);
    QCOMPARE(seq1.sequence_slot1, ++sequence);
    QCOMPARE(seq2.sequence_slot1, ++sequence);
    QCOMPARE(seq4.sequence_slot1, ++sequence);

    QObject::disconnect(sender2, 0, &seq1, 0);
    QObject::disconnect(sender2, 0, &seq2, 0);
    QObject::disconnect(sender2, 0, seq3, 0);
    QObject::disconnect(sender2, 0, &seq4, 0);
    seq1.reset();
    seq2.reset();
    seq3->reset();
    seq4.reset();

    // try 3
    connect(sender2, SIGNAL(signal1()), &seq1, SLOT(slot1()));
    connect(sender2, SIGNAL(signal1()), &seq2, SLOT(slot1_disconnectNext()));
    connect(sender2, SIGNAL(signal1()), seq3, SLOT(slot1()));
    connect(sender2, SIGNAL(signal1()), &seq4, SLOT(slot1()));
    connect(sender2, SIGNAL(signal1()), &seq1, SLOT(slot2()));
    connect(sender2, SIGNAL(signal1()), &seq2, SLOT(slot2_reconnectNext()));
    connect(sender2, SIGNAL(signal1()), seq3, SLOT(slot2()));
    connect(sender2, SIGNAL(signal1()), &seq4, SLOT(slot2()));

    SequenceObject::sequence = sequence = 0;
    sender2->emitSignal1();
    QVERIFY(seq1.called(1));
    QVERIFY(seq2.called(1));
    QVERIFY(!seq3->called(1));
    QVERIFY(seq4.called(1));
    QVERIFY(seq1.called(2));
    QVERIFY(seq2.called(2));
    QVERIFY(!seq3->called(2));
    QVERIFY(seq4.called(2));
    QCOMPARE(seq1.sequence_slot1, ++sequence);
    QCOMPARE(seq2.sequence_slot1, ++sequence);
    QCOMPARE(seq4.sequence_slot1, ++sequence);
    QCOMPARE(seq1.sequence_slot2, ++sequence);
    QCOMPARE(seq2.sequence_slot2, ++sequence);
    QCOMPARE(seq4.sequence_slot2, ++sequence);

    // ensure emission order even if objects are destroyed during emission
    QObject::disconnect(sender2, 0, &seq1, 0);
    QObject::disconnect(sender2, 0, &seq2, 0);
    QObject::disconnect(sender2, 0, seq3, 0);
    QObject::disconnect(sender2, 0, &seq4, 0);
    seq1.reset();
    seq2.reset();
    seq3->reset();
    seq4.reset();

    connect(sender2, SIGNAL(signal1()), &seq1, SLOT(slot1()));
    connect(sender2, SIGNAL(signal1()), &seq2, SLOT(slot1_deleteNext()));
    connect(sender2, SIGNAL(signal1()), seq3, SLOT(slot1()));
    connect(sender2, SIGNAL(signal1()), &seq4, SLOT(slot1()));
    connect(sender2, SIGNAL(signal1()), &seq1, SLOT(slot2()));
    connect(sender2, SIGNAL(signal1()), &seq2, SLOT(slot2_deleteSender()));
    connect(sender2, SIGNAL(signal1()), seq3, SLOT(slot2()));
    connect(sender2, SIGNAL(signal1()), &seq4, SLOT(slot2()));

    QPointer<SenderObject> psender = sender2;
    QPointer<SequenceObject> pseq3 = seq3;

    SequenceObject::sequence = sequence = 0;
    sender2->emitSignal1();
    QCOMPARE(static_cast<QObject *>(psender), static_cast<QObject *>(0));
    QCOMPARE(static_cast<QObject *>(pseq3), static_cast<QObject *>(0));
    QVERIFY(seq1.called(1));
    QVERIFY(seq2.called(1));
    QVERIFY(seq4.called(1));
    QVERIFY(seq1.called(2));
    QVERIFY(seq2.called(2));
    QVERIFY(!seq4.called(2));
    QCOMPARE(seq1.sequence_slot1, ++sequence);
    QCOMPARE(seq2.sequence_slot1, ++sequence);
    QCOMPARE(seq4.sequence_slot1, ++sequence);
    QCOMPARE(seq1.sequence_slot2, ++sequence);
    QCOMPARE(seq2.sequence_slot2, ++sequence);

    QPointer<SenderObject> psender3 = new SenderObject;
    connect(psender3, SIGNAL(signal1()), psender3, SIGNAL(signal2()));
    connect(psender3, SIGNAL(signal2()), &seq1, SLOT(slot2_deleteSender()));
    psender3->emitSignal1();
    QVERIFY(!psender3);
}

static int instanceCount = 0;

struct CheckInstanceCount
{
    const int saved;
    CheckInstanceCount() : saved(instanceCount) {}
    ~CheckInstanceCount() { QCOMPARE(saved, instanceCount); }
};


struct CustomType
{
    CustomType(int l1 = 0, int l2 = 0, int l3 = 0): i1(l1), i2(l2), i3(l3)
    { ++instanceCount; playWithObjects(); }
    CustomType(const CustomType &other): i1(other.i1), i2(other.i2), i3(other.i3)
    { ++instanceCount; playWithObjects(); }
    ~CustomType() { --instanceCount; playWithObjects(); }
    CustomType &operator=(const CustomType &) = default;

    int i1, i2, i3;
    int value() { return i1 + i2 + i3; }
};

QDataStream &operator<<(QDataStream &stream, const CustomType &ct)
{
    stream << ct.i1 << ct.i2 << ct.i3;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, CustomType &ct)
{
    stream >> ct.i1;
    stream >> ct.i2;
    stream >> ct.i3;
    return stream;
}

Q_DECLARE_METATYPE(CustomType*)
W_REGISTER_ARGTYPE(CustomType*)
Q_DECLARE_METATYPE(CustomType)
W_REGISTER_ARGTYPE(CustomType)

W_REGISTER_ARGTYPE(QList<CustomType>)

class QCustomTypeChecker: public QObject
{
    W_OBJECT(QCustomTypeChecker)

public:
    QCustomTypeChecker(QObject *parent = nullptr): QObject(parent) {}
    void doEmit(CustomType ct)
    { emit signal1(ct); }

public slots:
    void slot1(CustomType ct);
    W_SLOT(slot1)
    void slot2(const QList<CustomType> &ct);
    W_SLOT(slot2)

signals:
    void signal1(CustomType ct) W_SIGNAL(signal1,ct)
    void signal2(const QList<CustomType> &ct) W_SIGNAL(signal2,ct)

public:
    CustomType received;
};

void QCustomTypeChecker::slot1(CustomType ct)
{ received = ct; }

void QCustomTypeChecker::slot2(const QList< CustomType >& ct)
{ received = ct[0]; }

void tst_QObject::customTypes()
{
    CustomType t0;
    CustomType t1(1, 2, 3);
    CustomType t2(2, 3, 4);

    {
        QCustomTypeChecker checker;
        QCOMPARE(instanceCount, 4);

        connect(&checker, SIGNAL(signal1(CustomType)), &checker, SLOT(slot1(CustomType)),
                Qt::DirectConnection);
        QCOMPARE(checker.received.value(), 0);
        checker.doEmit(t1);
        QCOMPARE(checker.received.value(), t1.value());
        checker.received = t0;

        int idx = qRegisterMetaType<CustomType>("CustomType");
        QCOMPARE(QMetaType::type("CustomType"), idx);

        checker.disconnect();
        connect(&checker, SIGNAL(signal1(CustomType)), &checker, SLOT(slot1(CustomType)),
                Qt::QueuedConnection);
        QCOMPARE(instanceCount, 4);
        checker.doEmit(t2);
        QCOMPARE(instanceCount, 5);
        QCOMPARE(checker.received.value(), t0.value());

        QCoreApplication::processEvents();
        QCOMPARE(checker.received.value(), t2.value());
        QCOMPARE(instanceCount, 4);

        QVERIFY(QMetaType::isRegistered(idx));
        QCOMPARE(qRegisterMetaType<CustomType>("CustomType"), idx);
        QCOMPARE(QMetaType::type("CustomType"), idx);
        QVERIFY(QMetaType::isRegistered(idx));
    }
    QCOMPARE(instanceCount, 3);
}

void tst_QObject::streamCustomTypes()
{
    QByteArray ba;

    int idx = qRegisterMetaType<CustomType>("CustomType");

    {
        CustomType t1(1, 2, 3);
        QCOMPARE(instanceCount, 1);
        QDataStream stream(&ba, (QIODevice::OpenMode)QIODevice::WriteOnly);
        QMetaType::save(stream, idx, &t1);
    }

    QCOMPARE(instanceCount, 0);

    {
        CustomType t2;
        QCOMPARE(instanceCount, 1);
        QDataStream stream(&ba, (QIODevice::OpenMode)QIODevice::ReadOnly);
        QMetaType::load(stream, idx, &t2);
        QCOMPARE(instanceCount, 1);
        QCOMPARE(t2.i1, 1);
        QCOMPARE(t2.i2, 2);
        QCOMPARE(t2.i3, 3);
    }
    QCOMPARE(instanceCount, 0);
}

typedef QString CustomString;

class PropertyObject : public QObject
{
    W_OBJECT(PropertyObject)

public:
    enum Alpha {
        Alpha0,
        Alpha1,
        Alpha2
    };

    enum Priority { High, Low, VeryHigh, VeryLow };

    PropertyObject()
        : m_alpha(Alpha0), m_priority(High), m_number(0), m_custom(0), m_float(42)
    {}

    Alpha alpha() const { return m_alpha; }
    void setAlpha(Alpha alpha) { m_alpha = alpha; }

    Priority priority() const { return m_priority; }
    void setPriority(Priority priority) { m_priority = priority; }

    int number() const { return m_number; }
    void setNumber(int number) { m_number = number; }

    QString string() const { return m_string; }
    void setString(const QString &string) { m_string = string; }

    QVariant variant() const { return m_variant; }
    void setVariant(const QVariant &variant) { m_variant = variant; }

    CustomType *custom() const { return m_custom; }
    void setCustom(CustomType *custom) { m_custom = custom; }

    void setMyFloat(float value) { m_float = value; }
    inline float myFloat() const { return m_float; }

    void setMyQReal(qreal value) { m_qreal = value; }
    qreal myQReal() const { return m_qreal; }

    CustomString customString() const { return m_customString; }
    void setCustomString(const QString &string) { m_customString = string; }

private:
    Alpha m_alpha;
    Priority m_priority;
    int m_number;
    QString m_string;
    QVariant m_variant;
    CustomType *m_custom;
    float m_float;
    qreal m_qreal;
    CustomString m_customString;

    W_ENUM(Alpha, Alpha0, Alpha1, Alpha2)
    W_ENUM(Priority, High, Low, VeryHigh, VeryLow)

    W_PROPERTY(Alpha, alpha READ alpha WRITE setAlpha)
    W_PROPERTY(Priority, priority READ priority WRITE setPriority)
    W_PROPERTY(int, number READ number WRITE setNumber)
    W_PROPERTY(QString, string READ string WRITE setString)
    W_PROPERTY(QVariant, variant READ variant WRITE setVariant)
    W_PROPERTY(CustomType*, custom READ custom WRITE setCustom)
    W_PROPERTY(float, myFloat READ myFloat WRITE setMyFloat)
    W_PROPERTY(qreal, myQReal READ myQReal WRITE setMyQReal)
    W_PROPERTY(CustomString, customString READ customString WRITE setCustomString )

};

Q_DECLARE_METATYPE(PropertyObject::Priority)

void tst_QObject::threadSignalEmissionCrash()
{
    int loopCount = 1000;
    for (int i = 0; i < loopCount; ++i) {
        QTcpSocket socket;
        socket.connectToHost("localhost", 80);
    }
}

class TestThread : public QThread
{
    W_OBJECT(TestThread)
public:
    inline void run() override
    {
        *object = new QObject;
        *child = new QObject(*object);
        mutex.lock();
        cond.wakeOne();
        cond.wait(&mutex);
        mutex.unlock();
    }

    QObject **object, **child;
    QMutex mutex;
    QWaitCondition cond;
};

void tst_QObject::thread()
{
    QThread *currentThread = QThread::currentThread();
    // the current thread is the same as the QApplication
    // thread... see tst_QApplication::thread()

    {
        QObject object;
        // thread affinity for objects with no parent should be the
        // current thread
        QVERIFY(object.thread() != nullptr);
        QCOMPARE(object.thread(), currentThread);
        // children inherit their parent's thread
        QObject child(&object);
        QCOMPARE(child.thread(), object.thread());
    }

    QObject *object = nullptr;
    QObject *child = nullptr;

    {
        TestThread thr;
        QVERIFY(thr.thread() != nullptr);
        QCOMPARE(thr.thread(), currentThread);

        thr.object = &object;
        thr.child = &child;

        thr.mutex.lock();
        thr.start();
        thr.cond.wait(&thr.mutex);

        // thread affinity for an object with no parent should be the
        // thread in which the object was created
        QCOMPARE(object->thread(), (QThread *)&thr);
        // children inherit their parent's thread
        QCOMPARE(child->thread(), object->thread());

        thr.cond.wakeOne();
        thr.mutex.unlock();
        thr.wait();

        // even though the thread is no longer running, the affinity
        // should not change
        QCOMPARE(object->thread(), (QThread *)&thr);
        QCOMPARE(child->thread(), object->thread());
    }

    // the thread has been destroyed, thread affinity should
    // automatically reset to no thread
    QCOMPARE(object->thread(), (QThread *)0);
    QCOMPARE(child->thread(), object->thread());

    delete object;
}

class MoveToThreadObject : public QObject
{
    W_OBJECT(MoveToThreadObject)
public:
    QThread *timerEventThread = nullptr;
    QThread *customEventThread = nullptr;
    QThread *slotThread = nullptr;

    MoveToThreadObject(QObject *parent = nullptr)
        : QObject(parent)
    { }

    void customEvent(QEvent *) override
    {
        if (customEventThread)
            qFatal("%s: customEventThread should be null", Q_FUNC_INFO);
        customEventThread = QThread::currentThread();
        emit theSignal();
    }

    void timerEvent(QTimerEvent *) override
    {
        if (timerEventThread)
            qFatal("%s: timerEventThread should be null", Q_FUNC_INFO);
        timerEventThread = QThread::currentThread();
        emit theSignal();
    }

public slots:
    void theSlot()
    {
        if (slotThread)
            qFatal("%s: slotThread should be null", Q_FUNC_INFO);
        slotThread = QThread::currentThread();
        emit theSignal();
    }
    W_SLOT(theSlot)

signals:
    void theSignal() W_SIGNAL(theSignal)
};

class MoveToThreadThread : public QThread
{
public:
    ~MoveToThreadThread()
    {
        if (isRunning()) {
            terminate();
            wait();
        }
    }
    void start()
    {
        QEventLoop eventLoop;
        connect(this, SIGNAL(started()), &eventLoop, SLOT(quit()), Qt::QueuedConnection);
        QThread::start();
        // wait for thread to start
        (void) eventLoop.exec();
    }
    void run() override
    { (void) exec(); }
};

void tst_QObject::thread0()
{
    QObject *object = new QObject;
    object->moveToThread(0);
    QObject *child = new QObject(object);
    QCOMPARE(child->parent(), object);
    QCOMPARE(child->thread(), (QThread *)0);

#if 0
    // We don't support moving children into a parent that has no thread
    // affinity (yet?).
    QObject *child2 = new QObject;
    child2->moveToThread(0);
    child2->setParent(object);
    QCOMPARE(child2->parent(), object);
    QCOMPARE(child2->thread(), (QThread *)0);
#endif

    delete object;
}

void tst_QObject::moveToThread()
{
    QThread *currentThread = QThread::currentThread();

    {
        QObject *object = new QObject;
        QObject *child = new QObject(object);
        QCOMPARE(object->thread(), currentThread);
        QCOMPARE(child->thread(), currentThread);
        object->moveToThread(0);
        QCOMPARE(object->thread(), (QThread *)0);
        QCOMPARE(child->thread(), (QThread *)0);
        object->moveToThread(currentThread);
        QCOMPARE(object->thread(), currentThread);
        QCOMPARE(child->thread(), currentThread);
        object->moveToThread(0);
        QCOMPARE(object->thread(), (QThread *)0);
        QCOMPARE(child->thread(), (QThread *)0);
        // can delete an object with no thread anywhere
        delete object;
    }

    {
        MoveToThreadThread thread;
        thread.start();

        QObject *object = new QObject;
        QObject *child = new QObject(object);
        QPointer<QObject> opointer = object;
        QPointer<QObject> cpointer = object;

        QCOMPARE(object->thread(), currentThread);
        QCOMPARE(child->thread(), currentThread);
        object->moveToThread(&thread);
        QCOMPARE(object->thread(), (QThread *)&thread);
        QCOMPARE(child->thread(), (QThread *)&thread);

        connect(object, SIGNAL(destroyed()), &thread, SLOT(quit()), Qt::DirectConnection);
        QMetaObject::invokeMethod(object, "deleteLater", Qt::QueuedConnection);
        thread.wait();

        QVERIFY(opointer == nullptr);
        QVERIFY(cpointer == nullptr);
    }

    {
        // make sure posted events are moved with the object
        MoveToThreadThread thread;
        thread.start();

        MoveToThreadObject *object = new MoveToThreadObject;
        MoveToThreadObject *child = new MoveToThreadObject(object);

        connect(object, SIGNAL(theSignal()), &thread, SLOT(quit()), Qt::DirectConnection);
        QCoreApplication::postEvent(child, new QEvent(QEvent::User));
        QCoreApplication::postEvent(object, new QEvent(QEvent::User));

        QCOMPARE(object->thread(), currentThread);
        QCOMPARE(child->thread(), currentThread);
        object->moveToThread(&thread);
        QCOMPARE(object->thread(), (QThread *)&thread);
        QCOMPARE(child->thread(), (QThread *)&thread);

        thread.wait();

        QCOMPARE(object->customEventThread, (QThread *)&thread);
        QCOMPARE(child->customEventThread, (QThread *)&thread);

        thread.start();
        connect(object, SIGNAL(destroyed()), &thread, SLOT(quit()), Qt::DirectConnection);
        QMetaObject::invokeMethod(object, "deleteLater", Qt::QueuedConnection);
        thread.wait();
    }

    {
        // make sure timers are moved with the object
        MoveToThreadThread thread;
        thread.start();

        MoveToThreadObject *object = new MoveToThreadObject;
        MoveToThreadObject *child = new MoveToThreadObject(object);

        connect(object, SIGNAL(theSignal()), &thread, SLOT(quit()), Qt::DirectConnection);

        child->startTimer(90);
        object->startTimer(100);

        QCOMPARE(object->thread(), currentThread);
        QCOMPARE(child->thread(), currentThread);
        object->moveToThread(&thread);
        QCOMPARE(object->thread(), (QThread *)&thread);
        QCOMPARE(child->thread(), (QThread *)&thread);

        thread.wait();

        QCOMPARE(object->timerEventThread, (QThread *)&thread);
        QCOMPARE(child->timerEventThread, (QThread *)&thread);

        thread.start();
        connect(object, SIGNAL(destroyed()), &thread, SLOT(quit()), Qt::DirectConnection);
        QMetaObject::invokeMethod(object, "deleteLater", Qt::QueuedConnection);
        thread.wait();
    }

    {
        // make sure socket notifiers are moved with the object
        MoveToThreadThread thread;
        thread.start();

        QTcpServer server;
        QVERIFY(server.listen(QHostAddress::LocalHost, 0));
        QTcpSocket *socket = new QTcpSocket;
        MoveToThreadObject *child = new MoveToThreadObject(socket);
        connect(socket, SIGNAL(disconnected()), child, SLOT(theSlot()), Qt::DirectConnection);
        connect(child, SIGNAL(theSignal()), &thread, SLOT(quit()), Qt::DirectConnection);

        socket->connectToHost(server.serverAddress(), server.serverPort());

        QVERIFY(server.waitForNewConnection(1000));
        QTcpSocket *serverSocket = server.nextPendingConnection();
        QVERIFY(serverSocket);

        socket->waitForConnected();

        QCOMPARE(socket->thread(), currentThread);
        socket->moveToThread(&thread);
        QCOMPARE(socket->thread(), (QThread *)&thread);

        serverSocket->close();

        QVERIFY(thread.wait(10000));

        QCOMPARE(child->slotThread, (QThread *)&thread);

        thread.start();
        connect(socket, SIGNAL(destroyed()), &thread, SLOT(quit()), Qt::DirectConnection);
        QMetaObject::invokeMethod(socket, "deleteLater", Qt::QueuedConnection);
        thread.wait();
    }
}


void tst_QObject::property()
{
    PropertyObject object;
    const QMetaObject *mo = object.metaObject();
    QMetaProperty property;
    QVERIFY(mo);

    QVERIFY(mo->indexOfProperty("alpha") != -1);
    property = mo->property(mo->indexOfProperty("alpha"));
    QVERIFY(property.isEnumType());
#if QT_VERSION < QT_VERSION_CHECK(6, 1, 0)
    QCOMPARE(property.typeName(), "Alpha");
#else
    QCOMPARE(property.typeName(), "PropertyObject::Alpha");
#endif
    QCOMPARE(property.userType(), QMetaType::fromType<PropertyObject::Alpha>().id());

    QVariant var = object.property("alpha");
    QVERIFY(!var.isNull());
    QCOMPARE(var.toInt(), int(PropertyObject::Alpha0));
    object.setAlpha(PropertyObject::Alpha1);
    QCOMPARE(object.property("alpha").toInt(), int(PropertyObject::Alpha1));
    QVERIFY(object.setProperty("alpha", PropertyObject::Alpha2));
    QCOMPARE(object.property("alpha").toInt(), int(PropertyObject::Alpha2));
    QVERIFY(object.setProperty("alpha", "Alpha1"));
    QCOMPARE(object.property("alpha").toInt(), int(PropertyObject::Alpha1));
    QVERIFY(object.setProperty("alpha", QVariant()));
    QCOMPARE(object.property("alpha").toInt(), 0);

    QVERIFY(mo->indexOfProperty("number") != -1);
    QCOMPARE(object.property("number").toInt(), 0);
    object.setNumber(24);
    QCOMPARE(object.property("number"), QVariant(24));
    QVERIFY(object.setProperty("number", 12));
    QCOMPARE(object.property("number"), QVariant(12));
    QVERIFY(object.setProperty("number", "42"));
    QCOMPARE(object.property("number"), QVariant(42));

    QVERIFY(mo->indexOfProperty("string") != -1);
    QCOMPARE(object.property("string").toString(), QString());
    object.setString("String1");
    QCOMPARE(object.property("string"), QVariant("String1"));
    QVERIFY(object.setProperty("string", "String2"));
    QCOMPARE(object.property("string"), QVariant("String2"));
    QVERIFY(object.setProperty("string", QVariant()));

    const int idx = mo->indexOfProperty("variant");
    QVERIFY(idx != -1);
    QCOMPARE(QMetaType::Type(mo->property(idx).type()), QMetaType::QVariant);
    QCOMPARE(object.property("variant"), QVariant());
    QVariant variant1(42);
    QVariant variant2("string");
    object.setVariant(variant1);
    QCOMPARE(object.property("variant"), variant1);
    QVERIFY(object.setProperty("variant", variant2));
    QCOMPARE(object.variant(), QVariant(variant2));
    QCOMPARE(object.property("variant"), variant2);
    QVERIFY(object.setProperty("variant", QVariant()));
    QCOMPARE(object.property("variant"), QVariant());

    QVERIFY(mo->indexOfProperty("custom") != -1);
    property = mo->property(mo->indexOfProperty("custom"));
    QVERIFY(property.isValid());
    QVERIFY(property.isWritable());
    QVERIFY(!property.isEnumType());
    QCOMPARE(property.typeName(), "CustomType*");
    qRegisterMetaType<CustomType*>();
    QCOMPARE(property.type(), QVariant::UserType);
    QCOMPARE(property.userType(), qMetaTypeId<CustomType*>());

    CustomType *customPointer = nullptr;
    QVariant customVariant = object.property("custom");
    customPointer = qvariant_cast<CustomType *>(customVariant);
    QCOMPARE(customPointer, object.custom());

    CustomType custom;
    customPointer = &custom;
    customVariant.setValue(customPointer);

    property = mo->property(mo->indexOfProperty("custom"));
    QVERIFY(property.isWritable());
    QCOMPARE(property.typeName(), "CustomType*");
    QCOMPARE(property.type(), QVariant::UserType);
    QCOMPARE(property.userType(), qMetaTypeId<CustomType*>());

    QVERIFY(object.setProperty("custom", customVariant));
    QCOMPARE(object.custom(), customPointer);

    customVariant = object.property("custom");
    customPointer = qvariant_cast<CustomType *>(customVariant);
    QCOMPARE(object.custom(), customPointer);

    // this enum property has a meta type, but it's not yet registered, so we know this fails
    QVERIFY(mo->indexOfProperty("priority") != -1);
    property = mo->property(mo->indexOfProperty("priority"));
    QVERIFY(property.isEnumType());
#if QT_VERSION < QT_VERSION_CHECK(6, 1, 0)
    QCOMPARE(property.typeName(), "Priority");
#else
    QCOMPARE(property.typeName(), "PropertyObject::Priority");
#endif
    QCOMPARE(property.userType(), QMetaType::fromType<PropertyObject::Priority>().id());

    var = object.property("priority");
    QVERIFY(!var.isNull());
    QCOMPARE(var.toInt(), int(PropertyObject::High));
    object.setPriority(PropertyObject::Low);
    QCOMPARE(object.property("priority").toInt(), int(PropertyObject::Low));
    QVERIFY(object.setProperty("priority", PropertyObject::VeryHigh));
    QCOMPARE(object.property("priority").toInt(), int(PropertyObject::VeryHigh));
    QVERIFY(object.setProperty("priority", "High"));
    QCOMPARE(object.property("priority").toInt(), int(PropertyObject::High));
    QVERIFY(object.setProperty("priority", QVariant()));
    QCOMPARE(object.property("priority").toInt(), 0);

    // now it's registered, so it works as expected
    int priorityMetaTypeId = qRegisterMetaType<PropertyObject::Priority>("PropertyObject::Priority");

    QVERIFY(mo->indexOfProperty("priority") != -1);
    property = mo->property(mo->indexOfProperty("priority"));
    QVERIFY(property.isEnumType());
#if QT_VERSION < QT_VERSION_CHECK(6, 1, 0)
    QCOMPARE(property.typeName(), "Priority");
#else
    QCOMPARE(property.typeName(), "PropertyObject::Priority");
#endif
    QCOMPARE(property.type(), QVariant::UserType);
    QCOMPARE(property.userType(), priorityMetaTypeId);

    var = object.property("priority");
    QVERIFY(!var.isNull());
    QVERIFY(var.canConvert<PropertyObject::Priority>());
    QCOMPARE(qvariant_cast<PropertyObject::Priority>(var), PropertyObject::High);
    object.setPriority(PropertyObject::Low);
    QCOMPARE(qvariant_cast<PropertyObject::Priority>(object.property("priority")), PropertyObject::Low);
    QVERIFY(object.setProperty("priority", PropertyObject::VeryHigh));
    QCOMPARE(qvariant_cast<PropertyObject::Priority>(object.property("priority")), PropertyObject::VeryHigh);
    QVERIFY(object.setProperty("priority", "High"));
    QCOMPARE(qvariant_cast<PropertyObject::Priority>(object.property("priority")), PropertyObject::High);
    QVERIFY(object.setProperty("priority", QVariant()));
    QCOMPARE(object.property("priority").toInt(), 0);

    var = object.property("priority");
    QCOMPARE(qvariant_cast<PropertyObject::Priority>(var), PropertyObject::High);
    object.setPriority(PropertyObject::Low);
    QCOMPARE(qvariant_cast<PropertyObject::Priority>(object.property("priority")), PropertyObject::Low);
    object.setProperty("priority", var);
    QCOMPARE(qvariant_cast<PropertyObject::Priority>(object.property("priority")), PropertyObject::High);

    qRegisterMetaType<CustomString>("CustomString");
    QVERIFY(mo->indexOfProperty("customString") != -1);
    QCOMPARE(object.property("customString").toString(), QString());
    object.setCustomString("String1");
    QCOMPARE(object.property("customString"), QVariant("String1"));
    QVERIFY(object.setProperty("customString", "String2"));
    QCOMPARE(object.property("customString"), QVariant("String2"));
    QVERIFY(object.setProperty("customString", QVariant()));
}

void tst_QObject::metamethod()
{
    SenderObject obj;
    const QMetaObject *mobj = obj.metaObject();
    QMetaMethod m;

    m = mobj->method(mobj->indexOfMethod("invoke1()"));
    QVERIFY(m.methodSignature() == "invoke1()");
    QCOMPARE(m.methodType(), QMetaMethod::Method);
    QCOMPARE(m.access(), QMetaMethod::Public);
    QVERIFY(!(m.attributes() & QMetaMethod::Scriptable));
    QVERIFY(!(m.attributes() & QMetaMethod::Compatibility));

    m = mobj->method(mobj->indexOfMethod("sinvoke1()"));
    QVERIFY(m.methodSignature() == "sinvoke1()");
    QCOMPARE(m.methodType(), QMetaMethod::Method);
    QCOMPARE(m.access(), QMetaMethod::Public);
    QVERIFY((m.attributes() & QMetaMethod::Scriptable));
    QVERIFY(!(m.attributes() & QMetaMethod::Compatibility));

    m = mobj->method(mobj->indexOfMethod("invoke2()"));
    QVERIFY(m.methodSignature() == "invoke2()");
    QCOMPARE(m.methodType(), QMetaMethod::Method);
    QCOMPARE(m.access(), QMetaMethod::Protected);
    QVERIFY(!(m.attributes() & QMetaMethod::Scriptable));
    QVERIFY((m.attributes() & QMetaMethod::Compatibility));

    m = mobj->method(mobj->indexOfMethod("sinvoke2()"));
    QVERIFY(m.methodSignature() == "sinvoke2()");
    QCOMPARE(m.methodType(), QMetaMethod::Method);
    QCOMPARE(m.access(), QMetaMethod::Protected);
    QVERIFY((m.attributes() & QMetaMethod::Scriptable));
    QVERIFY((m.attributes() & QMetaMethod::Compatibility));

#if 0 // Verdigris does not support default values with W_INVOKABLE
    m = mobj->method(mobj->indexOfMethod("invoke3()"));
    QVERIFY(m.methodSignature() == "invoke3()");
#else
    m = mobj->method(mobj->indexOfMethod("invoke3(int,int)"));
    QVERIFY(m.methodSignature() == "invoke3(int,int)");
#endif
    QCOMPARE(m.methodType(), QMetaMethod::Method);
    QCOMPARE(m.access(), QMetaMethod::Private);
    QVERIFY(!(m.attributes() & QMetaMethod::Scriptable));
    QVERIFY(!(m.attributes() & QMetaMethod::Compatibility));

    m = mobj->method(mobj->indexOfMethod("sinvoke3()"));
    QVERIFY(m.methodSignature() == "sinvoke3()");
    QCOMPARE(m.methodType(), QMetaMethod::Method);
    QCOMPARE(m.access(), QMetaMethod::Private);
    QVERIFY((m.attributes() & QMetaMethod::Scriptable));
    QVERIFY(!(m.attributes() & QMetaMethod::Compatibility));

    m = mobj->method(mobj->indexOfMethod("signal5()"));
    QVERIFY(m.methodSignature() == "signal5()");
    QCOMPARE(m.methodType(), QMetaMethod::Signal);
    QCOMPARE(m.access(), QMetaMethod::Public);
    QVERIFY(!(m.attributes() & QMetaMethod::Scriptable));
    QVERIFY((m.attributes() & QMetaMethod::Compatibility));

    m = mobj->method(mobj->indexOfMethod("aPublicSlot()"));
    QVERIFY(m.methodSignature() == "aPublicSlot()");
    QCOMPARE(m.methodType(), QMetaMethod::Slot);
    QCOMPARE(m.access(), QMetaMethod::Public);
    QVERIFY(!(m.attributes() & QMetaMethod::Scriptable));
    QVERIFY(!(m.attributes() & QMetaMethod::Compatibility));

    m = mobj->method(mobj->indexOfMethod("invoke1()"));
    QCOMPARE(m.parameterNames().count(), 0);
    QCOMPARE(m.parameterTypes().count(), 0);

    m = mobj->method(mobj->indexOfMethod("invoke2(int)"));
    QCOMPARE(m.parameterNames().count(), 1);
    QCOMPARE(m.parameterTypes().count(), 1);
    QCOMPARE(m.parameterTypes().at(0), QByteArray("int"));
    QVERIFY(m.parameterNames().at(0).isEmpty());

    m = mobj->method(mobj->indexOfMethod("invoke3(int,int)"));
    QCOMPARE(m.parameterNames().count(), 2);
    QCOMPARE(m.parameterTypes().count(), 2);
    QCOMPARE(m.parameterTypes().at(0), QByteArray("int"));
#if 0 // Verdigris does not support parameterNames()
    QCOMPARE(m.parameterNames().at(0), QByteArray("hinz"));
#endif
    QCOMPARE(m.parameterTypes().at(1), QByteArray("int"));
#if 0 // Verdigris does not support parameterNames()
    QCOMPARE(m.parameterNames().at(1), QByteArray("kunz"));
#endif

}

namespace QObjectTest
{
    class TestObject: public QObject
    {
        W_OBJECT(TestObject)
    public:
        TestObject(): QObject(), i(0) {}
        void doEmit() { emit aSignal(); }
        int i;
    public slots:
        void aSlot() { ++i; }
        W_SLOT(aSlot)
    signals:
        void aSignal() W_SIGNAL(aSignal)
    };
}

void tst_QObject::namespaces()
{
    QObjectTest::TestObject obj;

    QVERIFY(connect(&obj, SIGNAL(aSignal()), &obj, SLOT(aSlot())));
    obj.doEmit();
    QCOMPARE(obj.i, 1);
}

class SuperObject : public QObject
{
    W_OBJECT(SuperObject)
public:
    QObject *theSender;
    int theSignalId;

    SuperObject()
    {
        theSender = nullptr;
        theSignalId = 0;
    }

    friend class tst_QObject;

    using QObject::sender;

public slots:
    void rememberSender()
    {
        theSender = sender();
        theSignalId = senderSignalIndex();
    }
    W_SLOT(rememberSender)

    void deleteAndRememberSender()
    {
        delete theSender;
        rememberSender();
    }
    W_SLOT(deleteAndRememberSender)
signals:
    void anotherSignal() W_SIGNAL(anotherSignal)
    void theSignal() W_SIGNAL(theSignal)
};

void tst_QObject::senderTest()
{
    {
        SuperObject sender;
        SuperObject receiver;
        connect(&sender, SIGNAL(anotherSignal()),
                &receiver, SLOT(rememberSender()));
        connect(&sender, SIGNAL(theSignal()),
                &receiver, SLOT(rememberSender()));
        QCOMPARE(receiver.sender(), (QObject *)0);
        QCOMPARE(receiver.senderSignalIndex(), -1);
        emit sender.theSignal();
        QCOMPARE(receiver.theSender, (QObject *)&sender);
        QCOMPARE(receiver.sender(), (QObject *)0);
        QCOMPARE(receiver.theSignalId,
                 sender.metaObject()->indexOfSignal("theSignal()"));
        QCOMPARE(receiver.senderSignalIndex(), -1);

        emit sender.anotherSignal();
        QCOMPARE(receiver.theSignalId,
                 sender.metaObject()->indexOfSignal("anotherSignal()"));
        QCOMPARE(receiver.senderSignalIndex(), -1);
    }

    {
        SuperObject *sender = new SuperObject;
        SuperObject *receiver = new SuperObject;
        connect(sender, SIGNAL(theSignal()),
                receiver, SLOT(rememberSender()),
                Qt::BlockingQueuedConnection);

        QThread thread;
        receiver->moveToThread(&thread);
        connect(sender, SIGNAL(theSignal()),
                &thread, SLOT(quit()),
                Qt::DirectConnection);

        QCOMPARE(receiver->sender(), (QObject *)0);
        QCOMPARE(receiver->senderSignalIndex(), -1);
        receiver->theSender = nullptr;
        receiver->theSignalId = -1;
        thread.start();
        emit sender->theSignal();
        QCOMPARE(receiver->theSender, (QObject *) sender);
        QCOMPARE(receiver->sender(), (QObject *)0);
        QCOMPARE(receiver->theSignalId,
                 sender->metaObject()->indexOfSignal("theSignal()"));
        QCOMPARE(receiver->senderSignalIndex(), -1);

        QVERIFY(thread.wait(10000));
        delete receiver;
        delete sender;
    }

    {
        SuperObject *sender = new SuperObject;
        SuperObject receiver;
        connect(sender, SIGNAL(theSignal()),
                &receiver, SLOT(deleteAndRememberSender()));
        QCOMPARE(receiver.sender(), (QObject *)0);
        receiver.theSender = sender;
        emit sender->theSignal();
        QCOMPARE(receiver.theSender, (QObject *)0);
        QCOMPARE(receiver.sender(), (QObject *)0);
    }

    {
        SuperObject *sender = new SuperObject;
        SuperObject *receiver = new SuperObject;
        connect(sender, SIGNAL(theSignal()),
                receiver, SLOT(deleteAndRememberSender()),
                Qt::BlockingQueuedConnection);

        QThread thread;
        receiver->moveToThread(&thread);
        connect(sender, SIGNAL(destroyed()),
                &thread, SLOT(quit()),
                Qt::DirectConnection);

        QCOMPARE(receiver->sender(), (QObject *)0);
        receiver->theSender = sender;
        thread.start();
        emit sender->theSignal();
        QCOMPARE(receiver->theSender, (QObject *)0);
        QCOMPARE(receiver->sender(), (QObject *)0);

        QVERIFY(thread.wait(10000));
        delete receiver;
    }
}

namespace Foo
{
    struct Bar
    {
        virtual ~Bar() {}
        virtual int rtti() const = 0;
    };

    struct Bleh
    {
        virtual ~Bleh() {}
        virtual int rtti() const = 0;
    };
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(Foo::Bar, "com.qtest.foobar")
QT_END_NAMESPACE

#define Bleh_iid "com.qtest.bleh"
QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(Foo::Bleh, Bleh_iid)
QT_END_NAMESPACE

class FooObject: public QObject, public Foo::Bar
{
    W_OBJECT(FooObject)
    W_INTERFACE(Foo::Bar)
public:
    int rtti() const override { return 42; }
};

class BlehObject : public QObject, public Foo::Bleh
{
    W_OBJECT(BlehObject)
    W_INTERFACE(Foo::Bleh)
public:
    int rtti() const override { return 43; }
};

void tst_QObject::declareInterface()
{
    FooObject obj;

    Foo::Bar *bar = qobject_cast<Foo::Bar *>(&obj);
    QVERIFY(bar);
    QCOMPARE(bar->rtti(), 42);
    QCOMPARE(static_cast<Foo::Bar *>(&obj), bar);

    BlehObject bleh;

    bar = qobject_cast<Foo::Bar *>(&bleh);
    QVERIFY(!bar);
    Foo::Bleh *b = qobject_cast<Foo::Bleh *>(&bleh);
    QCOMPARE(b->rtti(), 43);
    QCOMPARE(static_cast<Foo::Bleh *>(&bleh), b);

}

class DestroyedListener : public QObject
{
    W_OBJECT(DestroyedListener)
public:
    inline DestroyedListener() : pointerWasZero(false) {}

    QPointer<QObject> pointer;
    bool pointerWasZero;

private slots:
    inline void otherObjectDestroyed()
    { pointerWasZero = pointer.isNull(); }
    W_SLOT(otherObjectDestroyed)
};

void tst_QObject::qpointerResetBeforeDestroyedSignal()
{
    QObject *obj = new QObject;
    DestroyedListener listener;
    listener.pointer = obj;
    listener.pointerWasZero = false;
    connect(obj, SIGNAL(destroyed()), &listener, SLOT(otherObjectDestroyed()));
    delete obj;
    QVERIFY(listener.pointerWasZero);
    QVERIFY(listener.pointer.isNull());
}

class DefaultArguments : public QObject
{
    W_OBJECT(DefaultArguments)

public slots:

    void theSlot(const QString &s) { result = s; }
    W_SLOT(theSlot)

signals:
    void theOriginalSignal() W_SIGNAL(theOriginalSignal)
    void theSecondSignal(const QString &s = QString("secondDefault")) W_SIGNAL(theSecondSignal,s)

public:

    void emitTheOriginalSignal() { emit theOriginalSignal(); }
    void emitTheSecondSignal() { emit theSecondSignal(); }
    QString result;
};

void tst_QObject::connectSignalsToSignalsWithDefaultArguments()
{
    QSKIP("Not supported by W_SLOT/W_SIGNAL");

    DefaultArguments o;
    connect(&o, SIGNAL(theOriginalSignal()), &o, SIGNAL(theSecondSignal()));
    connect(&o, SIGNAL(theSecondSignal(QString)), &o, SLOT(theSlot(QString)));
    QVERIFY( o.result.isEmpty() );
    o.emitTheSecondSignal();
    QCOMPARE(o.result, QString("secondDefault"));
    o.result = "Not called";
    o.emitTheOriginalSignal();
    QCOMPARE(o.result, QString("secondDefault"));

}

void tst_QObject::receivers()
{
    class Object : public QObject
    {
    public:
        int receivers(const char* signal) const
        { return QObject::receivers(signal); }
    };

    Object object;
    QCOMPARE(object.receivers(SIGNAL(destroyed())), 0);
    object.connect(&object, SIGNAL(destroyed()), SLOT(deleteLater()));
    QCOMPARE(object.receivers(SIGNAL(destroyed())), 1);
    object.connect(&object, SIGNAL(destroyed()), SLOT(deleteLater()));
    QCOMPARE(object.receivers(SIGNAL(destroyed())), 2);
    object.disconnect(SIGNAL(destroyed()), &object, SLOT(deleteLater()));
    QCOMPARE(object.receivers(SIGNAL(destroyed())), 0);
}

enum Enum { };

struct Struct { };
class Class { };
template <typename T> class Template { };

W_REGISTER_ARGTYPE(uint*)
W_REGISTER_ARGTYPE(ulong*)
W_REGISTER_ARGTYPE(const uint*)
W_REGISTER_ARGTYPE(const ulong*)
W_REGISTER_ARGTYPE(Struct)
W_REGISTER_ARGTYPE(Class)
W_REGISTER_ARGTYPE(Enum)
W_REGISTER_ARGTYPE(Struct*)
W_REGISTER_ARGTYPE(Class*)
W_REGISTER_ARGTYPE(Enum*)
W_REGISTER_ARGTYPE(const Struct*)
W_REGISTER_ARGTYPE(const Class*)
W_REGISTER_ARGTYPE(const Enum*)
W_REGISTER_ARGTYPE(const Struct*const*)
W_REGISTER_ARGTYPE(const Class*const*)
W_REGISTER_ARGTYPE(const Enum*const*)
W_REGISTER_ARGTYPE(Template<Class&>&)
W_REGISTER_ARGTYPE(Template<const Class&>)
W_REGISTER_ARGTYPE(Class*const&)
W_REGISTER_ARGTYPE(Template<int>)
W_REGISTER_ARGTYPE(Template<const int>)


class NormalizeObject : public QObject
{
    W_OBJECT(NormalizeObject)

public:

signals:
    void uintPointerSignal(uint * a) W_SIGNAL(uintPointerSignal,a)
    void ulongPointerSignal(ulong * a) W_SIGNAL(ulongPointerSignal,a)
    void constUintPointerSignal(const uint * a) W_SIGNAL(constUintPointerSignal,a)
    void constUlongPointerSignal(const ulong * a) W_SIGNAL(constUlongPointerSignal,a)

    void structSignal(Struct s) W_SIGNAL(structSignal,s)
    void classSignal(Class c) W_SIGNAL(classSignal,c)
    void enumSignal(Enum e) W_SIGNAL(enumSignal,e)

    void structPointerSignal(Struct *s) W_SIGNAL(structPointerSignal,s)
    void classPointerSignal(Class *c) W_SIGNAL(classPointerSignal,c)
    void enumPointerSignal(Enum *e) W_SIGNAL(enumPointerSignal,e)

    void constStructPointerSignal(const Struct *s) W_SIGNAL(constStructPointerSignal,s)
    void constClassPointerSignal(const Class *c) W_SIGNAL(constClassPointerSignal,c)
    void constEnumPointerSignal(const Enum *e) W_SIGNAL(constEnumPointerSignal,e)

    void constStructPointerConstPointerSignal(const Struct * const *s) W_SIGNAL(constStructPointerConstPointerSignal,s)
    void constClassPointerConstPointerSignal(const Class * const *c) W_SIGNAL(constClassPointerConstPointerSignal,c)
    void constEnumPointerConstPointerSignal(const Enum * const *e) W_SIGNAL(constEnumPointerConstPointerSignal,e)

    void unsignedintSignal(unsigned int a) W_SIGNAL(unsignedintSignal,a)
    void unsignedSignal(unsigned a) W_SIGNAL(unsignedSignal,a)
    void unsignedlongSignal(unsigned long a) W_SIGNAL(unsignedlongSignal,a)
    void unsignedlonglongSignal(quint64 a) W_SIGNAL(unsignedlonglongSignal,a)
    void unsignedlongintSignal(unsigned long int a) W_SIGNAL(unsignedlongintSignal,a)
    void unsignedshortSignal(unsigned short a) W_SIGNAL(unsignedshortSignal,a)
    void unsignedcharSignal(unsigned char a) W_SIGNAL(unsignedcharSignal,a)

    void typeRefSignal(Template<Class &> &ref) W_SIGNAL(typeRefSignal,ref)
    void constTypeRefSignal(const Template<Class const &> &ref) W_SIGNAL(constTypeRefSignal,ref)
    void typeConstRefSignal(Template<Class const &> const &ref) W_SIGNAL(typeConstRefSignal,ref)

    void typePointerConstRefSignal(Class * const &a) W_SIGNAL(typePointerConstRefSignal,a)

    void constTemplateSignal1( Template<int > a) W_SIGNAL(constTemplateSignal1,a)
    void constTemplateSignal2( Template< const int >a) W_SIGNAL(constTemplateSignal2,a)

public slots:
    void uintPointerSlot(uint *) { }
    W_SLOT(uintPointerSlot)
    void ulongPointerSlot(ulong *) { }
    W_SLOT(ulongPointerSlot)
    void constUintPointerSlot(const uint *) { }
    W_SLOT(constUintPointerSlot)
    void constUlongPointerSlot(const ulong *) { }
    W_SLOT(constUlongPointerSlot)

    void structSlot(Struct s) { Q_UNUSED(s); }
    W_SLOT(structSlot)
    void classSlot(Class c) { Q_UNUSED(c); }
    W_SLOT(classSlot)
    void enumSlot(Enum e) { Q_UNUSED(e); }
    W_SLOT(enumSlot)

    void structPointerSlot(Struct *s) { Q_UNUSED(s); }
    W_SLOT(structPointerSlot)
    void classPointerSlot(Class *c) { Q_UNUSED(c); }
    W_SLOT(classPointerSlot)
    void enumPointerSlot(Enum *e) { Q_UNUSED(e); }
    W_SLOT(enumPointerSlot)

    void constStructPointerSlot(const Struct *s) { Q_UNUSED(s); }
    W_SLOT(constStructPointerSlot)
    void constClassPointerSlot(const Class *c) { Q_UNUSED(c); }
    W_SLOT(constClassPointerSlot)
    void constEnumPointerSlot(const Enum *e) { Q_UNUSED(e); }
    W_SLOT(constEnumPointerSlot)

    void constStructPointerConstPointerSlot(const Struct * const *s) { Q_UNUSED(s); }
    W_SLOT(constStructPointerConstPointerSlot)
    void constClassPointerConstPointerSlot(const Class * const *c) { Q_UNUSED(c); }
    W_SLOT(constClassPointerConstPointerSlot)
    void constEnumPointerConstPointerSlot(const Enum * const *e) { Q_UNUSED(e); }
    W_SLOT(constEnumPointerConstPointerSlot)

    void uintSlot(uint) {}
    W_SLOT(uintSlot);
    void unsignedintSlot(unsigned int) {}
    W_SLOT(unsignedintSlot);
    void unsignedSlot(unsigned) {}
    W_SLOT(unsignedSlot);
    void unsignedlongSlot(unsigned long) {}
    W_SLOT(unsignedlongSlot);
    void unsignedlonglongSlot(quint64) {}
    W_SLOT(unsignedlonglongSlot);
    void unsignedlongintSlot(unsigned long int) {}
    W_SLOT(unsignedlongintSlot);
    void unsignedshortSlot(unsigned short) {}
    W_SLOT(unsignedshortSlot);
    void unsignedcharSlot(unsigned char) {}
    W_SLOT(unsignedcharSlot);

    void typeRefSlot(Template<Class &> &) {}
    W_SLOT(typeRefSlot)
    void constTypeRefSlot(const Template<const Class &> &) {}
    W_SLOT(constTypeRefSlot)
    void typeConstRefSlot(Template<Class const &> const &) {}
    W_SLOT(typeConstRefSlot)

    void typePointerConstRefSlot(Class * const &) {}
    W_SLOT(typePointerConstRefSlot)

    void constTemplateSlot1(Template<int > const) {}
    W_SLOT(constTemplateSlot1)
    void constTemplateSlot2(const Template<int > ) {}
    W_SLOT(constTemplateSlot2)
    void constTemplateSlot3(const Template< const int >) {}
    W_SLOT(constTemplateSlot3)
};

void tst_QObject::normalize()
{
    NormalizeObject object;

    // unsigned int -> uint, unsigned long -> ulong
    QVERIFY(object.connect(&object,
                           SIGNAL(uintPointerSignal(uint *)),
                           SLOT(uintPointerSlot(uint *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(uintPointerSignal(unsigned int *)),
                           SLOT(uintPointerSlot(uint *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(uintPointerSignal(uint *)),
                           SLOT(uintPointerSlot(unsigned int *))));

    QVERIFY(object.connect(&object,
                           SIGNAL(constUintPointerSignal(const uint *)),
                           SLOT(constUintPointerSlot(const uint *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constUintPointerSignal(const unsigned int *)),
                           SLOT(constUintPointerSlot(const uint *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constUintPointerSignal(const uint *)),
                           SLOT(constUintPointerSlot(const unsigned int *))));

    QVERIFY(object.connect(&object,
                           SIGNAL(ulongPointerSignal(ulong *)),
                           SLOT(ulongPointerSlot(ulong *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(ulongPointerSignal(unsigned long *)),
                           SLOT(ulongPointerSlot(ulong *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(ulongPointerSignal(ulong *)),
                           SLOT(ulongPointerSlot(unsigned long *))));

    QVERIFY(object.connect(&object,
                           SIGNAL(constUlongPointerSignal(const ulong *)),
                           SLOT(constUlongPointerSlot(const ulong *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constUlongPointerSignal(const unsigned long *)),
                           SLOT(constUlongPointerSlot(const ulong *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constUlongPointerSignal(const ulong *)),
                           SLOT(constUlongPointerSlot(const unsigned long *))));

    // struct, class, and enum are optional
    QVERIFY(object.connect(&object,
                           SIGNAL(structSignal(struct Struct)),
                           SLOT(structSlot(struct Struct))));
    QVERIFY(object.connect(&object,
                           SIGNAL(structSignal(Struct)),
                           SLOT(structSlot(struct Struct))));
    QVERIFY(object.connect(&object,
                           SIGNAL(structSignal(struct Struct)),
                           SLOT(structSlot(Struct))));
    QVERIFY(object.connect(&object,
                           SIGNAL(classSignal(class Class)),
                           SLOT(classSlot(class Class))));
    QVERIFY(object.connect(&object,
                           SIGNAL(classSignal(Class)),
                           SLOT(classSlot(class Class))));
    QVERIFY(object.connect(&object,
                           SIGNAL(classSignal(class Class)),
                           SLOT(classSlot(Class))));
    QVERIFY(object.connect(&object,
                           SIGNAL(enumSignal(enum Enum)),
                           SLOT(enumSlot(enum Enum))));
    QVERIFY(object.connect(&object,
                           SIGNAL(enumSignal(Enum)),
                           SLOT(enumSlot(enum Enum))));
    QVERIFY(object.connect(&object,
                           SIGNAL(enumSignal(enum Enum)),
                           SLOT(enumSlot(Enum))));

    QVERIFY(object.connect(&object,
                           SIGNAL(structPointerSignal(struct Struct *)),
                           SLOT(structPointerSlot(struct Struct *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(structPointerSignal(Struct *)),
                           SLOT(structPointerSlot(struct Struct *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(structPointerSignal(struct Struct *)),
                           SLOT(structPointerSlot(Struct *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(classPointerSignal(class Class *)),
                           SLOT(classPointerSlot(class Class *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(classPointerSignal(Class *)),
                           SLOT(classPointerSlot(class Class *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(classPointerSignal(class Class *)),
                           SLOT(classPointerSlot(Class *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(enumPointerSignal(enum Enum *)),
                           SLOT(enumPointerSlot(enum Enum *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(enumPointerSignal(Enum *)),
                           SLOT(enumPointerSlot(enum Enum *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(enumPointerSignal(enum Enum *)),
                           SLOT(enumPointerSlot(Enum *))));

    QVERIFY(object.connect(&object,
                           SIGNAL(constStructPointerSignal(const struct Struct *)),
                           SLOT(constStructPointerSlot(const struct Struct *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constStructPointerSignal(const Struct *)),
                           SLOT(constStructPointerSlot(const struct Struct *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constStructPointerSignal(const struct Struct *)),
                           SLOT(constStructPointerSlot(const Struct *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constClassPointerSignal(const class Class *)),
                           SLOT(constClassPointerSlot(const class Class *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constClassPointerSignal(const Class *)),
                           SLOT(constClassPointerSlot(const class Class *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constClassPointerSignal(const class Class *)),
                           SLOT(constClassPointerSlot(const Class *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constEnumPointerSignal(const enum Enum *)),
                           SLOT(constEnumPointerSlot(const enum Enum *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constEnumPointerSignal(const Enum *)),
                           SLOT(constEnumPointerSlot(const enum Enum *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constEnumPointerSignal(const enum Enum *)),
                           SLOT(constEnumPointerSlot(const Enum *))));

    QVERIFY(object.connect(&object,
                           SIGNAL(constStructPointerSignal(struct Struct const *)),
                           SLOT(constStructPointerSlot(struct Struct const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constStructPointerSignal(Struct const *)),
                           SLOT(constStructPointerSlot(struct Struct const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constStructPointerSignal(struct Struct const *)),
                           SLOT(constStructPointerSlot(Struct const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constClassPointerSignal(class Class const *)),
                           SLOT(constClassPointerSlot(class Class const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constClassPointerSignal(Class const *)),
                           SLOT(constClassPointerSlot(class Class const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constClassPointerSignal(class Class const *)),
                           SLOT(constClassPointerSlot(Class const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constEnumPointerSignal(enum Enum const *)),
                           SLOT(constEnumPointerSlot(enum Enum const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constEnumPointerSignal(Enum const *)),
                           SLOT(constEnumPointerSlot(enum Enum const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constEnumPointerSignal(enum Enum const *)),
                           SLOT(constEnumPointerSlot(Enum const *))));

    QVERIFY(object.connect(&object,
                           SIGNAL(constStructPointerConstPointerSignal(const struct Struct * const *)),
                           SLOT(constStructPointerConstPointerSlot(const struct Struct * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constStructPointerConstPointerSignal(const Struct * const *)),
                           SLOT(constStructPointerConstPointerSlot(const struct Struct * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constStructPointerConstPointerSignal(const struct Struct * const *)),
                           SLOT(constStructPointerConstPointerSlot(const Struct * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constClassPointerConstPointerSignal(const class Class * const *)),
                           SLOT(constClassPointerConstPointerSlot(const class Class * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constClassPointerConstPointerSignal(const Class * const *)),
                           SLOT(constClassPointerConstPointerSlot(const class Class * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constClassPointerConstPointerSignal(const class Class * const *)),
                           SLOT(constClassPointerConstPointerSlot(const Class * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constEnumPointerConstPointerSignal(const enum Enum * const *)),
                           SLOT(constEnumPointerConstPointerSlot(const enum Enum * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constEnumPointerConstPointerSignal(const Enum * const *)),
                           SLOT(constEnumPointerConstPointerSlot(const enum Enum * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constEnumPointerConstPointerSignal(const enum Enum * const *)),
                           SLOT(constEnumPointerConstPointerSlot(const Enum * const *))));

    QVERIFY(object.connect(&object,
                           SIGNAL(constStructPointerConstPointerSignal(struct Struct const * const *)),
                           SLOT(constStructPointerConstPointerSlot(struct Struct const * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constStructPointerConstPointerSignal(Struct const * const *)),
                           SLOT(constStructPointerConstPointerSlot(struct Struct const * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constStructPointerConstPointerSignal(struct Struct const * const *)),
                           SLOT(constStructPointerConstPointerSlot(Struct const * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constClassPointerConstPointerSignal(class Class const * const *)),
                           SLOT(constClassPointerConstPointerSlot(class Class const * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constClassPointerConstPointerSignal(Class const * const *)),
                           SLOT(constClassPointerConstPointerSlot(class Class const * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constClassPointerConstPointerSignal(class Class const * const *)),
                           SLOT(constClassPointerConstPointerSlot(Class const * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constEnumPointerConstPointerSignal(enum Enum const * const *)),
                           SLOT(constEnumPointerConstPointerSlot(enum Enum const * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constEnumPointerConstPointerSignal(Enum const * const *)),
                           SLOT(constEnumPointerConstPointerSlot(enum Enum const * const *))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constEnumPointerConstPointerSignal(enum Enum const * const *)),
                           SLOT(constEnumPointerConstPointerSlot(Enum const * const *))));

    QVERIFY(object.connect(&object,
                           SIGNAL(unsignedintSignal(unsigned int)),
                           SLOT(unsignedintSlot(unsigned int))));
    QVERIFY(object.connect(&object,
                           SIGNAL(unsignedSignal(unsigned)),
                           SLOT(unsignedSlot(unsigned))));
    QVERIFY(object.connect(&object,
                           SIGNAL(unsignedSignal(unsigned)),
                           SLOT(uintSlot(uint))));
    QVERIFY(object.connect(&object,
                           SIGNAL(unsignedlongSignal(unsigned long)),
                           SLOT(unsignedlongSlot(unsigned long))));
    QVERIFY(object.connect(&object,
                           SIGNAL(unsignedlonglongSignal(quint64)),
                           SLOT(unsignedlonglongSlot(quint64))));
    QVERIFY(object.connect(&object,
                           SIGNAL(unsignedlongintSignal(unsigned long int)),
                           SLOT(unsignedlongintSlot(unsigned long int))));
    QVERIFY(object.connect(&object,
                           SIGNAL(unsignedshortSignal(unsigned short)),
                           SLOT(unsignedshortSlot(unsigned short))));
    QVERIFY(object.connect(&object,
                           SIGNAL(unsignedcharSignal(unsigned char)),
                           SLOT(unsignedcharSlot(unsigned char))));

    // connect when original template signature and mixed usage of 'T<C const &> const &',
    // 'const T<const C &> &', and 'T<const C &>'

    QVERIFY(object.connect(&object,
                           SIGNAL(typeRefSignal(Template<Class &> &)),
                           SLOT(typeRefSlot(Template<Class &> &))));

    QVERIFY(object.connect(&object,
                           SIGNAL(constTypeRefSignal(const Template<const Class &> &)),
                           SLOT(constTypeRefSlot(const Template<const Class &> &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constTypeRefSignal(const Template<const Class &> &)),
                           SLOT(constTypeRefSlot(const Template<Class const &> &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constTypeRefSignal(const Template<const Class &> &)),
                           SLOT(constTypeRefSlot(Template<Class const &> const &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constTypeRefSignal(Template<const Class &> const &)),
                           SLOT(constTypeRefSlot(Template<Class const &> const &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constTypeRefSignal(Template<Class const &> const &)),
                           SLOT(constTypeRefSlot(Template<Class const &> const &))));

    QVERIFY(object.connect(&object,
                           SIGNAL(constTypeRefSignal(const Template<const Class &> &)),
                           SLOT(typeConstRefSlot(const Template<const Class &> &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constTypeRefSignal(const Template<const Class &> &)),
                           SLOT(typeConstRefSlot(const Template<Class const &> &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constTypeRefSignal(const Template<const Class &> &)),
                           SLOT(typeConstRefSlot(Template<Class const &> const &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constTypeRefSignal(Template<const Class &> const &)),
                           SLOT(typeConstRefSlot(Template<Class const &> const &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(constTypeRefSignal(Template<Class const &> const &)),
                           SLOT(typeConstRefSlot(Template<Class const &> const &))));

    QVERIFY(object.connect(&object,
                           SIGNAL(typeConstRefSignal(const Template<const Class &> &)),
                           SLOT(constTypeRefSlot(const Template<const Class &> &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(typeConstRefSignal(const Template<const Class &> &)),
                           SLOT(constTypeRefSlot(const Template<Class const &> &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(typeConstRefSignal(const Template<const Class &> &)),
                           SLOT(constTypeRefSlot(Template<Class const &> const &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(typeConstRefSignal(Template<const Class &> const &)),
                           SLOT(constTypeRefSlot(Template<Class const &> const &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(typeConstRefSignal(Template<Class const &> const &)),
                           SLOT(constTypeRefSlot(Template<Class const &> const &))));

    QVERIFY(object.connect(&object,
                           SIGNAL(typeConstRefSignal(const Template<const Class &> &)),
                           SLOT(typeConstRefSlot(const Template<const Class &> &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(typeConstRefSignal(const Template<const Class &> &)),
                           SLOT(typeConstRefSlot(const Template<Class const &> &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(typeConstRefSignal(const Template<const Class &> &)),
                           SLOT(typeConstRefSlot(Template<Class const &> const &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(typeConstRefSignal(Template<const Class &> const &)),
                           SLOT(typeConstRefSlot(Template<Class const &> const &))));
    QVERIFY(object.connect(&object,
                           SIGNAL(typeConstRefSignal(Template<Class const &> const &)),
                           SLOT(typeConstRefSlot(Template<Class const &> const &))));

    QVERIFY(object.connect(&object,
                           SIGNAL(typePointerConstRefSignal(Class*const&)),
                           SLOT(typePointerConstRefSlot(Class*const&))));
    QVERIFY(object.connect(&object,
                           SIGNAL(typePointerConstRefSignal(Class*const&)),
                           SLOT(typePointerConstRefSlot(Class*))));
    QVERIFY(object.connect(&object,
                           SIGNAL(typePointerConstRefSignal(Class*)),
                           SLOT(typePointerConstRefSlot(Class*const&))));
    QVERIFY(object.connect(&object,
                           SIGNAL(typePointerConstRefSignal(Class*)),
                           SLOT(typePointerConstRefSlot(Class*))));

    QVERIFY( connect(&object, SIGNAL(constTemplateSignal1(Template <int>)),
                     &object , SLOT(constTemplateSlot1 (Template<int > )  ) ));
    QVERIFY( connect(&object, SIGNAL(constTemplateSignal1(Template <int>)),
                     &object , SLOT(constTemplateSlot2 (Template<int > )  ) ));
    QVERIFY( connect(&object, SIGNAL(constTemplateSignal2(Template <const int>)),
                     &object , SLOT(constTemplateSlot3(Template<int const > ) ) ));

    //type does not match
    QTest::ignoreMessage(QtWarningMsg, "QObject::connect: Incompatible sender/receiver arguments\n"
                    "        NormalizeObject::constTemplateSignal1(Template<int>) --> NormalizeObject::constTemplateSlot3(Template<const int>)");
    QVERIFY(!connect(&object, SIGNAL(constTemplateSignal1(Template <int>)),
                     &object , SLOT(constTemplateSlot3(Template<int const> ) ) ));
}

class SiblingDeleter : public QObject
{
public:
    inline SiblingDeleter(QObject *sibling, QObject *parent)
        : QObject(parent), sibling(sibling) {}
    inline virtual ~SiblingDeleter() { delete sibling; }

private:
    QPointer<QObject> sibling;
};


void tst_QObject::childDeletesItsSibling()
{
    QObject *commonParent = new QObject(0);
    QPointer<QObject> child = new QObject(0);
    QPointer<QObject> siblingDeleter = new SiblingDeleter(child, commonParent);
    child->setParent(commonParent);
    delete commonParent; // don't crash
    QVERIFY(!child);
    QVERIFY(!siblingDeleter);
}

void tst_QObject::floatProperty()
{
    PropertyObject obj;
    const int idx = obj.metaObject()->indexOfProperty("myFloat");
    QVERIFY(idx > 0);
    QMetaProperty prop = obj.metaObject()->property(idx);
    QVERIFY(prop.isValid());
    QCOMPARE(int(prop.type()), QMetaType::type("float"));
    QVERIFY(!prop.write(&obj, QVariant("Hello")));
    QVERIFY(prop.write(&obj, QVariant::fromValue(128.0f)));
    QVariant v = prop.read(&obj);
    QCOMPARE(v.userType(), int(QMetaType::Float));
    QCOMPARE(qvariant_cast<float>(v), 128.0f);
}

void tst_QObject::qrealProperty()
{
    PropertyObject obj;
    const int idx = obj.metaObject()->indexOfProperty("myQReal");
    QVERIFY(idx > 0);
    QMetaProperty prop = obj.metaObject()->property(idx);
    QVERIFY(prop.isValid());
    QCOMPARE(int(prop.type()), QMetaType::type("qreal"));
    QVERIFY(!prop.write(&obj, QVariant("Hello")));

    QVERIFY(prop.write(&obj, QVariant::fromValue(128.0f)));
    QVariant v = prop.read(&obj);
    QCOMPARE(v.userType(), qMetaTypeId<qreal>());
    QCOMPARE(qvariant_cast<qreal>(v), 128.0);

    QVERIFY(prop.write(&obj, QVariant::fromValue(double(127))));
    v = prop.read(&obj);
    QCOMPARE(v.userType(), qMetaTypeId<qreal>());
    QCOMPARE(qvariant_cast<qreal>(v), 127.0);
}

class DynamicPropertyObject : public PropertyObject
{
public:
    inline DynamicPropertyObject() {}

    inline virtual bool event(QEvent *e) override {
        if (e->type() == QEvent::DynamicPropertyChange) {
            changedDynamicProperties.append(static_cast<QDynamicPropertyChangeEvent *>(e)->propertyName());
        }
        return QObject::event(e);
    }

    QList<QByteArray> changedDynamicProperties;
};

void tst_QObject::dynamicProperties()
{
    DynamicPropertyObject obj;

    QVERIFY(obj.dynamicPropertyNames().isEmpty());

    // set a non-dynamic property
    QVERIFY(obj.setProperty("number", 42));
    QVERIFY(obj.changedDynamicProperties.isEmpty());
    QCOMPARE(obj.property("number").toInt(), 42);

    QVERIFY(!obj.setProperty("number", "invalid string"));
    QVERIFY(obj.changedDynamicProperties.isEmpty());

    // set a dynamic property
    QVERIFY(!obj.setProperty("myuserproperty", "Hello"));
    QCOMPARE(obj.changedDynamicProperties.count(), 1);
    QCOMPARE(obj.changedDynamicProperties.first(), QByteArray("myuserproperty"));
    //check if there is no redundant DynamicPropertyChange events
    QVERIFY(!obj.setProperty("myuserproperty", "Hello"));
    QCOMPARE(obj.changedDynamicProperties.count(), 1);

    QCOMPARE(obj.property("myuserproperty").type(), QVariant::String);
    QCOMPARE(obj.property("myuserproperty").toString(), QString("Hello"));

    QCOMPARE(obj.dynamicPropertyNames().count(), 1);
    QCOMPARE(obj.dynamicPropertyNames().first(), QByteArray("myuserproperty"));

    // change type of the dynamic property
    obj.changedDynamicProperties.clear();
    QVERIFY(!obj.setProperty("myuserproperty", QByteArray("Hello")));
    QCOMPARE(obj.changedDynamicProperties.count(), 1);
    QCOMPARE(obj.changedDynamicProperties.first(), QByteArray("myuserproperty"));
    QCOMPARE(obj.property("myuserproperty").type(), QVariant::ByteArray);
    QCOMPARE(obj.property("myuserproperty").toString(), QByteArray("Hello"));

    // unset the property
    obj.changedDynamicProperties.clear();
    QVERIFY(!obj.setProperty("myuserproperty", QVariant()));

    QCOMPARE(obj.changedDynamicProperties.count(), 1);
    QCOMPARE(obj.changedDynamicProperties.first(), QByteArray("myuserproperty"));
    obj.changedDynamicProperties.clear();

    QVERIFY(obj.property("myuserproperty").isNull());

    QVERIFY(obj.dynamicPropertyNames().isEmpty());
}

void tst_QObject::recursiveSignalEmission()
{
#if !QT_CONFIG(process)
    QSKIP("No qprocess support", SkipAll);
#elif !defined(Q_OS_MAC) && !defined(Q_OS_WIN) // because it does not find the executable
    QProcess proc;
    // signalbug helper app should always be next to this test binary
#ifdef W_SIGNALBUG
    const QString path = QStringLiteral(W_SIGNALBUG);
#else
    const QString path = QStringLiteral("signalbug/signalbug");
#endif
    proc.start(path);
    QVERIFY2(proc.waitForStarted(), qPrintable(QString::fromLatin1("Cannot start '%1': %2").arg(path, proc.errorString())));
    QVERIFY(proc.waitForFinished());
    QCOMPARE(proc.exitStatus(), QProcess::NormalExit);
    QCOMPARE(proc.exitCode(), 0);
#endif
}

void tst_QObject::signalBlocking()
{
    SenderObject sender;
    ReceiverObject receiver;

    receiver.connect(&sender, SIGNAL(signal1()), SLOT(slot1()));

    sender.emitSignal1();
    QVERIFY(receiver.called(1));
    receiver.reset();

    sender.blockSignals(true);

    sender.emitSignal1();
    QVERIFY(!receiver.called(1));
    receiver.reset();

    sender.blockSignals(false);

    sender.emitSignal1();
    QVERIFY(receiver.called(1));
    receiver.reset();
}

void tst_QObject::blockingQueuedConnection()
{
    {
        SenderObject sender;

        MoveToThreadThread thread;
        ReceiverObject receiver;
        receiver.moveToThread(&thread);
        thread.start();

        receiver.connect(&sender, SIGNAL(signal1()), SLOT(slot1()), Qt::BlockingQueuedConnection);
        sender.emitSignal1();
        QVERIFY(receiver.called(1));

        receiver.reset();
        QVERIFY(QMetaObject::invokeMethod(&receiver, "slot1", Qt::BlockingQueuedConnection));
        QVERIFY(receiver.called(1));

        connect(&sender, &SenderObject::signal2, &receiver, &ReceiverObject::slot2, Qt::BlockingQueuedConnection);
        sender.emitSignal2();
        QVERIFY(receiver.called(2));

        thread.quit();
        QVERIFY(thread.wait());
    }
}

class EventSpy : public QObject
{
    W_OBJECT(EventSpy)

public:
    typedef QList<QPair<QObject *, QEvent::Type> > EventList;

    EventSpy(QObject *parent = nullptr)
        : QObject(parent)
    { }

    EventList eventList()
    {
        return events;
    }

    void clear()
    {
        events.clear();
    }

    bool eventFilter(QObject *object, QEvent *event) override
    {
        events.append(qMakePair(object, event->type()));
        return false;
    }

private:
    EventList events;
};

void tst_QObject::childEvents()
{
    EventSpy::EventList expected;

    {
        // no children created, so we expect no events
        QObject object;
        EventSpy spy;
        object.installEventFilter(&spy);

        QCoreApplication::postEvent(&object, new QEvent(QEvent::Type(QEvent::User + 1)));

        QCoreApplication::processEvents();

        expected =
            EventSpy::EventList()
            << qMakePair(&object, QEvent::Type(QEvent::User + 1));
        QCOMPARE(spy.eventList(), expected);
    }

    {
        // 2 children, so we expect 2 ChildAdded events
        QObject object;
        EventSpy spy;
        object.installEventFilter(&spy);

        QCoreApplication::postEvent(&object, new QEvent(QEvent::Type(QEvent::User + 1)));

        QObject child1(&object);
        QObject child2;
        child2.setParent(&object);

        QCoreApplication::postEvent(&object, new QEvent(QEvent::Type(QEvent::User + 2)));

        expected =
            EventSpy::EventList()
            << qMakePair(&object, QEvent::ChildAdded)
            << qMakePair(&object, QEvent::ChildAdded);
        QCOMPARE(spy.eventList(), expected);
        spy.clear();

        QCoreApplication::processEvents();

        expected =
            EventSpy::EventList()
            << qMakePair(&object, QEvent::Type(QEvent::User + 1))
            << qMakePair(&object, QEvent::Type(QEvent::User + 2));
        QCOMPARE(spy.eventList(), expected);
    }

    {
        // 2 children, but one is reparented away, so we expect:
        // 2 ChildAdded, 1 ChildRemoved
        QObject object;
        EventSpy spy;
        object.installEventFilter(&spy);

        QCoreApplication::postEvent(&object, new QEvent(QEvent::Type(QEvent::User + 1)));

        QObject child1(&object);
        QObject child2;
        child2.setParent(&object);
        child2.setParent(0);

        QCoreApplication::postEvent(&object, new QEvent(QEvent::Type(QEvent::User + 2)));

        expected =
            EventSpy::EventList()
            << qMakePair(&object, QEvent::ChildAdded)
            << qMakePair(&object, QEvent::ChildAdded)
            << qMakePair(&object, QEvent::ChildRemoved);
        QCOMPARE(spy.eventList(), expected);
        spy.clear();

        QCoreApplication::processEvents();

        expected =
            EventSpy::EventList()
            << qMakePair(&object, QEvent::Type(QEvent::User + 1))
            << qMakePair(&object, QEvent::Type(QEvent::User + 2));
        QCOMPARE(spy.eventList(), expected);
    }
}

void tst_QObject::installEventFilter()
{
    QEvent event(QEvent::User);
    EventSpy::EventList expected;

    QObject object;
    EventSpy spy;
    object.installEventFilter(&spy);

    // nothing special, should just work
    QCoreApplication::sendEvent(&object, &event);
    expected =
        EventSpy::EventList()
        << qMakePair(&object, QEvent::User);
    QCOMPARE(spy.eventList(), expected);
    spy.clear();

    // moving the filter causes QCoreApplication to skip the filter
    spy.moveToThread(0);
    QTest::ignoreMessage(QtWarningMsg, "QCoreApplication: Object event filter cannot be in a different thread.");
    QCoreApplication::sendEvent(&object, &event);
    QVERIFY(spy.eventList().isEmpty());

    // move it back, and the filter works again
    spy.moveToThread(object.thread());
    QCoreApplication::sendEvent(&object, &event);
    expected =
        EventSpy::EventList()
        << qMakePair(&object, QEvent::User);
    QCOMPARE(spy.eventList(), expected);
    spy.clear();

    // cannot install an event filter that lives in a different thread
    object.removeEventFilter(&spy);
    spy.moveToThread(0);
    QTest::ignoreMessage(QtWarningMsg, "QObject::installEventFilter(): Cannot filter events for objects in a different thread.");
    object.installEventFilter(&spy);
    QCoreApplication::sendEvent(&object, &event);
    QVERIFY(spy.eventList().isEmpty());
}

class EmitThread : public QThread
{   W_OBJECT(EmitThread)
public:
    void run(void) override {
        emit work();
    }
signals:
    void work() W_SIGNAL(work)
};

namespace QObjectTest { // Do not clash with WinAPI 'DeleteObject'
class DeleteObject : public QObject
{
    W_OBJECT(DeleteObject)

public slots:
    void deleteSelf()
    {
        delete this;
    }
    W_SLOT(deleteSelf)

    void relaySignalAndProcessEvents()
    {
        emit relayedSignal();
        QCoreApplication::processEvents();
    }
    W_SLOT(relaySignalAndProcessEvents)

signals:
    void relayedSignal() W_SIGNAL(relayedSignal)
};
} // namespace QObjectTest

void tst_QObject::deleteSelfInSlot()
{
    {
        SenderObject sender;
        QObjectTest::DeleteObject *receiver = new QObjectTest::DeleteObject();
        receiver->connect(&sender,
                          SIGNAL(signal1()),
                          SLOT(deleteSelf()),
                          Qt::BlockingQueuedConnection);

        QThread thread;
        receiver->moveToThread(&thread);
        thread.connect(receiver, SIGNAL(destroyed()), SLOT(quit()), Qt::DirectConnection);
        thread.start();

        QPointer<QObjectTest::DeleteObject> p = receiver;
        sender.emitSignal1();
        QVERIFY(p.isNull());

        QVERIFY(thread.wait(10000));
    }

    {
        SenderObject sender;
        QObjectTest::DeleteObject *receiver = new QObjectTest::DeleteObject();
        receiver->connect(&sender,
                          SIGNAL(signal1()),
                          SLOT(relaySignalAndProcessEvents()),
                          Qt::BlockingQueuedConnection);
        receiver->connect(receiver,
                          SIGNAL(relayedSignal()),
                          SLOT(deleteSelf()),
                          Qt::QueuedConnection);

        QThread thread;
        receiver->moveToThread(&thread);
        thread.connect(receiver, SIGNAL(destroyed()), SLOT(quit()), Qt::DirectConnection);
        thread.start();

        QPointer<QObjectTest::DeleteObject> p = receiver;
        sender.emitSignal1();
        QVERIFY(p.isNull());

        QVERIFY(thread.wait(10000));
    }

    {
        EmitThread sender;
        QObjectTest::DeleteObject *receiver = new QObjectTest::DeleteObject();
        connect(&sender, SIGNAL(work()), receiver, SLOT(deleteSelf()), Qt::DirectConnection);
        QPointer<QObjectTest::DeleteObject> p = receiver;
        sender.start();
        QVERIFY(sender.wait(10000));
        QVERIFY(p.isNull());
    }
}

class DisconnectObject : public QObject
{
    W_OBJECT(DisconnectObject)

public slots:
    void disconnectSelf()
    {
        disconnect(sender(), 0, this, 0);
    }
    W_SLOT(disconnectSelf)

    void relaySignalAndProcessEvents()
    {
        emit relayedSignal();
        QCoreApplication::processEvents();
    }
    W_SLOT(relaySignalAndProcessEvents)

signals:
    void relayedSignal() W_SIGNAL(relayedSignal)
};

void tst_QObject::disconnectSelfInSlotAndDeleteAfterEmit()
{
    {
        SenderObject sender;
        DisconnectObject *receiver = new DisconnectObject();
        receiver->connect(&sender, SIGNAL(signal1()), SLOT(disconnectSelf()));
        sender.emitSignal1AfterRecursion();
        delete receiver;
    }

    {
        SenderObject sender;
        DisconnectObject *receiver = new DisconnectObject();
        receiver->connect(&sender,
                          SIGNAL(signal1()),
                          SLOT(disconnectSelf()),
                          Qt::BlockingQueuedConnection);

        QThread thread;
        receiver->moveToThread(&thread);
        thread.connect(receiver, SIGNAL(destroyed()), SLOT(quit()), Qt::DirectConnection);
        thread.start();

        QPointer<DisconnectObject> p = receiver;
        sender.emitSignal1();
        QVERIFY(!p.isNull());

        receiver->deleteLater();

        QVERIFY(thread.wait(10000));
        QVERIFY(p.isNull());
    }

    {
        SenderObject sender;
        DisconnectObject *receiver = new DisconnectObject();
        receiver->connect(&sender,
                          SIGNAL(signal1()),
                          SLOT(relaySignalAndProcessEvents()),
                          Qt::BlockingQueuedConnection);
        receiver->connect(receiver,
                          SIGNAL(relayedSignal()),
                          SLOT(disconnectSelf()),
                          Qt::QueuedConnection);

        QThread thread;
        receiver->moveToThread(&thread);
        thread.connect(receiver, SIGNAL(destroyed()), SLOT(quit()), Qt::DirectConnection);
        thread.start();

        QPointer<DisconnectObject> p = receiver;
        sender.emitSignal1();
        QVERIFY(!p.isNull());

        receiver->deleteLater();

        QVERIFY(thread.wait(10000));
        QVERIFY(p.isNull());
    }
}

void tst_QObject::dumpObjectInfo()
{
    QObject a, b;
    QObject::connect(&a, &QObject::destroyed, &b, &QObject::deleteLater);
    QTest::ignoreMessage(QtDebugMsg, "OBJECT QObject::unnamed");
    QTest::ignoreMessage(QtDebugMsg, "  SIGNALS OUT");
    QTest::ignoreMessage(QtDebugMsg, "        signal: destroyed(QObject*)");
    QTest::ignoreMessage(QtDebugMsg, "          <functor or function pointer>");
    QTest::ignoreMessage(QtDebugMsg, "  SIGNALS IN");
    QTest::ignoreMessage(QtDebugMsg, "        <None>");
    a.dumpObjectInfo(); // should not crash
}

class ConnectToSender : public QObject
{ W_OBJECT(ConnectToSender)
    public slots:
        void uselessSlot() { count++; }
    W_SLOT(uselessSlot)

        void harmfullSlot() {
            //this used to crash
            connect(sender(), SIGNAL(signal4()), this, SLOT(uselessSlot()));
            //play a little bit with the memory in order to really get a segfault.
            connect(sender(), SIGNAL(signal1()), this, SLOT(uselessSlot()));
            QList<double>() << 45 << 78 << 65 << 121 << 45 << 78 << 12;
        }
        W_SLOT(harmfullSlot)
    public:
        int count;
};

void tst_QObject::connectToSender()
{
    SenderObject s;
    ConnectToSender r;
    r.count = 0;
    QObject::connect(&s, SIGNAL(signal1()), &r, SLOT(harmfullSlot()));
    QObject::connect(&s, SIGNAL(signal1()), &r, SLOT(uselessSlot()));

    s.emitSignal1();

    QCOMPARE(r.count, 1);
    s.emitSignal4();
    QCOMPARE(r.count, 2);
}

void tst_QObject::qobjectConstCast()
{
    FooObject obj;

    QObject *ptr = &obj;
    const QObject *cptr = &obj;

    QVERIFY(qobject_cast<FooObject *>(ptr));
    QVERIFY(qobject_cast<const FooObject *>(cptr));
}

void tst_QObject::uniqConnection()
{
    SenderObject s;
    ReceiverObject r1;
    ReceiverObject r2;
    r1.reset();
    r2.reset();
    ReceiverObject::sequence = 0;

    QVERIFY(connect(&s, SIGNAL(signal1()), &r1, SLOT(slot1()) , Qt::UniqueConnection) );
    QVERIFY(connect(&s, SIGNAL(signal1()), &r2, SLOT(slot1()) , Qt::UniqueConnection) );
    QVERIFY(connect(&s, SIGNAL(signal1()), &r1, SLOT(slot3()) , Qt::UniqueConnection) );
    QVERIFY(connect(&s, SIGNAL(signal3()), &r1, SLOT(slot3()) , Qt::UniqueConnection) );

    s.emitSignal1();
    s.emitSignal2();
    s.emitSignal3();
    s.emitSignal4();

    QCOMPARE(r1.count_slot1, 1);
    QCOMPARE(r1.count_slot2, 0);
    QCOMPARE(r1.count_slot3, 2);
    QCOMPARE(r1.count_slot4, 0);
    QCOMPARE(r2.count_slot1, 1);
    QCOMPARE(r2.count_slot2, 0);
    QCOMPARE(r2.count_slot3, 0);
    QCOMPARE(r2.count_slot4, 0);
    QCOMPARE(r1.sequence_slot1, 1);
    QCOMPARE(r2.sequence_slot1, 2);
    QCOMPARE(r1.sequence_slot3, 4);

    r1.reset();
    r2.reset();
    ReceiverObject::sequence = 0;

    QVERIFY( connect(&s, SIGNAL(signal4()), &r1, SLOT(slot4()) , Qt::UniqueConnection));
    QVERIFY( connect(&s, SIGNAL(signal4()), &r2, SLOT(slot4()) , Qt::UniqueConnection));
    QVERIFY(!connect(&s, SIGNAL(signal4()), &r2, SLOT(slot4()) , Qt::UniqueConnection));
    QVERIFY( connect(&s, SIGNAL(signal1()), &r2, SLOT(slot4()) , Qt::UniqueConnection));
    QVERIFY(!connect(&s, SIGNAL(signal4()), &r1, SLOT(slot4()) , Qt::UniqueConnection));

    s.emitSignal4();
    QCOMPARE(r1.count_slot4, 1);
    QCOMPARE(r2.count_slot4, 1);
    QCOMPARE(r1.sequence_slot4, 1);
    QCOMPARE(r2.sequence_slot4, 2);

    r1.reset();
    r2.reset();
    ReceiverObject::sequence = 0;

    connect(&s, SIGNAL(signal4()), &r1, SLOT(slot4()));

    s.emitSignal4();
    QCOMPARE(r1.count_slot4, 2);
    QCOMPARE(r2.count_slot4, 1);
    QCOMPARE(r1.sequence_slot4, 3);
    QCOMPARE(r2.sequence_slot4, 2);
}

void tst_QObject::uniqConnectionPtr()
{
    SenderObject s;
    ReceiverObject r1;
    ReceiverObject r2;
    r1.reset();
    r2.reset();
    ReceiverObject::sequence = 0;

    QVERIFY(connect(&s, &SenderObject::signal1, &r1, &ReceiverObject::slot1 ,
                    Qt::UniqueConnection));
    QVERIFY(connect(&s, &SenderObject::signal1, &r2, &ReceiverObject::slot1 ,
                    Qt::UniqueConnection));
    QVERIFY(connect(&s, &SenderObject::signal1, &r1, &ReceiverObject::slot3 ,
                    Qt::UniqueConnection));
    QVERIFY(connect(&s, &SenderObject::signal3, &r1, &ReceiverObject::slot3 ,
                    Qt::UniqueConnection));

    s.emitSignal1();
    s.emitSignal2();
    s.emitSignal3();
    s.emitSignal4();

    QCOMPARE(r1.count_slot1, 1);
    QCOMPARE(r1.count_slot2, 0);
    QCOMPARE(r1.count_slot3, 2);
    QCOMPARE(r1.count_slot4, 0);
    QCOMPARE(r2.count_slot1, 1);
    QCOMPARE(r2.count_slot2, 0);
    QCOMPARE(r2.count_slot3, 0);
    QCOMPARE(r2.count_slot4, 0);
    QCOMPARE(r1.sequence_slot1, 1);
    QCOMPARE(r2.sequence_slot1, 2);
    QCOMPARE(r1.sequence_slot3, 4);

    r1.reset();
    r2.reset();
    ReceiverObject::sequence = 0;

    QVERIFY( connect(&s, &SenderObject::signal4, &r1, &ReceiverObject::slot4 ,
                     Qt::UniqueConnection));
    QVERIFY( connect(&s, &SenderObject::signal4, &r2, &ReceiverObject::slot4 ,
                     Qt::UniqueConnection));
    QVERIFY(!connect(&s, &SenderObject::signal4, &r2, &ReceiverObject::slot4 ,
                     Qt::UniqueConnection));
    QVERIFY( connect(&s, &SenderObject::signal1, &r2, &ReceiverObject::slot4 ,
                     Qt::UniqueConnection));
    QVERIFY(!connect(&s, &SenderObject::signal4, &r1, &ReceiverObject::slot4 ,
                     Qt::UniqueConnection));

    s.emitSignal4();
    QCOMPARE(r1.count_slot4, 1);
    QCOMPARE(r2.count_slot4, 1);
    QCOMPARE(r1.sequence_slot4, 1);
    QCOMPARE(r2.sequence_slot4, 2);

    r1.reset();
    r2.reset();
    ReceiverObject::sequence = 0;

    connect(&s, &SenderObject::signal4, &r1, &ReceiverObject::slot4);

    s.emitSignal4();
    QCOMPARE(r1.count_slot4, 2);
    QCOMPARE(r2.count_slot4, 1);
    QCOMPARE(r1.sequence_slot4, 3);
    QCOMPARE(r2.sequence_slot4, 2);
}

void tst_QObject::interfaceIid()
{
    QCOMPARE(QByteArray(qobject_interface_iid<Foo::Bleh *>()),
             QByteArray(Bleh_iid));
    QCOMPARE(QByteArray(qobject_interface_iid<Foo::Bar *>()),
             QByteArray("com.qtest.foobar"));
    QCOMPARE(QByteArray(qobject_interface_iid<FooObject *>()),
             QByteArray());
}

void tst_QObject::deleteQObjectWhenDeletingEvent()
{
    // This is a regression test for an old bug that used to deadlock
    // when the QObject from the event was destroyed.

    struct MyEvent : public QEvent
    {
        MyEvent() : QEvent(QEvent::User) { }
        QObject obj;
    };

    QObject o;
    QCoreApplication::postEvent(&o, new MyEvent);
    QCoreApplication::removePostedEvents(&o); // here you would get a deadlock
}

class OverloadObject : public QObject
{
    friend class tst_QObject;
    W_OBJECT(OverloadObject)
    signals:
        void sig(int i, char c, qreal m = 12) W_SIGNAL(sig,(int, char, qreal),i,c,m)
        void sig(int i, int j = 12) W_SIGNAL(sig,(int,int),i,j)
        void sig(QObject *o, QObject *p, QObject *q = 0, QObject *r = 0) const
            W_SIGNAL(sig,(QObject*,QObject*,QObject*,QObject*),o,p,q,r)
        void other(int a = 0) W_SIGNAL(other,(int),a)
        void sig(QObject *o, OverloadObject *p = 0, QObject *q = 0, QObject *r = 0)
            W_SIGNAL(sig,(QObject*,OverloadObject*,QObject*,QObject*),o,p,q,r)
        void sig(double r = 0.5) W_SIGNAL(sig,(double),r)
    public slots:
        void slo(int i, int j = 43)
        {
            s_num += 1;
            i1_num = i;
            i2_num = j;
        }
        W_SLOT(slo,(int,int))
        void slo(QObject *o, QObject *p = qApp, QObject *q = qApp, QObject *r = qApp)
        {
            s_num += 10;
            o1_obj = o;
            o2_obj = p;
            o3_obj = q;
            o4_obj = r;
        }
        W_SLOT(slo,(QObject*,QObject*,QObject*,QObject*))
        void slo()
        {
            s_num += 100;
        }
        W_SLOT(slo,())

    public:
        int s_num;
        int i1_num;
        int i2_num;
        QObject *o1_obj;
        QObject *o2_obj;
        QObject *o3_obj;
        QObject *o4_obj;
};

void tst_QObject::overloads()
{
    QSKIP("Overload not supported by W_SIGNAL/W_SLOT");
    OverloadObject obj1;
    OverloadObject obj2;
    QObject obj3;
    obj1.s_num = 0;
    obj2.s_num = 0;

    connect (&obj1, SIGNAL(sig(int)) , &obj1, SLOT(slo(int)));
    connect (&obj1, SIGNAL(sig(QObject*,QObject*,QObject*)) , &obj1, SLOT(slo(QObject*,QObject*,QObject*)));

    connect (&obj1, SIGNAL(sig(QObject*,QObject*,QObject*,QObject*)) , &obj2, SLOT(slo(QObject*,QObject*,QObject*)));
    connect (&obj1, SIGNAL(sig(QObject*)) , &obj2, SLOT(slo()));
    connect (&obj1, SIGNAL(sig(int,int)) , &obj2, SLOT(slo(int,int)));

    emit obj1.sig(0.5); //connected to nothing
    emit obj1.sig(1, 'a'); //connected to nothing
    QCOMPARE(obj1.s_num, 0);
    QCOMPARE(obj2.s_num, 0);

    emit obj1.sig(1); //this signal is connected
    QCOMPARE(obj1.s_num, 1);
    QCOMPARE(obj1.i1_num, 1);
    QCOMPARE(obj1.i2_num, 43); //default argument of the slot

    QCOMPARE(obj2.s_num, 1);
    QCOMPARE(obj2.i1_num, 1);
    QCOMPARE(obj2.i2_num, 12); //default argument of the signal


    emit obj1.sig(&obj2); //this signal is conencted to obj2
    QCOMPARE(obj1.s_num, 1);
    QCOMPARE(obj2.s_num, 101);
    emit obj1.sig(&obj2, &obj3); //this signal is connected
    QCOMPARE(obj1.s_num, 11);
    QCOMPARE(obj1.o1_obj, (QObject *)&obj2);
    QCOMPARE(obj1.o2_obj, &obj3);
    QCOMPARE(obj1.o3_obj, (QObject *)0); //default arg of the signal
    QCOMPARE(obj1.o4_obj, (QObject *)qApp); //default arg of the slot

    QCOMPARE(obj2.s_num, 111);
    QCOMPARE(obj2.o1_obj, (QObject *)&obj2);
    QCOMPARE(obj2.o2_obj, &obj3);
    QCOMPARE(obj2.o3_obj, (QObject *)0); //default arg of the signal
    QCOMPARE(obj2.o4_obj, (QObject *)qApp); //default arg of the slot
}

class ManySignals : public QObject
{   W_OBJECT(ManySignals)
    friend class tst_QObject;
signals:
    void sig00() W_SIGNAL(sig00) void sig01() W_SIGNAL(sig01) void sig02() W_SIGNAL(sig02) void sig03() W_SIGNAL(sig03) void sig04() W_SIGNAL(sig04)
    void sig05() W_SIGNAL(sig05) void sig06() W_SIGNAL(sig06) void sig07() W_SIGNAL(sig07) void sig08() W_SIGNAL(sig08) void sig09() W_SIGNAL(sig09)
    void sig10() W_SIGNAL(sig10) void sig11() W_SIGNAL(sig11) void sig12() W_SIGNAL(sig12) void sig13() W_SIGNAL(sig13) void sig14() W_SIGNAL(sig14)
    void sig15() W_SIGNAL(sig15) void sig16() W_SIGNAL(sig16) void sig17() W_SIGNAL(sig17) void sig18() W_SIGNAL(sig18) void sig19() W_SIGNAL(sig19)
    void sig20() W_SIGNAL(sig20) void sig21() W_SIGNAL(sig21) void sig22() W_SIGNAL(sig22) void sig23() W_SIGNAL(sig23) void sig24() W_SIGNAL(sig24)
    void sig25() W_SIGNAL(sig25) void sig26() W_SIGNAL(sig26) void sig27() W_SIGNAL(sig27) void sig28() W_SIGNAL(sig28) void sig29() W_SIGNAL(sig29)
    void sig30() W_SIGNAL(sig30) void sig31() W_SIGNAL(sig31) void sig32() W_SIGNAL(sig32) void sig33() W_SIGNAL(sig33) void sig34() W_SIGNAL(sig34)
    void sig35() W_SIGNAL(sig35) void sig36() W_SIGNAL(sig36) void sig37() W_SIGNAL(sig37) void sig38() W_SIGNAL(sig38) void sig39() W_SIGNAL(sig39)
    void sig40() W_SIGNAL(sig40) void sig41() W_SIGNAL(sig41) void sig42() W_SIGNAL(sig42) void sig43() W_SIGNAL(sig43) void sig44() W_SIGNAL(sig44)
    void sig45() W_SIGNAL(sig45) void sig46() W_SIGNAL(sig46) void sig47() W_SIGNAL(sig47) void sig48() W_SIGNAL(sig48) void sig49() W_SIGNAL(sig49)
    void sig50() W_SIGNAL(sig50) void sig51() W_SIGNAL(sig51) void sig52() W_SIGNAL(sig52) void sig53() W_SIGNAL(sig53) void sig54() W_SIGNAL(sig54)
    void sig55() W_SIGNAL(sig55) void sig56() W_SIGNAL(sig56) void sig57() W_SIGNAL(sig57) void sig58() W_SIGNAL(sig58) void sig59() W_SIGNAL(sig59)
    void sig60() W_SIGNAL(sig60) void sig61() W_SIGNAL(sig61) void sig62() W_SIGNAL(sig62) void sig63() W_SIGNAL(sig63) void sig64() W_SIGNAL(sig64)
    void sig65() W_SIGNAL(sig65) void sig66() W_SIGNAL(sig66) void sig67() W_SIGNAL(sig67) void sig68() W_SIGNAL(sig68) void sig69() W_SIGNAL(sig69)

public slots:
    void received() { rec++; }
    W_SLOT(received)
public:
    int rec;
};


void tst_QObject::isSignalConnected()
{
    ManySignals o;
    const QMetaObject *meta = o.metaObject();
    o.rec = 0;
#ifdef QT_BUILD_INTERNAL
    QObjectPrivate *priv = QObjectPrivate::get(&o);
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("destroyed()")));
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("sig00()")));
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("sig05()")));
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("sig15()")));
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("sig29()")));
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("sig60()")));
#endif
    QVERIFY(!o.isSignalConnected(meta->method(meta->indexOfSignal("destroyed()"))));
    QVERIFY(!o.isSignalConnected(meta->method(meta->indexOfSignal("sig00()"))));
    QVERIFY(!o.isSignalConnected(meta->method(meta->indexOfSignal("sig05()"))));
    QVERIFY(!o.isSignalConnected(meta->method(meta->indexOfSignal("sig15()"))));
    QVERIFY(!o.isSignalConnected(meta->method(meta->indexOfSignal("sig29()"))));
    QVERIFY(!o.isSignalConnected(meta->method(meta->indexOfSignal("sig60()"))));
    QVERIFY(!o.isSignalConnected(meta->method(meta->indexOfSignal("sig69()"))));

    QObject::connect(&o, SIGNAL(sig00()), &o, SIGNAL(sig69()));
    QObject::connect(&o, SIGNAL(sig34()), &o, SIGNAL(sig03()));
    QObject::connect(&o, SIGNAL(sig69()), &o, SIGNAL(sig34()));
    QObject::connect(&o, SIGNAL(sig03()), &o, SIGNAL(sig18()));

#ifdef QT_BUILD_INTERNAL
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("destroyed()")));
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("sig05()")));
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("sig15()")));
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("sig29()")));

    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig00()")));
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig03()")));
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig34()")));
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig69()")));
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("sig18()")));
#endif
    QVERIFY(!o.isSignalConnected(meta->method(meta->indexOfSignal("destroyed()"))));
    QVERIFY(!o.isSignalConnected(meta->method(meta->indexOfSignal("destroyed(QObject*)"))));
    QVERIFY(!o.isSignalConnected(meta->method(meta->indexOfSignal("sig05()"))));
    QVERIFY(!o.isSignalConnected(meta->method(meta->indexOfSignal("sig15()"))));
    QVERIFY(!o.isSignalConnected(meta->method(meta->indexOfSignal("sig29()"))));

    QVERIFY(o.isSignalConnected(meta->method(meta->indexOfSignal("sig00()"))));
    QVERIFY(o.isSignalConnected(meta->method(meta->indexOfSignal("sig03()"))));
    QVERIFY(o.isSignalConnected(meta->method(meta->indexOfSignal("sig34()"))));
    QVERIFY(o.isSignalConnected(meta->method(meta->indexOfSignal("sig69()"))));
    QVERIFY(!o.isSignalConnected(meta->method(meta->indexOfSignal("sig18()"))));


    QObject::connect(&o, SIGNAL(sig18()), &o, SIGNAL(sig29()));
    QObject::connect(&o, SIGNAL(sig29()), &o, SIGNAL(sig62()));
    QObject::connect(&o, SIGNAL(sig62()), &o, SIGNAL(sig28()));
    QObject::connect(&o, SIGNAL(sig28()), &o, SIGNAL(sig27()));

#ifdef QT_BUILD_INTERNAL
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig18()")));
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig62()")));
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig28()")));
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig69()")));
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("sig27()")));
#endif
    QVERIFY(o.isSignalConnected(meta->method(meta->indexOfSignal("sig18()"))));
    QVERIFY(o.isSignalConnected(meta->method(meta->indexOfSignal("sig62()"))));
    QVERIFY(o.isSignalConnected(meta->method(meta->indexOfSignal("sig28()"))));
    QVERIFY(o.isSignalConnected(meta->method(meta->indexOfSignal("sig69()"))));
    QVERIFY(!o.isSignalConnected(meta->method(meta->indexOfSignal("sig27()"))));

    QCOMPARE(o.rec, 0);
    emit o.sig01();
    emit o.sig34();
    QCOMPARE(o.rec, 0);

    QObject::connect(&o, SIGNAL(sig27()), &o, SLOT(received()));

#ifdef QT_BUILD_INTERNAL
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig00()")));
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig03()")));
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig34()")));
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig18()")));
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig62()")));
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig28()")));
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig69()")));
    QVERIFY(priv->isSignalConnected(priv->signalIndex("sig27()")));

    QVERIFY(!priv->isSignalConnected(priv->signalIndex("sig04()")));
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("sig21()")));
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("sig25()")));
    QVERIFY(!priv->isSignalConnected(priv->signalIndex("sig55()")));
#endif

    emit o.sig00();
    QCOMPARE(o.rec, 1);
    emit o.sig69();
    QCOMPARE(o.rec, 2);
    emit o.sig36();
    QCOMPARE(o.rec, 2);

    QObject::connect(&o, &QObject::destroyed, qt_noop);
    QVERIFY(o.isSignalConnected(meta->method(meta->indexOfSignal("destroyed()"))));
    QVERIFY(o.isSignalConnected(meta->method(meta->indexOfSignal("destroyed(QObject*)"))));

    QVERIFY(!o.isSignalConnected(QMetaMethod()));
}

void tst_QObject::isSignalConnectedAfterDisconnection()
{
    ManySignals o;
    const QMetaObject *meta = o.metaObject();

    const QMetaMethod sig00 = meta->method(meta->indexOfSignal("sig00()"));
    QVERIFY(!o.isSignalConnected(sig00));
    QObject::connect(&o, &ManySignals::sig00, qt_noop);
    QVERIFY(o.isSignalConnected(sig00));
    QVERIFY(QObject::disconnect(&o, &ManySignals::sig00, 0, 0));
    QVERIFY(!o.isSignalConnected(sig00));

    const QMetaMethod sig69 = meta->method(meta->indexOfSignal("sig69()"));
    QVERIFY(!o.isSignalConnected(sig69));
    QObject::connect(&o, &ManySignals::sig69, qt_noop);
    QVERIFY(o.isSignalConnected(sig69));
    QVERIFY(QObject::disconnect(&o, &ManySignals::sig69, 0, 0));
    QVERIFY(!o.isSignalConnected(sig69));

    {
        ManySignals o2;
        QObject::connect(&o, &ManySignals::sig00, &o2, &ManySignals::sig00);
        QVERIFY(o.isSignalConnected(sig00));
        // o2 is destructed
    }
    QVERIFY(!o.isSignalConnected(sig00));

    const QMetaMethod sig01 = meta->method(meta->indexOfSignal("sig01()"));
    QObject::connect(&o, &ManySignals::sig00, qt_noop);
    QObject::connect(&o, &ManySignals::sig01, qt_noop);
    QObject::connect(&o, &ManySignals::sig69, qt_noop);
    QVERIFY(o.isSignalConnected(sig00));
    QVERIFY(o.isSignalConnected(sig01));
    QVERIFY(o.isSignalConnected(sig69));
    QVERIFY(QObject::disconnect(&o, &ManySignals::sig69, 0, 0));
    QVERIFY(o.isSignalConnected(sig00));
    QVERIFY(o.isSignalConnected(sig01));
    QVERIFY(!o.isSignalConnected(sig69));
    QVERIFY(QObject::disconnect(&o, &ManySignals::sig00, 0, 0));
    QVERIFY(!o.isSignalConnected(sig00));
    QVERIFY(o.isSignalConnected(sig01));
    QVERIFY(!o.isSignalConnected(sig69));
    QObject::connect(&o, &ManySignals::sig69, qt_noop);
    QVERIFY(!o.isSignalConnected(sig00));
    QVERIFY(o.isSignalConnected(sig01));
    QVERIFY(o.isSignalConnected(sig69));
    QVERIFY(QObject::disconnect(&o, &ManySignals::sig01, 0, 0));
    QVERIFY(!o.isSignalConnected(sig00));
    QVERIFY(!o.isSignalConnected(sig01));
    QVERIFY(o.isSignalConnected(sig69));
}

void tst_QObject::qMetaObjectConnect()
{
    ReceiverObject r1;
    ReceiverObject r2;
    int slot1Index, slot2Index, slot3Index;
    {
        SenderObject s;
        r1.reset();
        r2.reset();
        ReceiverObject::sequence = 0;

        int signal1Index = s.metaObject()->indexOfSignal("signal1()");
        int signal3Index = s.metaObject()->indexOfSignal("signal3()");
        slot1Index = r1.metaObject()->indexOfSlot("slot1()");
        slot2Index = r1.metaObject()->indexOfSlot("slot2()");
        slot3Index = r1.metaObject()->indexOfSlot("slot3()");

        QVERIFY(slot1Index > 0);
        QVERIFY(slot2Index > 0);
        QVERIFY(slot3Index > 0);

        QVERIFY(QMetaObject::connect(&s, signal1Index, &r1, slot1Index));
        QVERIFY(QMetaObject::connect(&s, signal3Index, &r2, slot3Index));
        QVERIFY(QMetaObject::connect(&s, -1, &r2, slot2Index));

        QCOMPARE(r1.count_slot1, 0);
        QCOMPARE(r1.count_slot2, 0);
        QCOMPARE(r1.count_slot3, 0);
        QCOMPARE(r2.count_slot1, 0);
        QCOMPARE(r2.count_slot2, 0);
        QCOMPARE(r2.count_slot3, 0);

        s.emitSignal1();

        QCOMPARE(r1.count_slot1, 1);
        QCOMPARE(r1.count_slot2, 0);
        QCOMPARE(r1.count_slot3, 0);
        QCOMPARE(r2.count_slot1, 0);
        QCOMPARE(r2.count_slot2, 1);
        QCOMPARE(r2.count_slot3, 0);

        s.emitSignal2();
        s.emitSignal3();
        s.emitSignal4();

        QCOMPARE(r1.count_slot1, 1);
        QCOMPARE(r1.count_slot2, 0);
        QCOMPARE(r1.count_slot3, 0);
        QCOMPARE(r2.count_slot1, 0);
        QCOMPARE(r2.count_slot2, 4);
        QCOMPARE(r2.count_slot3, 1);

        QVERIFY(QMetaObject::disconnect(&s, signal1Index, &r1, slot1Index));
        QVERIFY(QMetaObject::disconnect(&s, signal3Index, &r2, slot3Index));
        QVERIFY(QMetaObject::disconnect(&s, -1, &r2, slot2Index));

        s.emitSignal1();
        s.emitSignal2();
        s.emitSignal3();
        s.emitSignal4();

        QCOMPARE(r1.count_slot1, 1);
        QCOMPARE(r1.count_slot2, 0);
        QCOMPARE(r1.count_slot3, 0);
        QCOMPARE(r2.count_slot1, 0);
        QCOMPARE(r2.count_slot2, 4);
        QCOMPARE(r2.count_slot3, 1);

        //some "dynamic" signal
        QVERIFY(QMetaObject::connect(&s, s.metaObject()->methodOffset() + 20, &r1, slot3Index));
        QVERIFY(QMetaObject::connect(&s, s.metaObject()->methodOffset() + 35, &r2, slot1Index));
        QVERIFY(QMetaObject::connect(&s, -1, &r1, slot2Index));

        r1.reset();
        r2.reset();

        void *args[] = { 0 , 0 };
        QMetaObject::activate(&s, s.metaObject()->methodOffset() + 20, args);
        QMetaObject::activate(&s, s.metaObject()->methodOffset() + 48, args);
        QCOMPARE(r1.count_slot1, 0);
        QCOMPARE(r1.count_slot2, 2);
        QCOMPARE(r1.count_slot3, 1);
        QCOMPARE(r2.count_slot1, 0);
        QCOMPARE(r2.count_slot2, 0);
        QCOMPARE(r2.count_slot3, 0);

        QMetaObject::activate(&s, s.metaObject()->methodOffset() + 35, args);
        s.emitSignal1();
        s.emitSignal2();

        QCOMPARE(r1.count_slot1, 0);
        QCOMPARE(r1.count_slot2, 5);
        QCOMPARE(r1.count_slot3, 1);
        QCOMPARE(r2.count_slot1, 1);
        QCOMPARE(r2.count_slot2, 0);
        QCOMPARE(r2.count_slot3, 0);
    }

    r1.reset();
    r2.reset();

#define SIGNAL_INDEX(S)  obj1.metaObject()->indexOfSignal(QMetaObject::normalizedSignature(#S))
    OverloadObject obj1;
    QObject obj2, obj3;

#if 0 // Verdigris does not support optional arguments
    QMetaObject::connect(&obj1, SIGNAL_INDEX(sig(int)) , &r1, slot1Index);
    QMetaObject::connect(&obj1, SIGNAL_INDEX(sig(QObject *, QObject *, QObject *)) ,
                         &r2, slot1Index);
#else
    QMetaObject::connect(&obj1, SIGNAL_INDEX(sig(int,int)) , &r1, slot1Index);
    QMetaObject::connect(&obj1, SIGNAL_INDEX(sig(QObject *, QObject *, QObject *, QObject *)) ,
                         &r2, slot1Index);
#endif

    QMetaObject::connect(&obj1, SIGNAL_INDEX(sig(QObject *, QObject *, QObject *, QObject *)) ,
                         &r1, slot2Index);
#if 0 // Verdigris does not support optional arguments
    QMetaObject::connect(&obj1, SIGNAL_INDEX(sig(QObject *)) , &r2, slot2Index);
#else
    QMetaObject::connect(&obj1, SIGNAL_INDEX(sig(QObject *, OverloadObject *,QObject *,QObject *)) , &r2, slot2Index);
#endif
    QMetaObject::connect(&obj1, SIGNAL_INDEX(sig(int, int)) , &r1, slot3Index);

    emit obj1.sig(0.5); //connected to nothing
    emit obj1.sig(1, 'a'); //connected to nothing
    QCOMPARE(r1.count_slot1, 0);
    QCOMPARE(r1.count_slot2, 0);
    QCOMPARE(r1.count_slot3, 0);
    QCOMPARE(r2.count_slot1, 0);
    QCOMPARE(r2.count_slot2, 0);
    QCOMPARE(r2.count_slot3, 0);

    emit obj1.sig(1); //this signal is connected
    emit obj1.sig(&obj2);

    QCOMPARE(r1.count_slot1, 1);
    QCOMPARE(r1.count_slot2, 0);
    QCOMPARE(r1.count_slot3, 1);
    QCOMPARE(r2.count_slot1, 0);
    QCOMPARE(r2.count_slot2, 1);
    QCOMPARE(r2.count_slot3, 0);

    emit obj1.sig(&obj2, &obj3); //this signal is connected

    QCOMPARE(r1.count_slot1, 1);
    QCOMPARE(r1.count_slot2, 1);
    QCOMPARE(r1.count_slot3, 1);
    QCOMPARE(r2.count_slot1, 1);
    QCOMPARE(r2.count_slot2, 1);
    QCOMPARE(r2.count_slot3, 0);
}

void tst_QObject::qMetaObjectDisconnectOne()
{
    SenderObject s;
    ReceiverObject r1;

    int signal1Index = s.metaObject()->indexOfSignal("signal1()");
    int signal3Index = s.metaObject()->indexOfSignal("signal3()");
    int slot1Index = r1.metaObject()->indexOfSlot("slot1()");
    int slot2Index = r1.metaObject()->indexOfSlot("slot2()");

    QVERIFY(signal1Index > 0);
    QVERIFY(signal3Index > 0);
    QVERIFY(slot1Index > 0);
    QVERIFY(slot2Index > 0);

    QVERIFY(QMetaObject::connect(&s, signal1Index, &r1, slot1Index));
    QVERIFY(QMetaObject::connect(&s, signal3Index, &r1, slot2Index));
    QVERIFY(QMetaObject::connect(&s, signal3Index, &r1, slot2Index));
    QVERIFY(QMetaObject::connect(&s, signal3Index, &r1, slot2Index));

    r1.reset();
    QCOMPARE(r1.count_slot1, 0);
    QCOMPARE(r1.count_slot2, 0);

    s.emitSignal1();
    QCOMPARE(r1.count_slot1, 1);
    QCOMPARE(r1.count_slot2, 0);

    s.emitSignal3();
    QCOMPARE(r1.count_slot1, 1);
    QCOMPARE(r1.count_slot2, 3);

    r1.reset();
    QVERIFY(QMetaObject::disconnectOne(&s, signal1Index, &r1, slot1Index));
    QVERIFY(QMetaObject::disconnectOne(&s, signal3Index, &r1, slot2Index));

    s.emitSignal1();
    QCOMPARE(r1.count_slot1, 0);
    QCOMPARE(r1.count_slot2, 0);

    s.emitSignal3();
    QCOMPARE(r1.count_slot1, 0);
    QCOMPARE(r1.count_slot2, 2);

    r1.reset();
    QVERIFY(!QMetaObject::disconnectOne(&s, signal1Index, &r1, slot1Index));
    QVERIFY( QMetaObject::disconnectOne(&s, signal3Index, &r1, slot2Index));

    s.emitSignal1();
    QCOMPARE(r1.count_slot1, 0);
    QCOMPARE(r1.count_slot2, 0);

    s.emitSignal3();
    QCOMPARE(r1.count_slot1, 0);
    QCOMPARE(r1.count_slot2, 1);

    r1.reset();
    QVERIFY(!QMetaObject::disconnectOne(&s, signal1Index, &r1, slot1Index));
    QVERIFY( QMetaObject::disconnectOne(&s, signal3Index, &r1, slot2Index));

    s.emitSignal1();
    QCOMPARE(r1.count_slot1, 0);
    QCOMPARE(r1.count_slot2, 0);

    s.emitSignal3();
    QCOMPARE(r1.count_slot1, 0);
    QCOMPARE(r1.count_slot2, 0);
}

class ConfusingObject : public SenderObject
{ W_OBJECT(ConfusingObject)
public slots:
    void signal1() { s++; }
    W_SLOT(signal1)
signals:
    void aPublicSlot() W_SIGNAL(aPublicSlot)
public:
    int s;
    ConfusingObject() : s(0) {}
    friend class tst_QObject;
};

void tst_QObject::sameName()
{
    ConfusingObject c1, c2;
    QVERIFY(connect(&c1, SIGNAL(signal1()), &c1, SLOT(signal1())));
    c1.emitSignal1();
    QCOMPARE(c1.s, 1);

    QVERIFY(connect(&c2, SIGNAL(signal1()), &c1, SIGNAL(signal1())));
    c2.emitSignal1();
    QCOMPARE(c1.s, 2);

#ifndef QT_NO_DEBUG
    QTest::ignoreMessage(QtWarningMsg, "QMetaObject::indexOfSignal: signal aPublicSlot() from SenderObject redefined in ConfusingObject");
#endif
    QVERIFY(connect(&c2, SIGNAL(aPublicSlot()), &c1, SLOT(signal1())));
    c2.aPublicSlot();
    QCOMPARE(c2.aPublicSlotCalled, 0);
    QCOMPARE(c1.aPublicSlotCalled, 0);
    QCOMPARE(c1.s, 3);

#ifndef QT_NO_DEBUG
    QTest::ignoreMessage(QtWarningMsg, "QMetaObject::indexOfSignal: signal aPublicSlot() from SenderObject redefined in ConfusingObject");
#endif
    QVERIFY(connect(&c2, SIGNAL(aPublicSlot()), &c1, SLOT(aPublicSlot())));
    c2.aPublicSlot();
    QCOMPARE(c2.aPublicSlotCalled, 0);
    QCOMPARE(c1.aPublicSlotCalled, 1);
    QCOMPARE(c1.s, 4);
}

void tst_QObject::connectByMetaMethods()
{
    SenderObject s;
    ReceiverObject r;
    const QMetaObject *smeta = s.metaObject();
    const QMetaObject *rmeta = r.metaObject();
    int sigIndx = smeta->indexOfSignal(QMetaObject::normalizedSignature("signal1()"));
    int slotIndx = rmeta->indexOfSlot(QMetaObject::normalizedSignature("slot1()"));
    QVERIFY( sigIndx != -1 );
    QVERIFY( slotIndx != -1 );
    QMetaMethod signal = smeta->method(sigIndx);
    QMetaMethod slot = rmeta->method(slotIndx);

    QVERIFY(connect(&s,signal, &r,slot));

    QVERIFY(!r.called(1));
    s.emitSignal1();
    QVERIFY(r.called(1));
}

void tst_QObject::connectByMetaMethodSlotInsteadOfSignal()
{
    SenderObject s;
    ReceiverObject r;
    const QMetaObject *smeta = s.metaObject();
    const QMetaObject *rmeta = r.metaObject();
    int badIndx = smeta->indexOfSlot(QMetaObject::normalizedSignature("aPublicSlot()"));
    int slotIndx = rmeta->indexOfSlot(QMetaObject::normalizedSignature("slot1()"));
    QVERIFY( badIndx != -1 );
    QVERIFY( slotIndx != -1 );
    QMetaMethod badMethod = smeta->method(badIndx);
    QMetaMethod slot = rmeta->method(slotIndx);

    QTest::ignoreMessage(QtWarningMsg,"QObject::connect: Cannot connect SenderObject::aPublicSlot() to ReceiverObject::slot1()");
    QVERIFY(!connect(&s,badMethod, &r,slot));
}

class Constructable: public QObject
{
    W_OBJECT(Constructable)

public:
    Constructable(){}
    W_CONSTRUCTOR()

};

void tst_QObject::connectConstructorByMetaMethod()
{
    Constructable sc;
    Constructable rc;
    SenderObject s;
    ReceiverObject r;

    const QMetaObject cmeta = Constructable::staticMetaObject;
    const QMetaObject *smeta = s.metaObject();
    const QMetaObject *rmeta = r.metaObject();
    int constructorIndx = cmeta.indexOfConstructor(QMetaObject::normalizedSignature("Constructable()"));
    int sigIndx = smeta->indexOfSignal(QMetaObject::normalizedSignature("signal1()"));
    int slotIndx = rmeta->indexOfSlot(QMetaObject::normalizedSignature("slot1()"));
    QVERIFY( constructorIndx != -1 );
    QVERIFY( sigIndx != -1 );
    QVERIFY( slotIndx != -1 );

    QMetaMethod constructor = cmeta.constructor(constructorIndx);
    QMetaMethod signal = smeta->method(sigIndx);
    QMetaMethod slot = rmeta->method(slotIndx);

    QTest::ignoreMessage(QtWarningMsg,"QObject::connect: Cannot connect Constructable::Constructable() to ReceiverObject::slot1()");
    QVERIFY(!connect(&sc,constructor, &r,slot));
    QTest::ignoreMessage(QtWarningMsg,"QObject::connect: Cannot connect SenderObject::signal1() to Constructable::Constructable()");
    QVERIFY(!connect(&s,signal, &rc,constructor));
    QTest::ignoreMessage(QtWarningMsg,"QObject::connect: Cannot connect Constructable::Constructable() to Constructable::Constructable()");
    QVERIFY(!connect(&sc,constructor, &rc,constructor));
}

void tst_QObject::disconnectByMetaMethod()
{
    SenderObject s;
    ReceiverObject r1;
    ReceiverObject r2;

    QMetaMethod signal1 = s.metaObject()->method(s.metaObject()->indexOfMethod("signal1()"));
    QMetaMethod signal2 = s.metaObject()->method(s.metaObject()->indexOfMethod("signal2()"));
    QMetaMethod signal3 = s.metaObject()->method(s.metaObject()->indexOfMethod("signal3()"));

    QMetaMethod slot1 = r1.metaObject()->method(r1.metaObject()->indexOfMethod("slot1()"));
    QMetaMethod slot2 = r1.metaObject()->method(r1.metaObject()->indexOfMethod("slot2()"));
    QMetaMethod slot3 = r1.metaObject()->method(r1.metaObject()->indexOfMethod("slot3()"));
    QMetaMethod slot4 = r1.metaObject()->method(r1.metaObject()->indexOfMethod("slot4()"));

    connect(&s, signal1, &r1, slot1);

    s.emitSignal1();

    QVERIFY(r1.called(1));
    r1.reset();

    // usual disconnect with all parameters given
    bool ret = QObject::disconnect(&s, signal1, &r1, slot1);

    s.emitSignal1();

    QVERIFY(!r1.called(1));
    r1.reset();

    QVERIFY(ret);
    ret = QObject::disconnect(&s, signal1, &r1, slot1);
    QVERIFY(!ret);

    r1.reset();

    connect(&s, signal1, &r1, slot1);
    connect(&s, signal1, &r1, slot2);
    connect(&s, signal1, &r1, slot3);
    connect(&s, signal2, &r1, slot4);

    // disconnect s's signal1() from all slots of r1
    QObject::disconnect(&s, signal1, &r1, QMetaMethod());

    s.emitSignal1();
    s.emitSignal2();

    QVERIFY(!r1.called(1));
    QVERIFY(!r1.called(2));
    QVERIFY(!r1.called(3));
    QVERIFY(r1.called(4));
    r1.reset();
    // make sure all is disconnected again
    QObject::disconnect(&s, 0, &r1, 0);

    connect(&s, signal1, &r1, slot1);
    connect(&s, signal1, &r2, slot1);
    connect(&s, signal2, &r1, slot2);
    connect(&s, signal2, &r2, slot2);
    connect(&s, signal3, &r1, slot3);
    connect(&s, signal3, &r2, slot3);

    // disconnect signal1() from all receivers
    QObject::disconnect(&s, signal1, 0, QMetaMethod());
    s.emitSignal1();
    s.emitSignal2();
    s.emitSignal3();

    QVERIFY(!r1.called(1));
    QVERIFY(!r2.called(1));
    QVERIFY(r1.called(2));
    QVERIFY(r2.called(2));
    QVERIFY(r1.called(2));
    QVERIFY(r2.called(2));

    r1.reset();
    r2.reset();

    // disconnect all signals of s from all receivers
    QObject::disconnect(&s, 0, 0, 0);

    connect(&s, signal1, &r1, slot1);
    connect(&s, signal1, &r2, slot1);

    // disconnect all signals from slot1 of r1
    QObject::disconnect(&s, QMetaMethod(), &r1, slot1);

    s.emitSignal1();

    QVERIFY(!r1.called(1));
    QVERIFY(r2.called(1));
}

void tst_QObject::disconnectNotSignalMetaMethod()
{
    SenderObject s;
    ReceiverObject r;

    connect(&s, SIGNAL(signal1()), &r, SLOT(slot1()));

    QMetaMethod slot = s.metaObject()->method(
            s.metaObject()->indexOfMethod("aPublicSlot()"));

    QTest::ignoreMessage(QtWarningMsg,"QObject::disconnect: Attempt to unbind non-signal SenderObject::aPublicSlot()");
    QVERIFY(!QObject::disconnect(&s, slot, &r, QMetaMethod()));
}

class ThreadAffinityThread : public QThread
{
public:
    SenderObject *sender;

    ThreadAffinityThread(SenderObject *sender)
        : sender(sender)
    { }
    void run() override
    {
        sender->emitSignal1();
    }
};

void tst_QObject::autoConnectionBehavior()
{
    SenderObject *sender = new SenderObject;
    ReceiverObject *receiver = new ReceiverObject;
    connect(sender, SIGNAL(signal1()), receiver, SLOT(slot1()));

    // at emit, currentThread == sender->thread(), currentThread == receiver->thread(), sender->thread() == receiver->thread()
    QVERIFY(!receiver->called(1));
    sender->emitSignal1();
    QVERIFY(receiver->called(1));
    receiver->reset();

    // at emit, currentThread != sender->thread(), currentThread != receiver->thread(), sender->thread() == receiver->thread()
    ThreadAffinityThread emitThread1(sender);
    QVERIFY(!receiver->called(1));
    emitThread1.start();
    QVERIFY(emitThread1.wait(30000));
    QVERIFY(!receiver->called(1));
    QCoreApplication::sendPostedEvents(receiver, QEvent::MetaCall);
    QVERIFY(receiver->called(1));
    receiver->reset();

    // at emit, currentThread == sender->thread(), currentThread != receiver->thread(), sender->thread() != receiver->thread()
    sender->moveToThread(&emitThread1);
    QVERIFY(!receiver->called(1));
    emitThread1.start();
    QVERIFY(emitThread1.wait(30000));
    QVERIFY(!receiver->called(1));
    QCoreApplication::sendPostedEvents(receiver, QEvent::MetaCall);
    QVERIFY(receiver->called(1));
    receiver->reset();

    // at emit, currentThread != sender->thread(), currentThread == receiver->thread(), sender->thread() != receiver->thread()
    QVERIFY(!receiver->called(1));
    sender->emitSignal1();
    QVERIFY(receiver->called(1));
    receiver->reset();

    // at emit, currentThread != sender->thread(), currentThread != receiver->thread(), sender->thread() != receiver->thread()
    ThreadAffinityThread emitThread2(sender);
    QThread receiverThread;
    QTimer *timer = new QTimer;
    timer->setSingleShot(true);
    timer->setInterval(100);
    connect(&receiverThread, SIGNAL(started()), timer, SLOT(start()));
    connect(timer, SIGNAL(timeout()), &receiverThread, SLOT(quit()), Qt::DirectConnection);
    connect(&receiverThread, SIGNAL(finished()), timer, SLOT(deleteLater()));
    timer->moveToThread(&receiverThread);

    receiver->moveToThread(&receiverThread);
    QVERIFY(!receiver->called(1));
    emitThread2.start();
    QVERIFY(emitThread2.wait(30000));
    QVERIFY(!receiver->called(1));
    receiverThread.start();
    QVERIFY(receiverThread.wait(30000));
    QVERIFY(receiver->called(1));
    receiver->reset();

    delete sender;
    delete receiver;
}

class BaseDestroyed : public QObject
{ W_OBJECT(BaseDestroyed)
    QList<QString> fooList;
    bool destroyed;
public:
    BaseDestroyed() : destroyed(false)
    { fooList << "a" << "b"; }
    ~BaseDestroyed()
    {
        QVERIFY(!destroyed);
        destroyed = true;
    }

public slots:
    void slotUseList()
    {
        QVERIFY(!destroyed);
        fooList << "c" << "d";
    }
    W_SLOT(slotUseList)
};

static void processEvents()
{
    qApp->processEvents();
}

void tst_QObject::baseDestroyed()
{
    {
        BaseDestroyed d;
        connect(&d, SIGNAL(destroyed()), &d, SLOT(slotUseList()));
        //When d goes out of scope, slotUseList should not be called as the BaseDestroyed has
        // already been destroyed while ~QObject emit destroyed
    }
    {
        BaseDestroyed d;
        connect(&d, &QObject::destroyed, processEvents);
        QMetaObject::invokeMethod(&d, "slotUseList", Qt::QueuedConnection);
        //the destructor will call processEvents, that should not call the slotUseList
    }
}

void tst_QObject::pointerConnect()
{
    SenderObject s;
    ReceiverObject r1;
    ReceiverObject r2;
    r1.reset();
    r2.reset();
    ReceiverObject::sequence = 0;
    QTimer timer;

    QVERIFY(connect(&s, &SenderObject::signal1 , &r1, &ReceiverObject::slot1));
    QVERIFY(connect(&s, &SenderObject::signal1 , &r2, &ReceiverObject::slot1));
    QVERIFY(connect(&s, &SenderObject::signal1 , &r1, &ReceiverObject::slot3));
    QVERIFY(connect(&s, &SenderObject::signal3 , &r1, &ReceiverObject::slot3));
    QVERIFY2(connect(&timer, &QTimer::timeout, &r1, &ReceiverObject::deleteLater),
             "Signal connection failed most likely due to failing comparison of pointers to member "
             "functions caused by problems with -reduce-relocations on this platform.");

    s.emitSignal1();
    s.emitSignal2();
    s.emitSignal3();
    s.emitSignal4();

    QCOMPARE(r1.count_slot1, 1);
    QCOMPARE(r1.count_slot2, 0);
    QCOMPARE(r1.count_slot3, 2);
    QCOMPARE(r1.count_slot4, 0);
    QCOMPARE(r2.count_slot1, 1);
    QCOMPARE(r2.count_slot2, 0);
    QCOMPARE(r2.count_slot3, 0);
    QCOMPARE(r2.count_slot4, 0);
    QCOMPARE(r1.sequence_slot1, 1);
    QCOMPARE(r2.sequence_slot1, 2);
    QCOMPARE(r1.sequence_slot3, 4);

    r1.reset();
    r2.reset();
    ReceiverObject::sequence = 0;

    QVERIFY(connect(&s, &SenderObject::signal4, &r1, &ReceiverObject::slot4));
    QVERIFY(connect(&s, &SenderObject::signal4, &r2, &ReceiverObject::slot4));
    QVERIFY(connect(&s, &SenderObject::signal1, &r2, &ReceiverObject::slot4));

    s.emitSignal4();
    QCOMPARE(r1.count_slot4, 1);
    QCOMPARE(r2.count_slot4, 1);
    QCOMPARE(r1.sequence_slot4, 1);
    QCOMPARE(r2.sequence_slot4, 2);

    r1.reset();
    r2.reset();
    ReceiverObject::sequence = 0;

    connect(&s, &SenderObject::signal4 , &r1, &ReceiverObject::slot4);

    s.emitSignal4();
    QCOMPARE(r1.count_slot4, 2);
    QCOMPARE(r2.count_slot4, 1);
    QCOMPARE(r1.sequence_slot4, 3);
    QCOMPARE(r2.sequence_slot4, 2);

    QMetaObject::Connection con;
    QVERIFY(!con);
    QVERIFY(!QObject::disconnect(con));

    //connect a slot to a signal (== error)
    QTest::ignoreMessage(QtWarningMsg, "QObject::connect: signal not found in ReceiverObject");
    con = connect(&r1, &ReceiverObject::slot4 , &s, &SenderObject::signal4);
    QVERIFY(!con);
    QVERIFY(!QObject::disconnect(con));
}

void tst_QObject::pointerDisconnect()
{
    SenderObject s;
    ReceiverObject r1;
    ReceiverObject r2;

    connect(&s, &SenderObject::signal1, &r1, &ReceiverObject::slot1);

    connect(&s, &SenderObject::signal2, &r1, &ReceiverObject::slot2);
    connect(&s, &SenderObject::signal3, &r1, &ReceiverObject::slot3);
    connect(&s, &SenderObject::signal4, &r1, &ReceiverObject::slot4);

    s.emitSignal1();
    s.emitSignal2();
    s.emitSignal3();
    s.emitSignal4();

    QVERIFY(r1.called(1));
    QVERIFY(r1.called(2));
    QVERIFY(r1.called(3));
    QVERIFY(r1.called(4));
    r1.reset();

    // usual disconnect with all parameters given
    bool ret = QObject::disconnect(&s, &SenderObject::signal1, &r1, &ReceiverObject::slot1);

    s.emitSignal1();

    QVERIFY(!r1.called(1));
    r1.reset();

    QVERIFY(ret);
    ret = QObject::disconnect(&s, &SenderObject::signal1, &r1, &ReceiverObject::slot1);
    QVERIFY(!ret);

    // disconnect all signals from s from all slots from r1
    QObject::disconnect(&s, 0, &r1, 0);

    s.emitSignal2();
    s.emitSignal3();
    s.emitSignal4();

    QVERIFY(!r1.called(2));
    QVERIFY(!r1.called(3));
    QVERIFY(!r1.called(4));
    r1.reset();

    connect(&s, &SenderObject::signal1, &r1, &ReceiverObject::slot1);
    connect(&s, &SenderObject::signal1, &r1, &ReceiverObject::slot2);
    connect(&s, &SenderObject::signal1, &r1, &ReceiverObject::slot3);
    connect(&s, &SenderObject::signal2, &r1, &ReceiverObject::slot4);

    // disconnect s's signal1() from all slots of r1
    QObject::disconnect(&s, &SenderObject::signal1, &r1, 0);

    s.emitSignal1();
    s.emitSignal2();

    QVERIFY(!r1.called(1));
    QVERIFY(!r1.called(2));
    QVERIFY(!r1.called(3));
    QVERIFY(r1.called(4));
    r1.reset();
    // make sure all is disconnected again
    QObject::disconnect(&s, 0, &r1, 0);

    connect(&s, &SenderObject::signal1, &r1, &ReceiverObject::slot1);
    connect(&s, &SenderObject::signal1, &r2, &ReceiverObject::slot1);
    connect(&s, &SenderObject::signal2, &r1, &ReceiverObject::slot2);
    connect(&s, &SenderObject::signal2, &r2, &ReceiverObject::slot2);
    connect(&s, &SenderObject::signal3, &r1, &ReceiverObject::slot3);
    connect(&s, &SenderObject::signal3, &r2, &ReceiverObject::slot3);

    // disconnect signal1() from all receivers
    QObject::disconnect(&s, &SenderObject::signal1, 0, 0);
    s.emitSignal1();
    s.emitSignal2();
    s.emitSignal3();

    QVERIFY(!r1.called(1));
    QVERIFY(!r2.called(1));
    QVERIFY(r1.called(2));
    QVERIFY(r2.called(2));
    QVERIFY(r1.called(2));
    QVERIFY(r2.called(2));

    r1.reset();
    r2.reset();

    // disconnect all signals of s from all receivers
    QObject::disconnect(&s, 0, 0, 0);

    QVERIFY(!r1.called(2));
    QVERIFY(!r2.called(2));
    QVERIFY(!r1.called(2));
    QVERIFY(!r2.called(2));
}


void tst_QObject::emitInDefinedOrderPointer()
{
    SenderObject sender;
    ReceiverObject receiver1, receiver2, receiver3, receiver4;

    QMetaObject::Connection h0 = connect(&sender, &SenderObject::signal1, &receiver1, &SequenceObject::slot1);
    QMetaObject::Connection h1 = connect(&sender, &SenderObject::signal1, &receiver2, &SequenceObject::slot1);
    QVERIFY(h0);
    QVERIFY(h1);
    connect(&sender, &SenderObject::signal1, &receiver3, &SequenceObject::slot1);
    connect(&sender, &SenderObject::signal1, &receiver4, &SequenceObject::slot1);
    connect(&sender, &SenderObject::signal1, &receiver1, &SequenceObject::slot2);
    connect(&sender, &SenderObject::signal1, &receiver2, &SequenceObject::slot2);
    connect(&sender, &SenderObject::signal1, &receiver3, &SequenceObject::slot2);
    connect(&sender, &SenderObject::signal1, &receiver4, &SequenceObject::slot2);

    int sequence;
    ReceiverObject::sequence = sequence = 0;
    sender.emitSignal1();
    QCOMPARE(receiver1.sequence_slot1, ++sequence);
    QCOMPARE(receiver2.sequence_slot1, ++sequence);
    QCOMPARE(receiver3.sequence_slot1, ++sequence);
    QCOMPARE(receiver4.sequence_slot1, ++sequence);
    QCOMPARE(receiver1.sequence_slot2, ++sequence);
    QCOMPARE(receiver2.sequence_slot2, ++sequence);
    QCOMPARE(receiver3.sequence_slot2, ++sequence);
    QCOMPARE(receiver4.sequence_slot2, ++sequence);

    QObject::disconnect(h1);
    h1 = connect(&sender, &SenderObject::signal1, &receiver2, &SequenceObject::slot1);

    ReceiverObject::sequence = sequence =  0;
    sender.emitSignal1();
    QCOMPARE(receiver1.sequence_slot1, ++sequence);
    QCOMPARE(receiver3.sequence_slot1, ++sequence);
    QCOMPARE(receiver4.sequence_slot1, ++sequence);
    QCOMPARE(receiver1.sequence_slot2, ++sequence);
    QCOMPARE(receiver2.sequence_slot2, ++sequence);
    QCOMPARE(receiver3.sequence_slot2, ++sequence);
    QCOMPARE(receiver4.sequence_slot2, ++sequence);
    QCOMPARE(receiver2.sequence_slot1, ++sequence);

    QObject::disconnect(h0);
    h0 = connect(&sender, &SenderObject::signal1, &receiver1, &SequenceObject::slot1);

    ReceiverObject::sequence = sequence =  0;
    sender.emitSignal1();
    QCOMPARE(receiver3.sequence_slot1, ++sequence);
    QCOMPARE(receiver4.sequence_slot1, ++sequence);
    QCOMPARE(receiver1.sequence_slot2, ++sequence);
    QCOMPARE(receiver2.sequence_slot2, ++sequence);
    QCOMPARE(receiver3.sequence_slot2, ++sequence);
    QCOMPARE(receiver4.sequence_slot2, ++sequence);
    QCOMPARE(receiver2.sequence_slot1, ++sequence);
    QCOMPARE(receiver1.sequence_slot1, ++sequence);

    QVERIFY(QObject::disconnect(h0));
    QVERIFY(!QObject::disconnect(h0));
}


void tst_QObject::customTypesPointer()
{
    CustomType t0;
    CustomType t1(1, 2, 3);
    CustomType t2(2, 3, 4);

    {
        QCustomTypeChecker checker;
        QCOMPARE(instanceCount, 4);

        connect(&checker, &QCustomTypeChecker::signal1, &checker, &QCustomTypeChecker::slot1,
                Qt::DirectConnection);
        QCOMPARE(checker.received.value(), 0);
        checker.doEmit(t1);
        QCOMPARE(checker.received.value(), t1.value());
        checker.received = t0;


        checker.disconnect();

        int idx = qRegisterMetaType<CustomType>("CustomType");
        QCOMPARE(QMetaType::type("CustomType"), idx);

        connect(&checker, &QCustomTypeChecker::signal1, &checker, &QCustomTypeChecker::slot1,
                Qt::QueuedConnection);
        QCOMPARE(instanceCount, 4);
        checker.doEmit(t2);
        QCOMPARE(instanceCount, 5);
        QCOMPARE(checker.received.value(), t0.value());

        QCoreApplication::processEvents();
        QCOMPARE(checker.received.value(), t2.value());
        QCOMPARE(instanceCount, 4);

        QVERIFY(QMetaType::isRegistered(idx));
        QCOMPARE(qRegisterMetaType<CustomType>("CustomType"), idx);
        QCOMPARE(QMetaType::type("CustomType"), idx);
        QVERIFY(QMetaType::isRegistered(idx));

        // Test auto registered type  (QList<CustomType>)
        QList<CustomType> list;
        QCOMPARE(instanceCount, 4);
        list.append(t1);
        QCOMPARE(instanceCount, 5);
        QVERIFY(connect(&checker, &QCustomTypeChecker::signal2,
                        &checker, &QCustomTypeChecker::slot2, Qt::QueuedConnection));
        emit checker.signal2(list);
        QCOMPARE(instanceCount, 5); //because the list is implicitly shared.
        list.clear();
        QCOMPARE(instanceCount, 5);
        QCoreApplication::processEvents();
        QCOMPARE(checker.received.value(), t1.value());
        QCOMPARE(instanceCount, 4);
    }
    QCOMPARE(instanceCount, 3);
}

void tst_QObject::connectCxx0x()
{
    SenderObject s;
    ReceiverObject r1;

    QObject::connect(&s, &SenderObject::signal1, &r1, &ReceiverObject::slot1);
    QObject::connect(&s, &SenderObject::signal3, &r1, &ReceiverObject::slot2);
    QObject::connect(&s, &SenderObject::signal3, &r1, &ReceiverObject::slot2);
    QObject::connect(&s, &SenderObject::signal3, &r1, &ReceiverObject::slot2);

    r1.reset();
    QCOMPARE(r1.count_slot1, 0);
    QCOMPARE(r1.count_slot2, 0);

    s.emitSignal1();
    QCOMPARE(r1.count_slot1, 1);
    QCOMPARE(r1.count_slot2, 0);

    s.emitSignal3();
    QCOMPARE(r1.count_slot1, 1);
    QCOMPARE(r1.count_slot2, 3);

    // connect signal to signal
    QObject::connect(&s, &SenderObject::signal2, &s, &SenderObject::signal1);

    r1.reset();
    s.emitSignal2();
    QCOMPARE(r1.count_slot1, 1);
}

int receivedCount;
void receiverFunction() { ++receivedCount; }

void tst_QObject::connectToStaticCxx0x()
{
    SenderObject *s = new SenderObject;

    void (*receiver)() = receiverFunction;

    QObject::connect(s, &SenderObject::signal1, receiver);
    receivedCount = 0;
    s->emitSignal1();
    QCOMPARE(receivedCount, 1);

    QObject::connect(s, &SenderObject::signal1, receiver);
    receivedCount = 0;
    s->emitSignal1();
    QCOMPARE(receivedCount, 2);

    delete s;
}

class LotsOfSignalsAndSlots: public QObject
{
    W_OBJECT(LotsOfSignalsAndSlots)
    typedef void (*fptr)();

    public slots:
        void slot_v() {}
        W_SLOT(slot_v)
        void slot_v_noexcept() Q_DECL_NOTHROW {}
        W_SLOT(slot_v_noexcept)
        void slot_vi(int) {}
        W_SLOT(slot_vi)
        void slot_vi_noexcept() Q_DECL_NOTHROW {}
        W_SLOT(slot_vi_noexcept)
        void slot_vii(int, int) {}
        W_SLOT(slot_vii)
        void slot_viii(int, int, int) {}
        W_SLOT(slot_viii)
        int slot_i() { return 0; }
        W_SLOT(slot_i)
        int slot_i_noexcept() Q_DECL_NOTHROW { return 0; }
        W_SLOT(slot_i_noexcept)
        int slot_ii(int) { return 0; }
        W_SLOT(slot_ii)
        int slot_iii(int, int) { return 0; }
        W_SLOT(slot_iii)
        int slot_iiii(int, int, int) { return 0; }
        W_SLOT(slot_iiii)
        void slot_vRi(int &) {}
        W_SLOT(slot_vRi)
        void slot_vs(short) {}
        W_SLOT(slot_vs)
        void slot_vRs(short&) {}
        W_SLOT(slot_vRs)
   /*     #ifdef Q_COMPILER_RVALUE_REFS
        void slot_vOi(int &&) {}
        W_SLOT(slot_vOi)
        void slot_vOs(short &&) {}
        W_SLOT(slot_vOs)
        #endif*/
        void slot_vPFvvE(fptr) {}
        W_SLOT(slot_vPFvvE,(fptr))

        void const_slot_v() const {}
        W_SLOT(const_slot_v);
        void const_slot_v_noexcept() const Q_DECL_NOTHROW {}
        W_SLOT(const_slot_v_noexcept);
        void const_slot_vi(int) const {}
        W_SLOT(const_slot_vi);
        void const_slot_vi_noexcept(int) Q_DECL_NOTHROW {}
        W_SLOT(const_slot_vi_noexcept);

        static void static_slot_v() {}
        W_SLOT(static_slot_v)
        static void static_slot_v_noexcept() Q_DECL_NOTHROW {}
        W_SLOT(static_slot_v_noexcept)
        static void static_slot_vi(int) {}
        W_SLOT(static_slot_vi)
        static void static_slot_vi_noexcept(int) Q_DECL_NOTHROW {}
        W_SLOT(static_slot_vi_noexcept)
        static void static_slot_vii(int, int) {}
        W_SLOT(static_slot_vii)
        static void static_slot_viii(int, int, int) {}
        W_SLOT(static_slot_viii)
        static int static_slot_i() { return 0; }
        W_SLOT(static_slot_i)
        static int static_slot_i_noexcept() Q_DECL_NOTHROW { return 0; }
        W_SLOT(static_slot_i_noexcept)
        static int static_slot_ii(int) { return 0; }
        W_SLOT(static_slot_ii)
        static int static_slot_iii(int, int) { return 0; }
        W_SLOT(static_slot_iii)
        static int static_slot_iiii(int, int, int) { return 0; }
        W_SLOT(static_slot_iiii)
        static void static_slot_vRi(int &) {}
        W_SLOT(static_slot_vRi)
        static void static_slot_vs(short) {}
        W_SLOT(static_slot_vs)
        static void static_slot_vRs(short&) {}
        W_SLOT(static_slot_vRs)
/*        #if defined(Q_COMPILER_RVALUE_REFS) || defined(QT_ENABLE_CXX0X)
        static void static_slot_vOi(int &&) {}
        W_SLOT(static_slot_vOi)
        static void static_slot_vOs(short &&) {}
        W_SLOT(static_slot_vOs)
        #endif*/
        static void static_slot_vPFvvE(fptr) {}
        W_SLOT(static_slot_vPFvvE,(fptr))

        void slot_vcRQObject(const QObject &) {}
        W_SLOT(slot_vcRQObject)
        void slot_vRQObject(QObject &) {}
        W_SLOT(slot_vRQObject)

    signals:
        void signal_v() W_SIGNAL(signal_v)
        void signal_vi(int a) W_SIGNAL(signal_vi, a)
        void signal_vii(int a, int b) W_SIGNAL(signal_vii, a,b)
        void signal_viii(int a, int b, int c) W_SIGNAL(signal_viii, a,b,c)
        void signal_vRi(int &a) W_SIGNAL(signal_vRi, a)
        void signal_vs(short a) W_SIGNAL(signal_vs, a)
        void signal_vRs(short &a) W_SIGNAL(signal_vRs, a)
/*        #if defined(Q_COMPILER_RVALUE_REFS) || defined(QT_ENABLE_CXX0X)
        void signal_vOi(int &&) W_SIGNAL()
        void signal_vOs(short &&) W_SIGNAL()
        #endif*/
        void signal_vPFvvE(fptr a) W_SIGNAL(signal_vPFvvE,(fptr), a)

        void const_signal_v() const W_SIGNAL(const_signal_v)
        void const_signal_vi(int a) const W_SIGNAL(const_signal_vi, a)

        void signal_vcRQObject(const QObject &a) W_SIGNAL(signal_vcRQObject, a)
        void signal_vRQObject(QObject &a) W_SIGNAL(signal_vRQObject, a)

        void signal(short&a, short b, long long c, short d) W_SIGNAL(signal, a,b,c,d)
        void otherSignal(const char *a) W_SIGNAL(otherSignal, a)
};

void tst_QObject::connectCxx0xTypeMatching()
{
    // this is just about connecting the signals to the slots
    // if this fails, this will be a compiler failure
    typedef LotsOfSignalsAndSlots Foo;
    Foo obj;

    // member connects
    QObject::connect(&obj, &Foo::signal_v, &obj, &Foo::slot_v);
    QObject::connect(&obj, &Foo::signal_v, &obj, &Foo::slot_i);

    QObject::connect(&obj, &Foo::signal_vi, &obj, &Foo::slot_v);
    QObject::connect(&obj, &Foo::signal_vi, &obj, &Foo::slot_i);
    QObject::connect(&obj, &Foo::signal_vi, &obj, &Foo::slot_vi);
    QObject::connect(&obj, &Foo::signal_vi, &obj, &Foo::slot_ii);

    QObject::connect(&obj, &Foo::signal_vii, &obj, &Foo::slot_v);
    QObject::connect(&obj, &Foo::signal_vii, &obj, &Foo::slot_i);
    QObject::connect(&obj, &Foo::signal_vii, &obj, &Foo::slot_vi);
    QObject::connect(&obj, &Foo::signal_vii, &obj, &Foo::slot_ii);
    QObject::connect(&obj, &Foo::signal_vii, &obj, &Foo::slot_vii);
    QObject::connect(&obj, &Foo::signal_vii, &obj, &Foo::slot_iii);

    QObject::connect(&obj, &Foo::signal_viii, &obj, &Foo::slot_v);
    QObject::connect(&obj, &Foo::signal_viii, &obj, &Foo::slot_i);
    QObject::connect(&obj, &Foo::signal_viii, &obj, &Foo::slot_vi);
    QObject::connect(&obj, &Foo::signal_viii, &obj, &Foo::slot_ii);
    QObject::connect(&obj, &Foo::signal_viii, &obj, &Foo::slot_vii);
    QObject::connect(&obj, &Foo::signal_viii, &obj, &Foo::slot_iii);
    QObject::connect(&obj, &Foo::signal_viii, &obj, &Foo::slot_viii);
    QObject::connect(&obj, &Foo::signal_viii, &obj, &Foo::slot_iiii);

    QObject::connect(&obj, &Foo::signal_vi, &obj, &Foo::slot_vi); // repeated from above
    QObject::connect(&obj, &Foo::signal_vRi, &obj, &Foo::slot_vi);
    QObject::connect(&obj, &Foo::signal_vRi, &obj, &Foo::slot_vRi);
/*#if defined(Q_COMPILER_RVALUE_REFS) || defined(QT_ENABLE_CXX0X)
    QObject::connect(&obj, &Foo::signal_vOi, &obj, &Foo::slot_vi);
    QObject::connect(&obj, &Foo::signal_vi, &obj, &Foo::slot_vOi);
    QObject::connect(&obj, &Foo::signal_vRi, &obj, &Foo::slot_vOi);
    QObject::connect(&obj, &Foo::signal_vOi, &obj, &Foo::slot_vOi);
#endif*/
    // these are not supposed to compile:
    //QObject::connect(&obj, &Foo::signal_vi, &obj, &Foo::slot_vRi);
    //QObject::connect(&obj, &Foo::signal_vOi, &obj, &Foo::slot_vRi);

    QObject::connect(&obj, &Foo::signal_vs, &obj, &Foo::slot_vi);
    QObject::connect(&obj, &Foo::signal_vRs, &obj, &Foo::slot_vi);
/*#if defined(Q_COMPILER_RVALUE_REFS) || defined(QT_ENABLE_CXX0X)
    QObject::connect(&obj, &Foo::signal_vOs, &obj, &Foo::slot_vi);
    QObject::connect(&obj, &Foo::signal_vRs, &obj, &Foo::slot_vOi);
    QObject::connect(&obj, &Foo::signal_vOs, &obj, &Foo::slot_vOi);
    // these are not supposed to compile:
    //QObject::connect(&obj, &Foo::signal_vOs, &obj, &Foo::slot_vRi);
    //QObject::connect(&obj, &Foo::signal_vRs, &obj, &Foo::slot_vRi);
#endif*/

    QObject::connect(&obj, &Foo::signal_vPFvvE, &obj, &Foo::slot_v);
    QObject::connect(&obj, &Foo::signal_vPFvvE, &obj, &Foo::slot_i);
    QObject::connect(&obj, &Foo::signal_vPFvvE, &obj, &Foo::slot_vPFvvE);

    QObject::connect(&obj, &Foo::signal_v, &Foo::static_slot_v);
    QObject::connect(&obj, &Foo::signal_v, &Foo::static_slot_i);

    QObject::connect(&obj, &Foo::signal_vi, &Foo::static_slot_v);
    QObject::connect(&obj, &Foo::signal_vi, &Foo::static_slot_i);
    QObject::connect(&obj, &Foo::signal_vi, &Foo::static_slot_vi);
    QObject::connect(&obj, &Foo::signal_vi, &Foo::static_slot_ii);

    QObject::connect(&obj, &Foo::signal_vii, &Foo::static_slot_v);
    QObject::connect(&obj, &Foo::signal_vii, &Foo::static_slot_i);
    QObject::connect(&obj, &Foo::signal_vii, &Foo::static_slot_vi);
    QObject::connect(&obj, &Foo::signal_vii, &Foo::static_slot_ii);
    QObject::connect(&obj, &Foo::signal_vii, &Foo::static_slot_vii);
    QObject::connect(&obj, &Foo::signal_vii, &Foo::static_slot_iii);

    QObject::connect(&obj, &Foo::signal_viii, &Foo::static_slot_v);
    QObject::connect(&obj, &Foo::signal_viii, &Foo::static_slot_i);
    QObject::connect(&obj, &Foo::signal_viii, &Foo::static_slot_vi);
    QObject::connect(&obj, &Foo::signal_viii, &Foo::static_slot_ii);
    QObject::connect(&obj, &Foo::signal_viii, &Foo::static_slot_vii);
    QObject::connect(&obj, &Foo::signal_viii, &Foo::static_slot_iii);
    QObject::connect(&obj, &Foo::signal_viii, &Foo::static_slot_viii);
    QObject::connect(&obj, &Foo::signal_viii, &Foo::static_slot_iiii);

/*#if defined(Q_COMPILER_RVALUE_REFS) && defined(QT_ENABLE_CXX0X)
    QObject::connect(&obj, &Foo::signal_vi, &Foo::static_slot_vOi);
    QObject::connect(&obj, &Foo::signal_vRi, &Foo::static_slot_vi);
    QObject::connect(&obj, &Foo::signal_vRi, &Foo::static_slot_vRi);
    QObject::connect(&obj, &Foo::signal_vRi, &Foo::static_slot_vOi);
    QObject::connect(&obj, &Foo::signal_vOi, &Foo::static_slot_vi);
    QObject::connect(&obj, &Foo::signal_vOi, &Foo::static_slot_vOi);
    //QObject::connect(&obj, &Foo::signal_vi, &Foo::static_slot_vRi);
    //QObject::connect(&obj, &Foo::signal_vOi, &Foo::static_slot_vRi);
#endif*/

    QObject::connect(&obj, &Foo::signal_vs, &Foo::static_slot_vi);
    QObject::connect(&obj, &Foo::signal_vRs, &Foo::static_slot_vi);
/*#if defined(Q_COMPILER_RVALUE_REFS) && defined(QT_ENABLE_CXX0X)
    QObject::connect(&obj, &Foo::signal_vOs, &Foo::static_slot_vi);
    QObject::connect(&obj, &Foo::signal_vRs, &Foo::static_slot_vOi);
    QObject::connect(&obj, &Foo::signal_vOs, &Foo::static_slot_vOi);
    //QObject::connect(&obj, &Foo::signal_vOs, &Foo::static_slot_vRi);
    //QObject::connect(&obj, &Foo::signal_vRs, &Foo::static_slot_vRi);
#endif*/
    QObject::connect(&obj, &Foo::signal_vPFvvE, &Foo::static_slot_v);
    QObject::connect(&obj, &Foo::signal_vPFvvE, &Foo::static_slot_i);
    QObject::connect(&obj, &Foo::signal_vPFvvE, &Foo::static_slot_vPFvvE);

    QVERIFY(QObject::connect(&obj, &Foo::const_signal_v, &obj, &Foo::const_slot_v));
    QVERIFY(QObject::connect(&obj, &Foo::const_signal_vi, &obj, &Foo::const_slot_v));
    QVERIFY(QObject::connect(&obj, &Foo::const_signal_vi, &obj, &Foo::slot_vi));
    QVERIFY(QObject::connect(&obj, &Foo::signal_vi, &obj, &Foo::const_slot_vi));
    QVERIFY(QObject::connect(&obj, &Foo::signal_vi, &obj, &Foo::const_slot_v));

    QVERIFY(QObject::connect(&obj, &Foo::signal_vcRQObject, &obj, &Foo::slot_vcRQObject));
    QVERIFY(QObject::connect(&obj, &Foo::signal_vRQObject, &obj, &Foo::slot_vRQObject));
    QVERIFY(QObject::connect(&obj, &Foo::signal_vRQObject, &obj, &Foo::slot_vcRQObject));
    // QVERIFY(QObject::connect(&obj, &Foo::signal_vcRQObject, &obj, &Foo::slot_vRQObject)); // Should be an error  (const& -> &)

    QVERIFY(QObject::connect(&obj, &Foo::signal_vRi, &obj, &Foo::slot_vs));

}

void receiverFunction_noexcept() noexcept {}
struct Functor_noexcept { void operator()() noexcept {} };
void tst_QObject::connectCxx17Noexcept()
{
    // this is about connecting signals to slots with the noexcept qualifier
    // as semantics changed due to http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0012r1.html
    typedef LotsOfSignalsAndSlots Foo;
    Foo obj;

    QObject::connect(&obj, &Foo::signal_v, &obj, &Foo::slot_v_noexcept);
    QObject::connect(&obj, &Foo::signal_v, &obj, &Foo::slot_i_noexcept);
    QObject::connect(&obj, &Foo::signal_v, &obj, &Foo::slot_vi_noexcept);

    QObject::connect(&obj, &Foo::signal_vii, &Foo::static_slot_v_noexcept);
    QObject::connect(&obj, &Foo::signal_vii, &Foo::static_slot_i_noexcept);
    QObject::connect(&obj, &Foo::signal_vii, &Foo::static_slot_vi_noexcept);

    QVERIFY(QObject::connect(&obj, &Foo::signal_vi, &obj, &Foo::const_slot_vi_noexcept));
    QVERIFY(QObject::connect(&obj, &Foo::signal_vi, &obj, &Foo::const_slot_v_noexcept));

    QObject::connect(&obj, &Foo::signal_v, receiverFunction_noexcept);

    Functor_noexcept fn;
    QObject::connect(&obj, &Foo::signal_v, fn);
}

class StringVariant : public QObject
{ W_OBJECT(StringVariant)
signals:
    void stringSignal(const QString &str) W_SIGNAL(stringSignal, str)
public slots:
    void variantSlot(const QVariant &v) { var = v; }
    W_SLOT(variantSlot)
public:
    QVariant var;
    friend class tst_QObject;
};

struct Functor {
    QVariant *var;
    void operator() (const QVariant &v) {
        *var = v;
    }
};

void tst_QObject::connectConvert()
{
    StringVariant obj;
    QVERIFY(connect(&obj, &StringVariant::stringSignal, &obj, &StringVariant::variantSlot));
    QString s = QString::fromLatin1("Hello World");
    emit obj.stringSignal(s);
    QCOMPARE(obj.var.toString(), s);
    QVERIFY(obj.var.toString().isSharedWith(s));

    QVariant var;
    Functor f;
    f.var = &var;
    QVERIFY(connect(&obj, &StringVariant::stringSignal, f));
    s = QString::fromLatin1("GoodBye");
    emit obj.stringSignal(s);
    QCOMPARE(obj.var.toString(), s);
    QVERIFY(obj.var.toString().isSharedWith(s));
    QCOMPARE(var, obj.var);
}

W_REGISTER_ARGTYPE(QString&)
W_REGISTER_ARGTYPE(bool*)
W_REGISTER_ARGTYPE(QString*)


class ConnectWithReferenceObject : public QObject {
    W_OBJECT(ConnectWithReferenceObject)
    friend class tst_QObject;
signals:
    void boolRef(bool &a, bool b) W_SIGNAL(boolRef,a,b)
    void stringRef(QString &a, const QString & b) W_SIGNAL(stringRef,a,b)
    void boolPtr(bool *a, bool b) W_SIGNAL(boolPtr,a,b)
    void stringPtr(QString *a, const QString & b) W_SIGNAL(stringPtr,a,b)
public slots:
    void boolRefSlot(bool &b1, bool b2) {  b1 = b2; }
    W_SLOT(boolRefSlot)
    void stringRefSlot(QString &s1, const QString &s2) {  s1 = s2; }
    W_SLOT(stringRefSlot)
    void boolPtrSlot(bool *b1, bool b2) {  *b1 = b2; }
    W_SLOT(boolPtrSlot)
    void stringPtrSlot(QString *s1, const QString &s2) {  *s1 = s2; }
    W_SLOT(stringPtrSlot)

    void stringSlot1(QString s) { last = s; }
    W_SLOT(stringSlot1)
    void stringSlot2(const QString &s) { last = s; }
    W_SLOT(stringSlot2)
    void stringSlot3(QString &s) { last = s; }
    W_SLOT(stringSlot3)
public:
    QString last;
};

void tst_QObject::connectWithReference()
{
    ConnectWithReferenceObject o;
    bool b1 = true;
    QString s1 = QString::fromLatin1("str1");
    const QString s2 = QString::fromLatin1("str2");
    const QString s3 = QString::fromLatin1("str3");
    o.boolRef(b1, false);
    o.stringRef(s1, s2);
    QCOMPARE(b1, true);
    QCOMPARE(s1, QString::fromLatin1("str1"));
    o.boolPtr(&b1, false);
    o.stringPtr(&s1, s2);
    QCOMPARE(b1, true);
    QCOMPARE(s1, QString::fromLatin1("str1"));

    QVERIFY(connect(&o, &ConnectWithReferenceObject::boolRef, &o, &ConnectWithReferenceObject::boolRefSlot));
    QVERIFY(connect(&o, &ConnectWithReferenceObject::stringRef, &o, &ConnectWithReferenceObject::stringRefSlot));
    QVERIFY(connect(&o, &ConnectWithReferenceObject::boolPtr, &o, &ConnectWithReferenceObject::boolPtrSlot));
    QVERIFY(connect(&o, &ConnectWithReferenceObject::stringPtr, &o, &ConnectWithReferenceObject::stringPtrSlot));
    o.boolRef(b1, false);
    o.stringRef(s1, s2);
    QCOMPARE(b1, false);
    QCOMPARE(s1, QString::fromLatin1("str2"));

    o.boolPtr(&b1, true);
    o.stringPtr(&s1, s3);
    QCOMPARE(b1, true);
    QCOMPARE(s1, QString::fromLatin1("str3"));

    {
        ConnectWithReferenceObject o2;
        QVERIFY(connect(&o2, &ConnectWithReferenceObject::stringRef, &o2, &ConnectWithReferenceObject::stringSlot1));
        o2.stringRef(s1, s2);
        QCOMPARE(s1, s3);
        QCOMPARE(o2.last, s3);
    }
    {
        ConnectWithReferenceObject o2;
        QVERIFY(connect(&o2, &ConnectWithReferenceObject::stringRef, &o2, &ConnectWithReferenceObject::stringSlot2));
        o2.stringRef(s1, s2);
        QCOMPARE(s1, s3);
        QCOMPARE(o2.last, s3);
    }
    {
        ConnectWithReferenceObject o2;
        QVERIFY(connect(&o2, &ConnectWithReferenceObject::stringRef, &o2, &ConnectWithReferenceObject::stringSlot3));
        o2.stringRef(s1, s2);
        QCOMPARE(s1, s3);
        QCOMPARE(o2.last, s3);
    }
}

class ManyArgumentObject : public QObject {
    W_OBJECT(ManyArgumentObject)
signals:
    void signal1(const QString &a) W_SIGNAL(signal1,a)
    void signal2(const QString &a, const QString &b) W_SIGNAL(signal2,a,b)
    void signal3(const QString &a, const QString &b, const QString &c) W_SIGNAL(signal3,a,b,c)
    void signal4(const QString &a, const QString &b, const QString &c, const QString&d) W_SIGNAL(signal4,a,b,c,d)
    void signal5(const QString &a, const QString &b, const QString &c, const QString&d, const QString&e) W_SIGNAL(signal5,a,b,c,d,e)
    void signal6(const QString &a, const QString &b, const QString &c, const QString&d, const QString&e, const QString&f) W_SIGNAL(signal6,a,b,c,d,e,f)

public slots:
#define MANYARGUMENT_COMPARE(L) QCOMPARE(L, QString(#L))
    void slot1(const QString &a) {
        MANYARGUMENT_COMPARE(a);
        count++;
    }
    W_SLOT(slot1)
    void slot2(const QString &a, const QString &b) {
        MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b);
        count++;
    }
    W_SLOT(slot2)
    void slot3(const QString &a, const QString &b, const QString &c) {
        MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b); MANYARGUMENT_COMPARE(c);
        count++;
    }
    W_SLOT(slot3)
    void slot4(const QString &a, const QString &b, const QString &c, const QString&d) {
        MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b); MANYARGUMENT_COMPARE(c);
        MANYARGUMENT_COMPARE(d);
        count++;
    }
    W_SLOT(slot4)
    void slot5(const QString &a, const QString &b, const QString &c, const QString&d, const QString&e) {
        MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b); MANYARGUMENT_COMPARE(c);
        MANYARGUMENT_COMPARE(d); MANYARGUMENT_COMPARE(e);
        count++;
    }
    W_SLOT(slot5)
    void slot6(const QString &a, const QString &b, const QString &c, const QString&d, const QString&e, const QString&f) {
        MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b); MANYARGUMENT_COMPARE(c);
        MANYARGUMENT_COMPARE(d); MANYARGUMENT_COMPARE(e); MANYARGUMENT_COMPARE(f);
        count++;
    }
    W_SLOT(slot6)
public:
    int count;

};

namespace ManyArgumentNamespace {
    int count;
    void slot1(const QString &a) {
        MANYARGUMENT_COMPARE(a);
        count++;
    }
    void slot2(const QString &a, const QString &b) {
        MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b);
        count++;
    }
    void slot3(const QString &a, const QString &b, const QString &c) {
        MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b); MANYARGUMENT_COMPARE(c);
        count++;
    }
    void slot4(const QString &a, const QString &b, const QString &c, const QString&d) {
        MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b); MANYARGUMENT_COMPARE(c);
        MANYARGUMENT_COMPARE(d);
        count++;
    }
    void slot5(const QString &a, const QString &b, const QString &c, const QString&d, const QString&e) {
        MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b); MANYARGUMENT_COMPARE(c);
        MANYARGUMENT_COMPARE(d); MANYARGUMENT_COMPARE(e);
        count++;
    }
    void slot6(const QString &a, const QString &b, const QString &c, const QString&d, const QString&e, const QString&f) {
        MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b); MANYARGUMENT_COMPARE(c);
        MANYARGUMENT_COMPARE(d); MANYARGUMENT_COMPARE(e); MANYARGUMENT_COMPARE(f);
        count++;
    }

    struct Funct1 {
        void operator()(const QString &a) {
            MANYARGUMENT_COMPARE(a);
            count++;
        }
    };

    struct Funct2 {
        void operator()(const QString &a, const QString &b) {
            MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b);
            count++;
        }
    };

    struct Funct3 {
        void operator()(const QString &a, const QString &b, const QString &c) {
            MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b); MANYARGUMENT_COMPARE(c);
            count++;
        }
    };

    struct Funct4 {
        void operator()(const QString &a, const QString &b, const QString &c, const QString&d) {
            MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b); MANYARGUMENT_COMPARE(c);
            MANYARGUMENT_COMPARE(d);
            count++;
        }
    };

    struct Funct5 {
        void operator()(const QString &a, const QString &b, const QString &c, const QString&d, const QString&e) {
            MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b); MANYARGUMENT_COMPARE(c);
            MANYARGUMENT_COMPARE(d); MANYARGUMENT_COMPARE(e);
            count++;
        }
    };

    struct Funct6 {
        void operator()(const QString &a, const QString &b, const QString &c, const QString&d, const QString&e, const QString&f) {
            MANYARGUMENT_COMPARE(a); MANYARGUMENT_COMPARE(b); MANYARGUMENT_COMPARE(c);
            MANYARGUMENT_COMPARE(d); MANYARGUMENT_COMPARE(e); MANYARGUMENT_COMPARE(f);
            count++;
        }
    };
}

void tst_QObject::connectManyArguments()
{
    ManyArgumentObject ob;
    ob.count = 0;
    ManyArgumentNamespace::count = 0;
    connect(&ob, &ManyArgumentObject::signal1, &ob, &ManyArgumentObject::slot1);
    connect(&ob, &ManyArgumentObject::signal2, &ob, &ManyArgumentObject::slot2);
    connect(&ob, &ManyArgumentObject::signal3, &ob, &ManyArgumentObject::slot3);
    connect(&ob, &ManyArgumentObject::signal4, &ob, &ManyArgumentObject::slot4);
    connect(&ob, &ManyArgumentObject::signal5, &ob, &ManyArgumentObject::slot5);
    connect(&ob, &ManyArgumentObject::signal6, &ob, &ManyArgumentObject::slot6);
    connect(&ob, &ManyArgumentObject::signal1, ManyArgumentNamespace::slot1);
    connect(&ob, &ManyArgumentObject::signal2, ManyArgumentNamespace::slot2);
    connect(&ob, &ManyArgumentObject::signal3, ManyArgumentNamespace::slot3);
    connect(&ob, &ManyArgumentObject::signal4, ManyArgumentNamespace::slot4);
    connect(&ob, &ManyArgumentObject::signal5, ManyArgumentNamespace::slot5);
    connect(&ob, &ManyArgumentObject::signal6, ManyArgumentNamespace::slot6);


    connect(&ob, &ManyArgumentObject::signal6, &ob, &ManyArgumentObject::signal5);
    connect(&ob, &ManyArgumentObject::signal5, &ob, &ManyArgumentObject::signal4);
    connect(&ob, &ManyArgumentObject::signal4, &ob, &ManyArgumentObject::signal3);
    connect(&ob, &ManyArgumentObject::signal3, &ob, &ManyArgumentObject::signal2);
    connect(&ob, &ManyArgumentObject::signal2, &ob, &ManyArgumentObject::signal1);

    emit ob.signal6("a", "b", "c", "d", "e", "f");
    QCOMPARE(ob.count, 6);
    QCOMPARE(ManyArgumentNamespace::count, 6);


    ManyArgumentObject ob2;
    ob2.count = 0;
    ManyArgumentNamespace::count = 0;
    connect(&ob2, &ManyArgumentObject::signal6, &ob2, &ManyArgumentObject::slot1);
    connect(&ob2, &ManyArgumentObject::signal6, &ob2, &ManyArgumentObject::slot2);
    connect(&ob2, &ManyArgumentObject::signal6, &ob2, &ManyArgumentObject::slot3);
    connect(&ob2, &ManyArgumentObject::signal6, &ob2, &ManyArgumentObject::slot4);
    connect(&ob2, &ManyArgumentObject::signal6, &ob2, &ManyArgumentObject::slot5);
    connect(&ob2, &ManyArgumentObject::signal6, &ob2, &ManyArgumentObject::slot6);
    connect(&ob2, &ManyArgumentObject::signal6, ManyArgumentNamespace::slot1);
    connect(&ob2, &ManyArgumentObject::signal6, ManyArgumentNamespace::slot2);
    connect(&ob2, &ManyArgumentObject::signal6, ManyArgumentNamespace::slot3);
    connect(&ob2, &ManyArgumentObject::signal6, ManyArgumentNamespace::slot4);
    connect(&ob2, &ManyArgumentObject::signal6, ManyArgumentNamespace::slot5);
    connect(&ob2, &ManyArgumentObject::signal6, ManyArgumentNamespace::slot6);
    connect(&ob2, &ManyArgumentObject::signal6, ManyArgumentNamespace::Funct1());
    connect(&ob2, &ManyArgumentObject::signal6, ManyArgumentNamespace::Funct2());
    connect(&ob2, &ManyArgumentObject::signal6, ManyArgumentNamespace::Funct3());
    connect(&ob2, &ManyArgumentObject::signal6, ManyArgumentNamespace::Funct4());
    connect(&ob2, &ManyArgumentObject::signal6, ManyArgumentNamespace::Funct5());
    connect(&ob2, &ManyArgumentObject::signal6, ManyArgumentNamespace::Funct6());

    emit ob2.signal6("a", "b", "c", "d", "e", "f");
    QCOMPARE(ob2.count, 6);
    QCOMPARE(ManyArgumentNamespace::count, 12);
}

class ForwardDeclared;
W_REGISTER_ARGTYPE(ForwardDeclared)

class ForwardDeclareArguments : public QObject
{
    W_OBJECT(ForwardDeclareArguments)
signals:
    void mySignal(const ForwardDeclared&a) W_SIGNAL(mySignal,a)
public slots:
    void mySlot(const ForwardDeclared&) {}
    W_SLOT(mySlot)
};

void tst_QObject::connectForwardDeclare()
{
    ForwardDeclareArguments ob;
    // it should compile
    QVERIFY(connect(&ob, &ForwardDeclareArguments::mySignal, &ob, &ForwardDeclareArguments::mySlot, Qt::QueuedConnection));
}

class NoDefaultConstructor
{
    W_GADGET(NoDefaultConstructor)
public:
    NoDefaultConstructor(int) {}
};

W_REGISTER_ARGTYPE(NoDefaultConstructor)

class NoDefaultContructorArguments : public QObject
{
    W_OBJECT(NoDefaultContructorArguments)
signals:
    void mySignal(const NoDefaultConstructor&a) W_SIGNAL(mySignal,a)
public slots:
    void mySlot(const NoDefaultConstructor&) {}
    W_SLOT(mySlot)
};

void tst_QObject::connectNoDefaultConstructorArg()
{
    NoDefaultContructorArguments ob;
    // it should compile
    QVERIFY(connect(&ob, &NoDefaultContructorArguments::mySignal, &ob, &NoDefaultContructorArguments::mySlot, Qt::QueuedConnection));
}

struct MoveOnly
{
    int value;
    explicit MoveOnly(int v = 1) : value(v) {}
    MoveOnly(MoveOnly &&o) : value(o.value) { o.value = -1; }
    MoveOnly &operator=(MoveOnly &&o) { value = o.value; o.value = -1; return *this;  }
    Q_DISABLE_COPY(MoveOnly);
};

W_REGISTER_ARGTYPE(MoveOnly)

class ReturnValue : public QObject {
friend class tst_QObject;
W_OBJECT(ReturnValue)
signals:
    QVariant returnVariant(int a) W_SIGNAL(returnVariant,a)
    QString returnString(int a) W_SIGNAL(returnString,a)
    int returnInt(int a) W_SIGNAL(returnInt,a)
    void returnVoid(int a) W_SIGNAL(returnVoid,a)
    CustomType returnCustomType(int a) W_SIGNAL(returnCustomType,a)
    MoveOnly returnMoveOnly(int a) W_SIGNAL(returnMoveOnly,a)

    QObject *returnPointer() W_SIGNAL(returnPointer)
public slots:
    QVariant returnVariantSlot(int i) { return i; }
    W_SLOT(returnVariantSlot)
    QString returnStringSlot(int i) { return QString::number(i); }
    W_SLOT(returnStringSlot)
    int returnIntSlot(int i) { return i; }
    W_SLOT(returnIntSlot)
    CustomType returnCustomTypeSlot(int i) { return CustomType(i); }
    W_SLOT(returnCustomTypeSlot)
    void returnVoidSlot() {}
    W_SLOT(returnVoidSlot)
    int return23() { return 23; }
    W_SLOT(return23)
    QString returnHello() { return QStringLiteral("hello"); }
    W_SLOT(returnHello)
    QObject *returnThisSlot1() { return this; }
    W_SLOT(returnThisSlot1)
    ReturnValue *returnThisSlot2() { return this; }
    W_SLOT(returnThisSlot2)
    MoveOnly returnMoveOnlySlot(int i) { return MoveOnly(i); }
    W_SLOT(returnMoveOnlySlot)
public:
    struct VariantFunctor {
        QVariant operator()(int i) { return i; }
    };
    struct CustomTypeFunctor {
        CustomType operator()(int i) { return CustomType(i); }
    };
    struct StringFunctor {
        QString operator()(int i) { return QString::number(i); }
    };
    struct IntFunctor {
        int operator()(int i) { return i; }
    };
    struct VoidFunctor {
        void operator()(int) {}
    };
    struct MoveOnlyFunctor {
        MoveOnly operator()(int i) { return MoveOnly(i); }
    };
};

W_REGISTER_ARGTYPE(ReturnValue*)

QString someFunctionReturningString(int i) {
    return '\'' + QString::number(i) + '\'';
}

void tst_QObject::returnValue_data()
{
    QTest::addColumn<bool>("isBlockingQueued");

    QTest::newRow("DirectConnection") << false;
    QTest::newRow("BlockingQueuedConnection") << true;
}

void tst_QObject::returnValue()
{
    CheckInstanceCount checker;

    QFETCH(bool, isBlockingQueued);
    QThread thread;
    ReturnValue receiver;
    Qt::ConnectionType type = Qt::DirectConnection;
    if (isBlockingQueued) {
        thread.start();
        receiver.moveToThread(&thread);
        type = Qt::BlockingQueuedConnection;
    }

    { // connected to nothing
        CheckInstanceCount checker;
        ReturnValue r;
        QCOMPARE(emit r.returnVariant(45), QVariant());
        QCOMPARE(emit r.returnString(45), QString());
        QCOMPARE(emit r.returnInt(45), int());
        emit r.returnVoid(45);
        QCOMPARE((emit r.returnCustomType(45)).value(), CustomType().value());
        QCOMPARE((emit r.returnPointer()), static_cast<QObject *>(0));
        QCOMPARE((emit r.returnMoveOnly(666)).value, MoveOnly().value);
    }
    { // connected to a slot returning the same type
        CheckInstanceCount checker;
        ReturnValue r;
        QVERIFY(connect(&r, &ReturnValue::returnVariant, &receiver, &ReturnValue::returnVariantSlot, type));
        QCOMPARE(emit r.returnVariant(45), QVariant(45));
        QVERIFY(connect(&r, &ReturnValue::returnString, &receiver, &ReturnValue::returnStringSlot, type));
        QCOMPARE(emit r.returnString(45), QString::fromLatin1("45"));
        QVERIFY(connect(&r, &ReturnValue::returnInt, &receiver, &ReturnValue::returnIntSlot, type));
        QCOMPARE(emit r.returnInt(45), int(45));
        QVERIFY(connect(&r, &ReturnValue::returnCustomType, &receiver, &ReturnValue::returnCustomTypeSlot, type));
        QCOMPARE((emit r.returnCustomType(45)).value(), CustomType(45).value());
        QVERIFY(connect(&r, &ReturnValue::returnPointer, &receiver, &ReturnValue::returnThisSlot1, type));
        QCOMPARE((emit r.returnPointer()), static_cast<QObject *>(&receiver));
        QVERIFY(connect(&r, &ReturnValue::returnMoveOnly, &receiver, &ReturnValue::returnMoveOnlySlot, type));
        QCOMPARE((emit r.returnMoveOnly(666)).value, 666);
    }
    if (!isBlockingQueued) { // connected to simple functions or functor
        CheckInstanceCount checker;
        ReturnValue r;
        QVERIFY(connect(&r, &ReturnValue::returnString, someFunctionReturningString));
        QCOMPARE(emit r.returnString(49), QString::fromLatin1("'49'"));

        ReturnValue::CustomTypeFunctor customTypeFunctor;
        QVERIFY(connect(&r, &ReturnValue::returnCustomType, customTypeFunctor));
        QCOMPARE((emit r.returnCustomType(49)).value(), CustomType(49).value());

        ReturnValue::VariantFunctor variantFunctor;
        QVERIFY(connect(&r, &ReturnValue::returnVariant, variantFunctor));
        QCOMPARE(emit r.returnVariant(45), QVariant(45));

        ReturnValue::IntFunctor intFunctor;
        QVERIFY(connect(&r, &ReturnValue::returnInt, intFunctor));
        QCOMPARE(emit r.returnInt(45), int(45));

        ReturnValue::MoveOnlyFunctor moveOnlyFunctor;
        QVERIFY(connect(&r, &ReturnValue::returnMoveOnly, moveOnlyFunctor));
        QCOMPARE((emit r.returnMoveOnly(666)).value, 666);
    }
    { // connected to a slot with different type
        CheckInstanceCount checker;
        ReturnValue r;
        QVERIFY(connect(&r, &ReturnValue::returnVariant, &receiver, &ReturnValue::returnStringSlot, type));
        QCOMPARE(emit r.returnVariant(48), QVariant(QString::fromLatin1("48")));
        QVERIFY(connect(&r, &ReturnValue::returnCustomType, &receiver, &ReturnValue::returnIntSlot, type));
        QCOMPARE((emit r.returnCustomType(48)).value(), CustomType(48).value());
        QVERIFY(connect(&r, &ReturnValue::returnVoid, &receiver, &ReturnValue::returnCustomTypeSlot, type));
        emit r.returnVoid(48);
        QVERIFY(connect(&r, &ReturnValue::returnPointer, &receiver, &ReturnValue::returnThisSlot2, type));
        QCOMPARE((emit r.returnPointer()), static_cast<QObject *>(&receiver));
    }
    if (!isBlockingQueued) { // connected to functor with different type
        CheckInstanceCount checker;
        ReturnValue r;

        ReturnValue::CustomTypeFunctor customTypeFunctor;
        QVERIFY(connect(&r, &ReturnValue::returnCustomType, customTypeFunctor));
        QCOMPARE((emit r.returnCustomType(49)).value(), CustomType(49).value());

        ReturnValue::StringFunctor stringFunctor;
        QVERIFY(connect(&r, &ReturnValue::returnVariant, stringFunctor));
        QCOMPARE(emit r.returnVariant(45), QVariant(QString::fromLatin1("45")));
    }
    { // connected to a void
        CheckInstanceCount checker;
        ReturnValue r;
        QVERIFY(connect(&r, &ReturnValue::returnVariant, &receiver, &ReturnValue::returnVoidSlot, type));
        QCOMPARE(emit r.returnVariant(45), QVariant());
        QVERIFY(connect(&r, &ReturnValue::returnString, &receiver, &ReturnValue::returnVoidSlot, type));
        QCOMPARE(emit r.returnString(45), QString());
        QVERIFY(connect(&r, &ReturnValue::returnInt, &receiver, &ReturnValue::returnVoidSlot, type));
        QCOMPARE(emit r.returnInt(45), int());
        QVERIFY(connect(&r, &ReturnValue::returnCustomType, &receiver, &ReturnValue::returnVoidSlot, type));
        QCOMPARE((emit r.returnCustomType(45)).value(), CustomType().value());
        QVERIFY(connect(&r, &ReturnValue::returnPointer, &receiver, &ReturnValue::returnVoidSlot, type));
        QCOMPARE((emit r.returnPointer()), static_cast<QObject *>(0));
        QVERIFY(connect(&r, &ReturnValue::returnMoveOnly, &receiver, &ReturnValue::returnVoidSlot, type));
        QCOMPARE((emit r.returnMoveOnly(666)).value, MoveOnly().value);
    }
    if (!isBlockingQueued) {
        // queued connection should not forward the return value
        CheckInstanceCount checker;
        ReturnValue r;
        QVERIFY(connect(&r, &ReturnValue::returnVariant, &receiver, &ReturnValue::returnVariantSlot, Qt::QueuedConnection));
        QCOMPARE(emit r.returnVariant(45), QVariant());
        QVERIFY(connect(&r, &ReturnValue::returnString, &receiver, &ReturnValue::returnStringSlot, Qt::QueuedConnection));
        QCOMPARE(emit r.returnString(45), QString());
        QVERIFY(connect(&r, &ReturnValue::returnInt, &receiver, &ReturnValue::returnIntSlot, Qt::QueuedConnection));
        QCOMPARE(emit r.returnInt(45), int());
        QVERIFY(connect(&r, &ReturnValue::returnCustomType, &receiver, &ReturnValue::returnCustomTypeSlot, Qt::QueuedConnection));
        QCOMPARE((emit r.returnCustomType(45)).value(), CustomType().value());
        QVERIFY(connect(&r, &ReturnValue::returnPointer, &receiver, &ReturnValue::returnThisSlot1, Qt::QueuedConnection));
        QCOMPARE((emit r.returnPointer()), static_cast<QObject *>(0));
        QVERIFY(connect(&r, &ReturnValue::returnMoveOnly, &receiver, &ReturnValue::returnMoveOnlySlot, Qt::QueuedConnection));
        QCOMPARE((emit r.returnMoveOnly(666)).value, MoveOnly().value);

        QCoreApplication::processEvents();

        QVERIFY(connect(&r, &ReturnValue::returnVariant, &receiver, &ReturnValue::returnStringSlot, Qt::QueuedConnection));
        QCOMPARE(emit r.returnVariant(48), QVariant());
        QVERIFY(connect(&r, &ReturnValue::returnCustomType, &receiver, &ReturnValue::returnIntSlot, Qt::QueuedConnection));
        QCOMPARE((emit r.returnCustomType(48)).value(), CustomType().value());
        QVERIFY(connect(&r, &ReturnValue::returnVoid, &receiver, &ReturnValue::returnCustomTypeSlot, Qt::QueuedConnection));
        emit r.returnVoid(48);
        QCoreApplication::processEvents();
    }

    { // connected to many slots
        ReturnValue::VoidFunctor voidFunctor;
        ReturnValue::IntFunctor intFunctor;
        ReturnValue r;
        QVERIFY(connect(&r, &ReturnValue::returnVariant, &receiver, &ReturnValue::returnVariantSlot, type));
        QCOMPARE(emit r.returnVariant(45), QVariant(45));
        QVERIFY(connect(&r, &ReturnValue::returnVariant, &receiver, &ReturnValue::return23, type));
        QCOMPARE(emit r.returnVariant(45), QVariant(23));
        QVERIFY(connect(&r, &ReturnValue::returnVariant, &receiver, &ReturnValue::returnVoidSlot, type));
        QCOMPARE(emit r.returnVariant(45), QVariant(23));
        QVERIFY(connect(&r, &ReturnValue::returnVariant, &receiver, &ReturnValue::returnHello, type));
        QCOMPARE(emit r.returnVariant(45), QVariant(QStringLiteral("hello")));
        QVERIFY(connect(&r, &ReturnValue::returnVariant, voidFunctor));
        QCOMPARE(emit r.returnVariant(45), QVariant(QStringLiteral("hello")));
        QVERIFY(connect(&r, &ReturnValue::returnVariant, intFunctor));
        QCOMPARE(emit r.returnVariant(45), QVariant(45));
        QVERIFY(connect(&r, &ReturnValue::returnVariant, &receiver, &ReturnValue::return23, Qt::QueuedConnection));
        QCOMPARE(emit r.returnVariant(45), QVariant(45));

        QCOMPARE(emit r.returnInt(45), int());
        QVERIFY(connect(&r, &ReturnValue::returnInt, &receiver, &ReturnValue::returnVoidSlot, type));
        QCOMPARE(emit r.returnInt(45), int());
        QVERIFY(connect(&r, &ReturnValue::returnInt, &receiver, &ReturnValue::returnIntSlot, type));
        QCOMPARE(emit r.returnInt(45), int(45));
        QVERIFY(connect(&r, &ReturnValue::returnInt, &receiver, &ReturnValue::return23, type));
        QCOMPARE(emit r.returnInt(45), int(23));
        QVERIFY(connect(&r, &ReturnValue::returnInt, voidFunctor));
        QCOMPARE(emit r.returnInt(45), int(23));
        QVERIFY(connect(&r, &ReturnValue::returnInt, intFunctor));
        QCOMPARE(emit r.returnInt(45), int(45));
        QVERIFY(connect(&r, &ReturnValue::returnInt, &receiver, &ReturnValue::return23, Qt::QueuedConnection));
        QCOMPARE(emit r.returnInt(45), int(45));

        QCoreApplication::processEvents();
    }

    if (isBlockingQueued) {
        thread.quit();
        thread.wait();
    }
}

void tst_QObject::returnValue2_data()
{ returnValue_data(); }

//String based syntax
void tst_QObject::returnValue2()
{
    CheckInstanceCount checker;

    QFETCH(bool, isBlockingQueued);
    QThread thread;
    ReturnValue receiver;
    Qt::ConnectionType type = Qt::DirectConnection;
    if (isBlockingQueued) {
        thread.start();
        receiver.moveToThread(&thread);
        type = Qt::BlockingQueuedConnection;
    }

    { // connected to a simple slot
        CheckInstanceCount checker;
        ReturnValue r;
        QVERIFY(connect(&r, SIGNAL(returnVariant(int)), &receiver, SLOT(returnVariantSlot(int)), type));
        QCOMPARE(emit r.returnVariant(45), QVariant(45));
        QVERIFY(connect(&r, SIGNAL(returnString(int)), &receiver, SLOT(returnStringSlot(int)), type));
        QCOMPARE(emit r.returnString(45), QString(QStringLiteral("45")));
        QVERIFY(connect(&r, SIGNAL(returnInt(int)), &receiver, SLOT(returnIntSlot(int)), type));
        QCOMPARE(emit r.returnInt(45), int(45));
        QVERIFY(connect(&r, SIGNAL(returnCustomType(int)), &receiver, SLOT(returnCustomTypeSlot(int)), type));
        QCOMPARE((emit r.returnCustomType(45)).value(), CustomType(45).value());
        QVERIFY(connect(&r, SIGNAL(returnMoveOnly(int)), &receiver, SLOT(returnMoveOnlySlot(int)), type));
        QCOMPARE((emit r.returnMoveOnly(45)).value, 45);
    }
    { // connected to a slot returning void
        CheckInstanceCount checker;
        ReturnValue r;
        QVERIFY(connect(&r, SIGNAL(returnVariant(int)), &receiver, SLOT(returnVoidSlot()), type));
        QCOMPARE(emit r.returnVariant(45), QVariant());
        QVERIFY(connect(&r, SIGNAL(returnString(int)), &receiver, SLOT(returnVoidSlot()), type));
        QCOMPARE(emit r.returnString(45), QString());
        QVERIFY(connect(&r, SIGNAL(returnInt(int)), &receiver, SLOT(returnVoidSlot()), type));
        QCOMPARE(emit r.returnInt(45), int());
        QVERIFY(connect(&r, SIGNAL(returnCustomType(int)), &receiver, SLOT(returnVoidSlot()), type));
        QCOMPARE((emit r.returnCustomType(45)).value(), CustomType().value());
        QVERIFY(connect(&r, SIGNAL(returnMoveOnly(int)), &receiver, SLOT(returnVoidSlot()), type));
        QCOMPARE((emit r.returnMoveOnly(45)).value, MoveOnly().value);
    }
    if (!isBlockingQueued) {
        // queued connection should not forward the return value
        CheckInstanceCount checker;
        ReturnValue r;
        QVERIFY(connect(&r, SIGNAL(returnVariant(int)), &receiver, SLOT(returnVariantSlot(int)), Qt::QueuedConnection));
        QCOMPARE(emit r.returnVariant(45), QVariant());
        QVERIFY(connect(&r, SIGNAL(returnString(int)), &receiver, SLOT(returnStringSlot(int)), Qt::QueuedConnection));
        QCOMPARE(emit r.returnString(45), QString());
        QVERIFY(connect(&r, SIGNAL(returnInt(int)), &receiver, SLOT(returnIntSlot(int)), Qt::QueuedConnection));
        QCOMPARE(emit r.returnInt(45), int());
        QVERIFY(connect(&r, SIGNAL(returnCustomType(int)), &receiver, SLOT(returnCustomTypeSlot(int)), Qt::QueuedConnection));
        QCOMPARE((emit r.returnCustomType(45)).value(), CustomType().value());
        QVERIFY(connect(&r, SIGNAL(returnMoveOnly(int)), &receiver, SLOT(returnMoveOnlySlot(int)), Qt::QueuedConnection));
        QCOMPARE((emit r.returnMoveOnly(45)).value, MoveOnly().value);

        QCoreApplication::processEvents();

        //Queued conneciton with different return type should be safe
        QVERIFY(connect(&r, SIGNAL(returnVariant(int)), &receiver, SLOT(returnStringSlot(int)), Qt::QueuedConnection));
        QCOMPARE(emit r.returnVariant(48), QVariant());
        QVERIFY(connect(&r, SIGNAL(returnCustomType(int)), &receiver, SLOT(returnIntSlot(int)), Qt::QueuedConnection));
        QCOMPARE((emit r.returnCustomType(48)).value(), CustomType().value());
        QVERIFY(connect(&r, SIGNAL(returnVoid(int)), &receiver, SLOT(returnCustomTypeSlot(int)), Qt::QueuedConnection));
        emit r.returnVoid(48);
        QCoreApplication::processEvents();
    }
    { // connected to many slots
        ReturnValue r;
        QVERIFY(connect(&r, SIGNAL(returnInt(int)), &receiver, SLOT(returnIntSlot(int)), type));
        QCOMPARE(emit r.returnInt(45), int(45));
        QVERIFY(connect(&r, SIGNAL(returnInt(int)), &receiver, SLOT(returnVoidSlot()), type));
        QCOMPARE(emit r.returnInt(45), int(45));
        QVERIFY(connect(&r, SIGNAL(returnInt(int)), &receiver, SLOT(return23()), type));
        QCOMPARE(emit r.returnInt(45), int(23));
        QVERIFY(connect(&r, SIGNAL(returnInt(int)), &receiver, SLOT(returnIntSlot(int)), Qt::QueuedConnection));
        QCOMPARE(emit r.returnInt(45), int(23));

        QVERIFY(connect(&r, SIGNAL(returnString(int)), &receiver, SLOT(returnStringSlot(int)), type));
        QCOMPARE(emit r.returnString(45), QString(QStringLiteral("45")));
        QVERIFY(connect(&r, SIGNAL(returnString(int)), &receiver, SLOT(returnVoidSlot()), type));
        QCOMPARE(emit r.returnString(45), QString(QStringLiteral("45")));
        QVERIFY(connect(&r, SIGNAL(returnString(int)), &receiver, SLOT(returnHello()), type));
        QCOMPARE(emit r.returnString(45), QString(QStringLiteral("hello")));
        QVERIFY(connect(&r, SIGNAL(returnString(int)), &receiver, SLOT(returnStringSlot(int)), Qt::QueuedConnection));
        QCOMPARE(emit r.returnString(45), QString(QStringLiteral("hello")));
    }
    if (isBlockingQueued) {
        thread.quit();
        thread.wait();
    }
}

class VirtualSlotsObjectBase : public QObject {
    W_OBJECT(VirtualSlotsObjectBase)
public slots:
    virtual void slot1() {
        base_counter1++;
    }
    W_SLOT(slot1)
public:
    VirtualSlotsObjectBase() : base_counter1(0) {}
    int base_counter1;
signals:
    void signal1() W_SIGNAL(signal1)
};

class VirtualSlotsObject : public VirtualSlotsObjectBase {
    W_OBJECT(VirtualSlotsObject)
public slots:
    virtual void slot1() {
        derived_counter1++;
    }
    W_SLOT(slot1)
public:
    VirtualSlotsObject() : derived_counter1(0) {}
    int derived_counter1;
};

void tst_QObject::connectVirtualSlots()
{
    VirtualSlotsObject obj;
    QVERIFY( QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &VirtualSlotsObjectBase::slot1, Qt::UniqueConnection));
    QVERIFY(!QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &VirtualSlotsObjectBase::slot1, Qt::UniqueConnection));

    emit obj.signal1();
    QCOMPARE(obj.base_counter1, 0);
    QCOMPARE(obj.derived_counter1, 1);

    QVERIFY(QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &VirtualSlotsObjectBase::slot1));
    QVERIFY(!QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &VirtualSlotsObjectBase::slot1));

    emit obj.signal1();
    QCOMPARE(obj.base_counter1, 0);
    QCOMPARE(obj.derived_counter1, 1);

    /* the C++ standard say the comparison between pointer to virtual member function is unspecified
    QVERIFY( QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &VirtualSlotsObjectBase::slot1, Qt::UniqueConnection));
    QVERIFY(!QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &VirtualSlotsObject::slot1, Qt::UniqueConnection));
    */
}

struct VirtualBase
{
    int virtual_base_count;
    VirtualBase() : virtual_base_count(0) {}
    virtual ~VirtualBase() {}
    virtual void slot2() = 0;
};

class ObjectWithVirtualBase : public VirtualSlotsObject, public virtual VirtualBase
{
    W_OBJECT(ObjectWithVirtualBase)
public:
    ObjectWithVirtualBase() : regular_call_count(0), derived_counter2(0) {}
    int regular_call_count;
    int derived_counter2;

public slots:
    void regularSlot() { ++regular_call_count; }
    W_SLOT(regularSlot)
    virtual void slot1() { ++derived_counter2; }
    W_SLOT(slot1)
    virtual void slot2() { ++virtual_base_count; }
    W_SLOT(slot2)
};
W_OBJECT_IMPL(ObjectWithVirtualBase)

struct NormalBase
{
    QByteArray lastCalled;
    virtual ~NormalBase() {}
    virtual void virtualBaseSlot() { lastCalled = "virtualBaseSlot"; }
    void normalBaseSlot() { lastCalled = "normalBaseSlot"; }
};

class ObjectWithMultiInheritance : public VirtualSlotsObject, public NormalBase
{
    W_OBJECT(ObjectWithMultiInheritance)
};
W_OBJECT_IMPL(ObjectWithMultiInheritance)

// Normally, the class that inherit QObject always must go first, because of the way qobject_cast
// work, and moc checks for that. But if we don't use Q_OBJECT, this should work
class ObjectWithMultiInheritance2 : public NormalBase, public VirtualSlotsObject
{
    // no QObject as QObject always must go first
    // Q_OBJECT
};

// VMI = Virtual or Multiple Inheritance
// (in this case, both)
void tst_QObject::connectSlotsVMIClass()
{
    // test connecting by the base
    {
        ObjectWithVirtualBase obj;
        QVERIFY( QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &VirtualSlotsObjectBase::slot1, Qt::UniqueConnection));
        QVERIFY(!QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &VirtualSlotsObjectBase::slot1, Qt::UniqueConnection));

        emit obj.signal1();
        QCOMPARE(obj.base_counter1, 0);
        QCOMPARE(obj.derived_counter1, 0);
        QCOMPARE(obj.derived_counter2, 1);
        QCOMPARE(obj.virtual_base_count, 0);

        QVERIFY(QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &VirtualSlotsObjectBase::slot1));
        QVERIFY(!QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &VirtualSlotsObjectBase::slot1));

        emit obj.signal1();
        QCOMPARE(obj.base_counter1, 0);
        QCOMPARE(obj.derived_counter1, 0);
        QCOMPARE(obj.derived_counter2, 1);
        QCOMPARE(obj.virtual_base_count, 0);
    }

    // test connecting with the actual class
    {
        ObjectWithVirtualBase obj;
        QVERIFY( QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &ObjectWithVirtualBase::regularSlot, Qt::UniqueConnection));
        QVERIFY(!QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &ObjectWithVirtualBase::regularSlot, Qt::UniqueConnection));
        QVERIFY( QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &ObjectWithVirtualBase::slot1, Qt::UniqueConnection));
        QVERIFY(!QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &ObjectWithVirtualBase::slot1, Qt::UniqueConnection));

        emit obj.signal1();
        QCOMPARE(obj.base_counter1, 0);
        QCOMPARE(obj.derived_counter1, 0);
        QCOMPARE(obj.derived_counter2, 1);
        QCOMPARE(obj.regular_call_count, 1);
        QCOMPARE(obj.virtual_base_count, 0);

        QVERIFY( QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &ObjectWithVirtualBase::regularSlot));
        QVERIFY(!QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &ObjectWithVirtualBase::regularSlot));
        QVERIFY( QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &ObjectWithVirtualBase::slot1));
        QVERIFY(!QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &ObjectWithVirtualBase::slot1));

        emit obj.signal1();
        QCOMPARE(obj.base_counter1, 0);
        QCOMPARE(obj.derived_counter1, 0);
        QCOMPARE(obj.derived_counter2, 1);
        QCOMPARE(obj.regular_call_count, 1);
        QCOMPARE(obj.virtual_base_count, 0);

        /* the C++ standard say the comparison between pointer to virtual member function is unspecified
        QVERIFY( QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &VirtualSlotsObjectBase::slot1, Qt::UniqueConnection));
        QVERIFY(!QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &ObjectWithVirtualBase::slot1, Qt::UniqueConnection));
        */
    }

    // test connecting a slot that is virtual from the virtual base
    {
        ObjectWithVirtualBase obj;
        QVERIFY( QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &ObjectWithVirtualBase::slot2, Qt::UniqueConnection));
        QVERIFY(!QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &ObjectWithVirtualBase::slot2, Qt::UniqueConnection));

        emit obj.signal1();
        QCOMPARE(obj.base_counter1, 0);
        QCOMPARE(obj.derived_counter1, 0);
        QCOMPARE(obj.derived_counter2, 0);
        QCOMPARE(obj.virtual_base_count, 1);
        QCOMPARE(obj.regular_call_count, 0);

        QVERIFY( QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &ObjectWithVirtualBase::slot2));
        QVERIFY(!QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, &ObjectWithVirtualBase::slot2));

        emit obj.signal1();
        QCOMPARE(obj.base_counter1, 0);
        QCOMPARE(obj.derived_counter1, 0);
        QCOMPARE(obj.derived_counter2, 0);
        QCOMPARE(obj.virtual_base_count, 1);
        QCOMPARE(obj.regular_call_count, 0);
    }

    // test connecting a slot that is virtual within the second base
    {
        ObjectWithMultiInheritance obj;
        void (ObjectWithMultiInheritance::*slot)() = &ObjectWithMultiInheritance::virtualBaseSlot;
        QVERIFY( QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot, Qt::UniqueConnection));
        QVERIFY(!QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot, Qt::UniqueConnection));

        emit obj.signal1();
        QCOMPARE(obj.base_counter1, 0);
        QCOMPARE(obj.derived_counter1, 0);
        QCOMPARE(obj.lastCalled, QByteArray("virtualBaseSlot"));
        obj.lastCalled.clear();

        QVERIFY( QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot));
        QVERIFY(!QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot));

        emit obj.signal1();
        QCOMPARE(obj.base_counter1, 0);
        QCOMPARE(obj.derived_counter1, 0);
        QCOMPARE(obj.lastCalled, QByteArray());
    }

    // test connecting a slot that is not virtual within the second base
    {
        ObjectWithMultiInheritance obj;
        void (ObjectWithMultiInheritance::*slot)() = &ObjectWithMultiInheritance::normalBaseSlot;
        QVERIFY( QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot, Qt::UniqueConnection));
        QVERIFY(!QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot, Qt::UniqueConnection));

        emit obj.signal1();
        QCOMPARE(obj.base_counter1, 0);
        QCOMPARE(obj.derived_counter1, 0);
        QCOMPARE(obj.lastCalled, QByteArray("normalBaseSlot"));
        obj.lastCalled.clear();

        QVERIFY( QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot));
        QVERIFY(!QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot));

        emit obj.signal1();
        QCOMPARE(obj.base_counter1, 0);
        QCOMPARE(obj.derived_counter1, 0);
        QCOMPARE(obj.lastCalled, QByteArray());
    }

    // test connecting a slot within the first non-QObject base
    {
        ObjectWithMultiInheritance2 obj;
        void (ObjectWithMultiInheritance2::*slot)() = &ObjectWithMultiInheritance2::normalBaseSlot;
        QVERIFY( QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot, Qt::UniqueConnection));
        QVERIFY(!QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot, Qt::UniqueConnection));

        emit obj.signal1();
        QCOMPARE(obj.base_counter1, 0);
        QCOMPARE(obj.derived_counter1, 0);
        QCOMPARE(obj.lastCalled, QByteArray("normalBaseSlot"));
        obj.lastCalled.clear();

        QVERIFY( QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot));
        QVERIFY(!QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot));

        emit obj.signal1();
        QCOMPARE(obj.base_counter1, 0);
        QCOMPARE(obj.derived_counter1, 0);
        QCOMPARE(obj.lastCalled, QByteArray());
    }

    // test connecting a slot within the second QObject base
    {
        ObjectWithMultiInheritance2 obj;
        void (ObjectWithMultiInheritance2::*slot)() = &ObjectWithMultiInheritance2::slot1;
        QVERIFY( QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot, Qt::UniqueConnection));
        QVERIFY(!QObject::connect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot, Qt::UniqueConnection));

        emit obj.signal1();
        QCOMPARE(obj.base_counter1, 0);
        QCOMPARE(obj.derived_counter1, 1);
        QCOMPARE(obj.lastCalled, QByteArray());

        QVERIFY( QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot));
        QVERIFY(!QObject::disconnect(&obj, &VirtualSlotsObjectBase::signal1, &obj, slot));

        emit obj.signal1();
        QCOMPARE(obj.base_counter1, 0);
        QCOMPARE(obj.derived_counter1, 1);
        QCOMPARE(obj.lastCalled, QByteArray());
    }
}

#ifndef QT_BUILD_INTERNAL
void tst_QObject::connectPrivateSlots()
{QSKIP("Needs QT_BUILD_INTERNAL");}
#else
class ConnectToPrivateSlotPrivate;

class ConnectToPrivateSlot :public QObject {
    W_OBJECT(ConnectToPrivateSlot)
public:
    ConnectToPrivateSlot();
    void test(SenderObject *obj1) ;
    Q_DECLARE_PRIVATE(ConnectToPrivateSlot)
};

class ConnectToPrivateSlotPrivate : public QObjectPrivate {
    Q_DECLARE_PUBLIC(ConnectToPrivateSlot)
public:
    int receivedCount;
    QVariant receivedValue;

    void thisIsAPrivateSlot() {
        receivedCount++;
    };

    void thisIsAPrivateSlotWithArg(const QVariant &v) {
        receivedCount++;
        receivedValue = v;
    };
};

ConnectToPrivateSlot::ConnectToPrivateSlot(): QObject(*new ConnectToPrivateSlotPrivate) {}

void ConnectToPrivateSlot::test(SenderObject* obj1) {
    Q_D(ConnectToPrivateSlot);
    d->receivedCount = 0;
    QVERIFY(QObjectPrivate::connect(obj1, &SenderObject::signal1, d, &ConnectToPrivateSlotPrivate::thisIsAPrivateSlot));
    QVERIFY(QObjectPrivate::connect(obj1, &SenderObject::signal7, d, &ConnectToPrivateSlotPrivate::thisIsAPrivateSlotWithArg));
    QCOMPARE(d->receivedCount, 0);
    obj1->signal1();
    QCOMPARE(d->receivedCount, 1);
    QCOMPARE(d->receivedValue, QVariant());
    obj1->signal7(666, QLatin1String("_"));
    QCOMPARE(d->receivedCount, 2);
    QCOMPARE(d->receivedValue, QVariant(666));
    QVERIFY(QObjectPrivate::connect(obj1, &SenderObject::signal2, d, &ConnectToPrivateSlotPrivate::thisIsAPrivateSlot, Qt::UniqueConnection));
    QVERIFY(!QObjectPrivate::connect(obj1, &SenderObject::signal2, d, &ConnectToPrivateSlotPrivate::thisIsAPrivateSlot, Qt::UniqueConnection));
    obj1->signal2();
    QCOMPARE(d->receivedCount, 3);
    QVERIFY(QObjectPrivate::disconnect(obj1, &SenderObject::signal2, d, &ConnectToPrivateSlotPrivate::thisIsAPrivateSlot));
    obj1->signal2();
    QCOMPARE(d->receivedCount, 3);
    QVERIFY(!QObjectPrivate::disconnect(obj1, &SenderObject::signal2, d, &ConnectToPrivateSlotPrivate::thisIsAPrivateSlot));
}

void tst_QObject::connectPrivateSlots()
{
    SenderObject sender;
    {
        ConnectToPrivateSlot o;
        o.test(&sender);
    }
    sender.signal7(777, QLatin1String("check that deleting the object properly disconnected"));
    sender.signal1();
}
#endif

struct SlotFunctor
{
    void operator()() {}
};

struct SlotFunctorString
{
    void operator()(const QString &) {}
};

void tst_QObject::connectFunctorArgDifference()
{
    QTimer timer;
    // Compile-time tests that the connection is successful.
    connect(&timer, &QTimer::timeout, SlotFunctor());
    connect(&timer, &QTimer::objectNameChanged, SlotFunctorString());
    connect(qApp, &QCoreApplication::aboutToQuit, SlotFunctor());

    connect(&timer, &QTimer::objectNameChanged, SlotFunctor());
    QStringListModel model;
    connect(&model, &QStringListModel::rowsInserted, SlotFunctor());

    connect(&timer, &QTimer::timeout, [=](){});
    connect(&timer, &QTimer::objectNameChanged, [=](const QString &){});
    connect(qApp, &QCoreApplication::aboutToQuit, [=](){});

    connect(&timer, &QTimer::objectNameChanged, [=](){});
    connect(&model, &QStringListModel::rowsInserted, [=](){});
    connect(&model, &QStringListModel::rowsInserted, [=](const QModelIndex &){});

    QVERIFY(true);
}

class ContextObject : public QObject
{
    W_OBJECT(ContextObject)
public:
    void compareSender(QObject *s) { QCOMPARE(s, sender()); }
};

struct SlotArgFunctor
{
    SlotArgFunctor(int *s) : status(s), context(nullptr), sender(nullptr) {}
    SlotArgFunctor(ContextObject *context, QObject *sender, int *s) : status(s), context(context), sender(sender) {}
    void operator()() { *status = 2; if (context) context->compareSender(sender); }

protected:
    int *status;
    ContextObject *context;
    QObject *sender;
};

void tst_QObject::connectFunctorQueued()
{
    int status = 1;
    SenderObject obj;
    QEventLoop e;

    connect(&obj, &SenderObject::signal1, this, SlotArgFunctor(&status), Qt::QueuedConnection);
    connect(&obj, &SenderObject::signal1, &e, &QEventLoop::quit, Qt::QueuedConnection);

    obj.emitSignal1();
    QCOMPARE(status, 1);
    e.exec();
    QCOMPARE(status, 2);

    status = 1;
    connect(&obj, &SenderObject::signal1, this, [&status] { status = 2; }, Qt::QueuedConnection);

    obj.emitSignal1();
    QCOMPARE(status, 1);
    e.exec();
    QCOMPARE(status, 2);
}

void tst_QObject::connectFunctorWithContext()
{
    int status = 1;
    SenderObject obj;
    ContextObject *context = new ContextObject;
    QEventLoop e;

    connect(&obj, &SenderObject::signal1, context, SlotArgFunctor(&status));
    connect(&obj, &SenderObject::signal1, &e, &QEventLoop::quit, Qt::QueuedConnection);

    // When the context gets deleted, the connection should decay and the signal shouldn't trigger
    // The connection is queued to make sure the destroyed signal propagates correctly and
    // cuts the connection.
    connect(context, &QObject::destroyed, &obj, &SenderObject::signal1, Qt::QueuedConnection);
    context->deleteLater();

    obj.emitSignal1();
    QCOMPARE(status, 2);
    e.exec();
    QCOMPARE(status, 2);

    // Check disconnect with the context object as "receiver" argument, all signals
    context = new ContextObject;
    status = 1;
    connect(&obj, &SenderObject::signal1, context, SlotArgFunctor(&status));

    obj.emitSignal1();
    QCOMPARE(status, 2);
    QObject::disconnect(&obj, nullptr, context, nullptr);
    obj.emitSignal1();
    QCOMPARE(status, 2);

    delete context;

    // Check disconnect with the context object as "receiver" argument, specific signal
    context = new ContextObject;
    status = 1;
    connect(&obj, &SenderObject::signal1, context, SlotArgFunctor(&status));

    obj.emitSignal1();
    QCOMPARE(status, 2);
    QObject::disconnect(&obj, &SenderObject::signal1, context, nullptr);
    obj.emitSignal1();
    QCOMPARE(status, 2);

    delete context;

    // Check the sender arg is set correctly in the context
    context = new ContextObject;

    status = 1;
    connect(&obj, &SenderObject::signal1, context,
            SlotArgFunctor(context, &obj, &status), Qt::QueuedConnection);

    obj.emitSignal1();
    QCOMPARE(status, 1);
    e.exec();
    QCOMPARE(status, 2);

    status = 1;
    connect(&obj, &SenderObject::signal1, this, [this, &status, &obj] { status = 2; QCOMPARE(sender(), &obj); }, Qt::QueuedConnection);

    obj.emitSignal1();
    QCOMPARE(status, 1);
    e.exec();
    QCOMPARE(status, 2);

    delete context;
}

class StatusChanger : public QObject
{
    W_OBJECT(StatusChanger)
public:
    StatusChanger(int *status) : m_status(status)
    {
    }
    ~StatusChanger()
    {
        *m_status = 2;
    }
private:
    int *m_status;
};
W_OBJECT_IMPL(StatusChanger)

class DispatcherWatcher : public QObject
{
    W_OBJECT(DispatcherWatcher)
public:
    DispatcherWatcher(QEventLoop &e, int *statusAwake, int *statusAboutToBlock) :
        m_eventLoop(&e),
        m_statusAwake(statusAwake),
        m_statusAboutToBlock(statusAboutToBlock),
        m_aboutToBlocks(0),
        m_awakes(0)
    {
        awake = new StatusChanger(statusAwake);
        abouttoblock = new StatusChanger(statusAboutToBlock);
        QCOMPARE(*statusAwake, 1);
        QCOMPARE(*statusAboutToBlock, 1);
        connect(QAbstractEventDispatcher::instance(), SIGNAL(awake()), this, SLOT(onAwake()));
        connect(QAbstractEventDispatcher::instance(), SIGNAL(aboutToBlock()), this, SLOT(onAboutToBlock()));

    }

    ~DispatcherWatcher()
    {
        if (awake)
            awake->deleteLater();
        if (abouttoblock)
            abouttoblock->deleteLater();
    }

public slots:
    // The order of these 2 handlers differs on different event dispatchers
    void onAboutToBlock()
    {
        if (abouttoblock) {
            abouttoblock->deleteLater();
            abouttoblock = nullptr;
        }
        ++m_aboutToBlocks;
    }
    W_SLOT(onAboutToBlock)
    void onAwake()
    {
        if (awake) {
            awake->deleteLater();
            awake = nullptr;
        }
        ++m_awakes;

    }
    W_SLOT(onAwake)
    void onSignal1()
    {
        // Status check. At this point the event loop should have spinned enough to delete all the objects.
        QCOMPARE(*m_statusAwake, 2);
        QCOMPARE(*m_statusAboutToBlock, 2);
        QMetaObject::invokeMethod(m_eventLoop, "quit", Qt::QueuedConnection);
    }
    W_SLOT(onSignal1)

private:
    StatusChanger *awake;
    StatusChanger *abouttoblock;
    QEventLoop    *m_eventLoop;
    int *m_statusAwake;
    int *m_statusAboutToBlock;
    int m_aboutToBlocks;
    int m_awakes;
};
W_OBJECT_IMPL(DispatcherWatcher)


void tst_QObject::deleteLaterInAboutToBlockHandler()
{
    int statusAwake        = 1;
    int statusAboutToBlock = 1;
    QEventLoop e;
    DispatcherWatcher dw(e, &statusAwake, &statusAboutToBlock);
    QTimer::singleShot(2000, &dw, &DispatcherWatcher::onSignal1);

    QCOMPARE(statusAwake, 1);
    QCOMPARE(statusAboutToBlock, 1);
    e.exec();
    QCOMPARE(statusAwake, 2);
    QCOMPARE(statusAboutToBlock, 2);
}

void tst_QObject::connectFunctorWithContextUnique()
{
    // Qt::UniqueConnections currently don't work for functors, but we need to
    // be sure that they don't crash. If that is implemented, change this test.

    SenderObject sender;
    ReceiverObject receiver;
    QObject::connect(&sender, &SenderObject::signal1, &receiver, &ReceiverObject::slot1);
    receiver.count_slot1 = 0;

    QObject::connect(&sender, &SenderObject::signal1, &receiver, SlotFunctor(), Qt::UniqueConnection);

    sender.emitSignal1();
    QCOMPARE(receiver.count_slot1, 1);
}

class MyFunctor
{
public:
    explicit MyFunctor(QObject *objectToDisconnect)
        : m_objectToDisconnect(objectToDisconnect)
    {}

    ~MyFunctor() {
        playWithObjects();
    }

    void operator()() {
        // This will cause the slot object associated with this functor to be destroyed after
        // this function returns. That in turn will destroy this functor.
        // If our dtor runs with the signalSlotLock held, the bunch of connect()
        // performed there will deadlock trying to lock that lock again.
        m_objectToDisconnect->disconnect();
    }

private:
    QObject *m_objectToDisconnect;
};

void tst_QObject::connectFunctorDeadlock()
{
    SenderObject sender;
    MyFunctor functor(&sender);
    QObject::connect(&sender, &SenderObject::signal1, functor);
    sender.emitSignal1();
}

void tst_QObject::connectFunctorMoveOnly()
{
    struct MoveOnlyFunctor {
        Q_DISABLE_COPY(MoveOnlyFunctor)
        MoveOnlyFunctor(int *status) : status(status) {}
        MoveOnlyFunctor(MoveOnlyFunctor &&o) : status(o.status) { o.status = nullptr; };
        void operator()(int i) { *status = i; }
        void operator()() { *status = -8; }
        int *status;
    };

    int status = 1;
    SenderObject obj;
    QEventLoop e;

    connect(&obj, &SenderObject::signal1, MoveOnlyFunctor(&status));
    QCOMPARE(status, 1);
    obj.signal1();
    QCOMPARE(status, -8);

    connect(&obj, &SenderObject::signal7, MoveOnlyFunctor(&status));
    QCOMPARE(status, -8);
    obj.signal7(7888, "Hello");
    QCOMPARE(status, 7888);

    // With a context
    status = 1;
    connect(&obj, &SenderObject::signal2, this, MoveOnlyFunctor(&status));
    QCOMPARE(status, 1);
    obj.signal2();
    QCOMPARE(status, -8);

    // QueuedConnection
    status = 1;
    connect(&obj, &SenderObject::signal3, this, MoveOnlyFunctor(&status), Qt::QueuedConnection);
    obj.signal3();
    QCOMPARE(status, 1);
    QCoreApplication::processEvents();
    QCOMPARE(status, -8);
}

static int s_static_slot_checker = 1;

class StaticSlotChecker : public QObject
{
    W_OBJECT(StaticSlotChecker)
public Q_SLOTS:
    static void staticSlot() { s_static_slot_checker = 2; }
    W_SLOT(staticSlot)
};

void tst_QObject::connectStaticSlotWithObject()
{
    SenderObject sender;
    StaticSlotChecker *receiver = new StaticSlotChecker;
    QEventLoop e;

    QVERIFY(connect(&sender, &SenderObject::signal1, receiver, &StaticSlotChecker::staticSlot, Qt::QueuedConnection));
    connect(&sender, &SenderObject::signal1, &e, &QEventLoop::quit, Qt::QueuedConnection);

    sender.emitSignal1();
    QCOMPARE(s_static_slot_checker, 1);
    e.exec();
    QCOMPARE(s_static_slot_checker, 2);

    s_static_slot_checker = 1;

    connect(receiver, &QObject::destroyed, &sender, &SenderObject::signal1, Qt::QueuedConnection);
    receiver->deleteLater();

    QCOMPARE(s_static_slot_checker, 1);
    e.exec();
    QCOMPARE(s_static_slot_checker, 1);
}

struct ComplexFunctor {
    ComplexFunctor(int &overload, QList<QVariant> &result) : overload(overload), result(result) {}
    void operator()(int a, int b) {
        overload = 1;
        result << a << b;
    }
    void operator()(double a, double b) {
        overload = 2;
        result << a << b;
    }
    void operator()(const QString &s) {
        overload = 3;
        result << s;
    }
    void operator()(const QString &) const {
        Q_ASSERT(!"Should not be called because the non-const one should");
        overload = -1;
    }
    template<typename T1, typename T2, typename T3, typename T4>
    void operator()(const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4) {
        overload = 4;
        result << QVariant::fromValue(t1) << QVariant::fromValue(t2) << QVariant::fromValue(t3) << QVariant::fromValue(t4);
    }
    int &overload;
    QList<QVariant> &result;
protected:
    void operator()() const {
        Q_ASSERT(!"Should not be called because it is protected");
        overload = -1;
    }
};

struct ComplexFunctorDeriv : ComplexFunctor {
    ComplexFunctorDeriv(int &overload, QList<QVariant> &result) : ComplexFunctor(overload, result) {}

    void operator()() const {
        overload = 10;
    }
    void operator()(int a, int b) {
        overload = 11;
        result << a << b;
    }
    using ComplexFunctor::operator();
private:
    void operator()(int) {
        Q_ASSERT(!"Should not be called because it is private");
        overload = -1;
    }
};

class FunctorArgDifferenceObject : public QObject
{
    W_OBJECT(FunctorArgDifferenceObject)
signals:
    void signal_ii(int a,int b) W_SIGNAL(signal_ii,a,b)
    void signal_iiS(int a,int b, const QString &c) W_SIGNAL(signal_iiS,a,b,c)
    void signal_dd(double a,double b) W_SIGNAL(signal_dd,a,b)
    void signal_ddS(double a,double b, const QString &c) W_SIGNAL(signal_ddS,a,b,c)
    void signal_S(const QString &a) W_SIGNAL(signal_S,a)
    void signal_SSSS(const QString &a, const QString &c, const QString &d, const QString &e) W_SIGNAL(signal_SSSS,a,c,d,e)
    void signal_iiSS(int a, int b, const QString &c, const QString &d) W_SIGNAL(signal_iiSS,a,b,c,d)
    void signal_VV(const QVariant &a, const QVariant &b) W_SIGNAL(signal_VV,a,b)
};

template<typename Functor, typename Signal>
void connectFunctorOverload_impl(Signal signal, int expOverload, QList<QVariant> expResult)
{
    FunctorArgDifferenceObject obj;
    int overload;
    QList<QVariant> result;
    QVERIFY(QObject::connect(&obj, signal, Functor(overload, result)));

    obj.signal_ii(1,2);
    obj.signal_iiS(3,4,"5");
    obj.signal_dd(6.6,7.7);
    obj.signal_ddS(8.8,9.9,"10");
    obj.signal_S("11");
    obj.signal_SSSS("12", "13", "14", "15");
    obj.signal_iiSS(16, 17, "18", "19");
    obj.signal_VV(20,21);

    QCOMPARE(overload, expOverload);
    QCOMPARE(result, expResult);
}

void tst_QObject::connectFunctorOverloads()
{
#if defined (Q_COMPILER_VARIADIC_TEMPLATES)
    connectFunctorOverload_impl<ComplexFunctor>(&FunctorArgDifferenceObject::signal_ii, 1,
                                (QList<QVariant>() << 1 << 2));
    connectFunctorOverload_impl<ComplexFunctor>(&FunctorArgDifferenceObject::signal_iiS, 1,
                                (QList<QVariant>() << 3 << 4));
    connectFunctorOverload_impl<ComplexFunctor>(&FunctorArgDifferenceObject::signal_dd, 2,
                                (QList<QVariant>() << 6.6 << 7.7));
    connectFunctorOverload_impl<ComplexFunctor>(&FunctorArgDifferenceObject::signal_ddS, 2,
                                (QList<QVariant>() << 8.8 << 9.9));
    connectFunctorOverload_impl<ComplexFunctor>(&FunctorArgDifferenceObject::signal_S, 3,
                                (QList<QVariant>() << QString("11")));
    connectFunctorOverload_impl<ComplexFunctor>(&FunctorArgDifferenceObject::signal_SSSS, 4,
                                (QList<QVariant>() << QString("12") << QString("13") << QString("14") << QString("15")));
    connectFunctorOverload_impl<ComplexFunctor>(&FunctorArgDifferenceObject::signal_iiSS, 4,
                                (QList<QVariant>() << 16 << 17 << QString("18") << QString("19")));
    connectFunctorOverload_impl<ComplexFunctorDeriv>(&FunctorArgDifferenceObject::signal_ii, 11,
                                (QList<QVariant>() << 1 << 2));
    connectFunctorOverload_impl<ComplexFunctorDeriv>(&FunctorArgDifferenceObject::signal_iiS, 11,
                                (QList<QVariant>() << 3 << 4));
    connectFunctorOverload_impl<ComplexFunctorDeriv>(&FunctorArgDifferenceObject::signal_dd, 2,
                                (QList<QVariant>() << 6.6 << 7.7));
    connectFunctorOverload_impl<ComplexFunctorDeriv>(&FunctorArgDifferenceObject::signal_ddS, 2,
                                (QList<QVariant>() << 8.8 << 9.9));
    connectFunctorOverload_impl<ComplexFunctorDeriv>(&FunctorArgDifferenceObject::signal_S, 3,
                                (QList<QVariant>() << QString("11")));
    connectFunctorOverload_impl<ComplexFunctorDeriv>(&FunctorArgDifferenceObject::signal_SSSS, 4,
                                (QList<QVariant>() << QString("12") << QString("13") << QString("14") << QString("15")));
    connectFunctorOverload_impl<ComplexFunctorDeriv>(&FunctorArgDifferenceObject::signal_iiSS, 4,
                                (QList<QVariant>() << 16 << 17 << QString("18") << QString("19")));
    connectFunctorOverload_impl<ComplexFunctorDeriv>(&FunctorArgDifferenceObject::signal_VV, 10,
                                (QList<QVariant>()));


#else
    QSKIP("Does not compile without C++11 variadic template");
#endif
}

class GetSenderObject : public QObject
{
    W_OBJECT(GetSenderObject)
public:
    using QObject::sender; // make public

public Q_SLOTS:
    void triggerSignal() { Q_EMIT aSignal(); }
    W_SLOT(triggerSignal)

Q_SIGNALS:
    void aSignal() W_SIGNAL(aSignal)
};

static int countedStructObjectsCount = 0;
struct CountedStruct
{
    CountedStruct() : sender(nullptr) { ++countedStructObjectsCount; }
    CountedStruct(GetSenderObject *sender) : sender(sender) { ++countedStructObjectsCount; }
    CountedStruct(const CountedStruct &o) : sender(o.sender) { ++countedStructObjectsCount; }
    CountedStruct &operator=(const CountedStruct &) { return *this; }
    // calling sender() here allows us to check if there's a deadlock
    ~CountedStruct() { --countedStructObjectsCount; if (sender) (void)sender->sender(); }
    void operator()() const { }

    GetSenderObject *sender;
};

void tst_QObject::disconnectDoesNotLeakFunctor()
{
    QCOMPARE(countedStructObjectsCount, 0);
    {
        GetSenderObject obj;
        QMetaObject::Connection c;
        {
            CountedStruct s(&obj);
            QCOMPARE(countedStructObjectsCount, 1);

            c = connect(&obj, &GetSenderObject::aSignal, s);
            QVERIFY(c);
            QCOMPARE(countedStructObjectsCount, 2);
            QVERIFY(QObject::disconnect(c));
            QCOMPARE(countedStructObjectsCount, 1);
        }
        QCOMPARE(countedStructObjectsCount, 0);
    }
    QCOMPARE(countedStructObjectsCount, 0);
    {
        GetSenderObject obj;
        QMetaObject::Connection c;
        {
            CountedStruct s(&obj);
            QObject context;
            QCOMPARE(countedStructObjectsCount, 1);

            c = connect(&obj, &GetSenderObject::aSignal, &context, s);
            QVERIFY(c);
            QCOMPARE(countedStructObjectsCount, 2);
            QVERIFY(QObject::disconnect(c));
            QCOMPARE(countedStructObjectsCount, 1);
        }
        QCOMPARE(countedStructObjectsCount, 0);
    }
    QCOMPARE(countedStructObjectsCount, 0);
    {
        QMetaObject::Connection c1, c2;
        {
            CountedStruct s;
            QCOMPARE(countedStructObjectsCount, 1);
            QTimer timer;

            c1 = connect(&timer, &QTimer::timeout, s);
            QVERIFY(c1);
            c2 = c1;
            QVERIFY(c2);
            QCOMPARE(countedStructObjectsCount, 2);
            QVERIFY(QObject::disconnect(c1));
            QVERIFY(!c1);
            QVERIFY(!c2);
            // functor object has been destroyed
            QCOMPARE(countedStructObjectsCount, 1);
            QVERIFY(!QObject::disconnect(c2));
            QCOMPARE(countedStructObjectsCount, 1);
        }
        QCOMPARE(countedStructObjectsCount, 0);
    }
    QCOMPARE(countedStructObjectsCount, 0);
    {
        CountedStruct s;
        QCOMPARE(countedStructObjectsCount, 1);
        QTimer timer;

        QMetaObject::Connection c = connect(&timer, &QTimer::timeout, s);
        QVERIFY(c);
        QCOMPARE(countedStructObjectsCount, 2);
        QVERIFY(QObject::disconnect(c));
        QCOMPARE(countedStructObjectsCount, 1);
    }
    QCOMPARE(countedStructObjectsCount, 0);
    {
        QTimer timer;

        QMetaObject::Connection c = connect(&timer, &QTimer::timeout, CountedStruct());
        QVERIFY(c);
        QCOMPARE(countedStructObjectsCount, 1); // only one instance, in Qt internals
        QVERIFY(QObject::disconnect(c));
        QCOMPARE(countedStructObjectsCount, 0); // functor being destroyed
    }
    QCOMPARE(countedStructObjectsCount, 0);
    {
        QTimer *timer = new QTimer;
        QEventLoop e;

        connect(timer, &QTimer::timeout, CountedStruct());
        QCOMPARE(countedStructObjectsCount, 1); // only one instance, in Qt internals
        timer->deleteLater();
        connect(timer, &QObject::destroyed, &e, &QEventLoop::quit, Qt::QueuedConnection);
        e.exec();
        QCOMPARE(countedStructObjectsCount, 0); // functor being destroyed
    }
    QCOMPARE(countedStructObjectsCount, 0);
    {
        GetSenderObject obj;

        connect(&obj, &GetSenderObject::aSignal, CountedStruct(&obj));
        QCOMPARE(countedStructObjectsCount, 1);
    }
    QCOMPARE(countedStructObjectsCount, 0);
    {
        GetSenderObject obj;

        connect(&obj, &GetSenderObject::aSignal, CountedStruct(&obj));
        QCOMPARE(countedStructObjectsCount, 1);
        QObject::disconnect(&obj, &GetSenderObject::aSignal, 0, 0);
    }
    QCOMPARE(countedStructObjectsCount, 0);
    {
        CountedStruct s;
        QCOMPARE(countedStructObjectsCount, 1);
        QTimer timer;

        QMetaObject::Connection c = connect(&timer, &QTimer::timeout, [s](){});
        QVERIFY(c);
        QCOMPARE(countedStructObjectsCount, 2);
        QVERIFY(QObject::disconnect(c));
        QCOMPARE(countedStructObjectsCount, 1);
    }
    QCOMPARE(countedStructObjectsCount, 0);
}

void tst_QObject::contextDoesNotLeakFunctor()
{
    QCOMPARE(countedStructObjectsCount, 0);
    {
        QMetaObject::Connection c;
        {
            QEventLoop e;
            ContextObject *context = new ContextObject;
            SenderObject obj;

            connect(&obj, &SenderObject::signal1, context, CountedStruct());
            connect(context, &QObject::destroyed, &e, &QEventLoop::quit, Qt::QueuedConnection);
            context->deleteLater();

            QCOMPARE(countedStructObjectsCount, 1);
            e.exec();
            QCOMPARE(countedStructObjectsCount, 0);
        }
        QCOMPARE(countedStructObjectsCount, 0);
    }
    QCOMPARE(countedStructObjectsCount, 0);
    {
        GetSenderObject obj;
        QMetaObject::Connection c;
        {
            CountedStruct s(&obj);
            QEventLoop e;
            ContextObject *context = new ContextObject;
            QCOMPARE(countedStructObjectsCount, 1);

            connect(&obj, &GetSenderObject::aSignal, context, s);
            QCOMPARE(countedStructObjectsCount, 2);

            connect(context, &QObject::destroyed, &e, &QEventLoop::quit, Qt::QueuedConnection);
            context->deleteLater();

            e.exec();
            QCOMPARE(countedStructObjectsCount, 1);
        }
        QCOMPARE(countedStructObjectsCount, 0);
    }
    QCOMPARE(countedStructObjectsCount, 0);
    {
        CountedStruct s;
        QEventLoop e;
        ContextObject *context = new ContextObject;
        QCOMPARE(countedStructObjectsCount, 1);
        QTimer timer;

        connect(&timer, &QTimer::timeout, context, [s](){});
        QCOMPARE(countedStructObjectsCount, 2);
        connect(context, &QObject::destroyed, &e, &QEventLoop::quit, Qt::QueuedConnection);
        context->deleteLater();
        e.exec();
        QCOMPARE(countedStructObjectsCount, 1);
    }
    QCOMPARE(countedStructObjectsCount, 0);
}

class SubSender : public SenderObject {
    W_OBJECT(SubSender)
};

void tst_QObject::connectBase()
{
    SubSender sub;
    ReceiverObject r1;
    r1.reset();

    QVERIFY( connect( &sub, &SubSender::signal1 , &r1, &ReceiverObject::slot1 ) );
    QVERIFY( connect( &sub, static_cast<void (SenderObject::*)()>(&SubSender::signal2) , &r1, &ReceiverObject::slot2 ) );
    QVERIFY( connect( &sub, static_cast<void (SubSender::*)()>(&SubSender::signal3) , &r1, &ReceiverObject::slot3 ) );

    sub.emitSignal1();
    sub.emitSignal2();
    sub.emitSignal3();

    QCOMPARE( r1.count_slot1, 1 );
    QCOMPARE( r1.count_slot2, 1 );
    QCOMPARE( r1.count_slot3, 1 );

    QVERIFY( QObject::disconnect( &sub, &SubSender::signal1 , &r1, &ReceiverObject::slot1 ) );
    QVERIFY( QObject::disconnect( &sub, static_cast<void (SenderObject::*)()>(&SubSender::signal2) , &r1, &ReceiverObject::slot2 ) );
    QVERIFY( QObject::disconnect( &sub, static_cast<void (SubSender::*)()>(&SubSender::signal3) , &r1, &ReceiverObject::slot3 ) );

    sub.emitSignal1();
    sub.emitSignal2();
    sub.emitSignal3();

    QCOMPARE( r1.count_slot1, 1 );
    QCOMPARE( r1.count_slot2, 1 );
    QCOMPARE( r1.count_slot3, 1 );
}

void tst_QObject::connectWarnings()
{
    SubSender sub;
    SenderObject obj;
    ReceiverObject r1;
    r1.reset();

    QTest::ignoreMessage(QtWarningMsg, "QObject::connect(SenderObject, ReceiverObject): invalid nullptr parameter");
    connect(static_cast<const SenderObject *>(nullptr), &SubSender::signal1, &r1, &ReceiverObject::slot1);

    QTest::ignoreMessage(QtWarningMsg, "QObject::connect(SubSender, Unknown): invalid nullptr parameter");
    connect(&sub, &SubSender::signal1, static_cast<ReceiverObject *>(nullptr), &ReceiverObject::slot1);

    QTest::ignoreMessage(QtWarningMsg, "QObject::connect(SenderObject, ReceiverObject): invalid nullptr parameter");
    connect(static_cast<const SenderObject *>(nullptr), &SenderObject::signal1, &r1, &ReceiverObject::slot1);

    QTest::ignoreMessage(QtWarningMsg, "QObject::connect(SenderObject, Unknown): invalid nullptr parameter");
    connect(&obj, &SenderObject::signal1, static_cast<ReceiverObject *>(nullptr), &ReceiverObject::slot1);
}

struct QmlReceiver : public QtPrivate::QSlotObjectBase
{
    int callCount;
    void *magic;

    QmlReceiver()
        : QtPrivate::QSlotObjectBase(&impl)
        , callCount(0)
        , magic(0)
    {}

    static void impl(int which, QSlotObjectBase *this_, QObject *, void **metaArgs, bool *ret)
    {
        switch (which) {
        case Destroy: delete static_cast<QmlReceiver*>(this_); return;
        case Call: static_cast<QmlReceiver*>(this_)->callCount++; return;
        case Compare: *ret = static_cast<QmlReceiver*>(this_)->magic == metaArgs[0]; return;
        case NumOperations: break;
        }
    }
};

void tst_QObject::qmlConnect()
{
#ifdef QT_BUILD_INTERNAL
    SenderObject sender;
    QmlReceiver *receiver = new QmlReceiver;
    receiver->magic = receiver;
    receiver->ref();

    QVERIFY(QObjectPrivate::connect(&sender, sender.metaObject()->indexOfSignal("signal1()"),
                                    receiver, Qt::AutoConnection));

    QCOMPARE(receiver->callCount, 0);
    sender.emitSignal1();
    QCOMPARE(receiver->callCount, 1);

    void *a[] = {
        receiver
    };
    QVERIFY(QObjectPrivate::disconnect(&sender, sender.metaObject()->indexOfSignal("signal1()"), reinterpret_cast<void**>(&a)));

    sender.emitSignal1();
    QCOMPARE(receiver->callCount, 1);

    receiver->destroyIfLastRef();
#else
    QSKIP("Needs QT_BUILD_INTERNAL");
#endif
}

#ifndef QT_NO_EXCEPTIONS
class ObjectException : public std::exception { };

struct ThrowFunctor
{
    CountedStruct operator()(const CountedStruct &, CountedStruct s2) const
    {
        throw ObjectException();
        return s2;
    }
    CountedStruct s;
};
#endif

W_REGISTER_ARGTYPE(CountedStruct)

class ExceptionThrower : public QObject
{
    W_OBJECT(ExceptionThrower)
public slots:
    CountedStruct throwException(const CountedStruct &, CountedStruct s2)
    {
#ifndef QT_NO_EXCEPTIONS
        throw ObjectException();
#endif
        return s2;
    }
    W_SLOT(throwException)
signals:
    CountedStruct mySignal(const CountedStruct &s1, CountedStruct s2) W_SIGNAL(mySignal,s1,s2)
};

class CountedExceptionThrower : public QObject
{
    W_OBJECT(CountedExceptionThrower)

public:
    explicit CountedExceptionThrower(bool throwException, QObject *parent = nullptr)
        : QObject(parent)
    {
        Q_UNUSED(throwException);
#ifndef QT_NO_EXCEPTIONS
        if (throwException)
            throw ObjectException();
#endif
        ++counter;
    }

    ~CountedExceptionThrower()
    {
        --counter;
    }

    static int counter;
};

int CountedExceptionThrower::counter = 0;

void tst_QObject::exceptions()
{
#ifndef QT_NO_EXCEPTIONS
    ReceiverObject receiver;

    // String based syntax
    {
        QCOMPARE(countedStructObjectsCount, 0);
        ExceptionThrower thrower;
        receiver.reset();

        connect(&thrower, SIGNAL(mySignal(CountedStruct,CountedStruct)), &receiver, SLOT(slot1()));
        connect(&thrower, SIGNAL(mySignal(CountedStruct,CountedStruct)), &thrower, SLOT(throwException(CountedStruct,CountedStruct)));
        connect(&thrower, SIGNAL(mySignal(CountedStruct,CountedStruct)), &receiver, SLOT(slot2()));
        try {
            CountedStruct s;
            emit thrower.mySignal(s, s);
            QFAIL("Exception not thrown?");
        } catch (ObjectException&) {}
        QCOMPARE(receiver.count_slot1, 1);
        QCOMPARE(receiver.count_slot2, 0);
        QCOMPARE(countedStructObjectsCount, 0);
    }
    // Pointer to member function
    {
        QCOMPARE(countedStructObjectsCount, 0);
        ExceptionThrower thrower;
        receiver.reset();

        connect(&thrower, &ExceptionThrower::mySignal, &receiver, &ReceiverObject::slot1);
        connect(&thrower, &ExceptionThrower::mySignal, &thrower, &ExceptionThrower::throwException);
        connect(&thrower, &ExceptionThrower::mySignal, &receiver, &ReceiverObject::slot2);
        try {
            CountedStruct s;
            emit thrower.mySignal(s, s);
            QFAIL("Exception not thrown?");
        } catch (ObjectException&) {}
        QCOMPARE(receiver.count_slot1, 1);
        QCOMPARE(receiver.count_slot2, 0);
        QCOMPARE(countedStructObjectsCount, 0);
    }
    // Functor
    {
        QCOMPARE(countedStructObjectsCount, 0);
        ExceptionThrower thrower;
        receiver.reset();

        connect(&thrower, &ExceptionThrower::mySignal, &receiver, &ReceiverObject::slot1);
        connect(&thrower, &ExceptionThrower::mySignal, ThrowFunctor());
        connect(&thrower, &ExceptionThrower::mySignal, &receiver, &ReceiverObject::slot2);
        try {
            CountedStruct s;
            emit thrower.mySignal(s, s);
            QFAIL("Exception not thrown?");
        } catch (ObjectException&) {}
        QCOMPARE(receiver.count_slot1, 1);
        QCOMPARE(receiver.count_slot2, 0);
        QCOMPARE(countedStructObjectsCount, 1); // the Functor
    }
    QCOMPARE(countedStructObjectsCount, 0);

    // Child object reaping in case of exceptions thrown by constructors
    {
        QCOMPARE(CountedExceptionThrower::counter, 0);

        try {
            class ParentObject : public QObject {
            public:
                explicit ParentObject(QObject *parent = nullptr)
                    : QObject(parent)
                {
                    new CountedExceptionThrower(false, this);
                    new CountedExceptionThrower(false, this);
                    new CountedExceptionThrower(true, this); // throws
                }
            };

            ParentObject p;
            QFAIL("Exception not thrown");
        } catch (const ObjectException &) {
        } catch (...) {
            QFAIL("Wrong exception thrown");
        }

        QCOMPARE(CountedExceptionThrower::counter, 0);

        try {
            QObject o;
            new CountedExceptionThrower(false, &o);
            new CountedExceptionThrower(false, &o);
            new CountedExceptionThrower(true, &o); // throws

            QFAIL("Exception not thrown");
        } catch (const ObjectException &) {
        } catch (...) {
            QFAIL("Wrong exception thrown");
        }

        QCOMPARE(CountedExceptionThrower::counter, 0);

        try {
            QObject o;
            CountedExceptionThrower c1(false, &o);
            CountedExceptionThrower c2(false, &o);
            CountedExceptionThrower c3(true, &o); // throws

            QFAIL("Exception not thrown");
        } catch (const ObjectException &) {
        } catch (...) {
            QFAIL("Wrong exception thrown");
        }

        QCOMPARE(CountedExceptionThrower::counter, 0);
    }

#else
    QSKIP("Needs exceptions");
#endif
}

#ifdef QT_BUILD_INTERNAL
static bool parentChangeCalled = false;

static void testParentChanged(QAbstractDeclarativeData *, QObject *, QObject *)
{
    parentChangeCalled = true;
}
#endif

void tst_QObject::noDeclarativeParentChangedOnDestruction()
{
#ifdef QT_BUILD_INTERNAL
    typedef void (*ParentChangedCallback)(QAbstractDeclarativeData *, QObject *, QObject *);
    QScopedValueRollback<ParentChangedCallback> rollback(QAbstractDeclarativeData::parentChanged);
    QAbstractDeclarativeData::parentChanged = testParentChanged;

    QObject *parent = new QObject;
    QObject *child = new QObject;

    QAbstractDeclarativeData dummy;
    QObjectPrivate::get(child)->declarativeData = &dummy;

    parentChangeCalled = false;
    child->setParent(parent);

    QVERIFY(parentChangeCalled);
    parentChangeCalled = false;

    delete child;
    QVERIFY(!parentChangeCalled);

    delete parent;
#else
    QSKIP("Needs QT_BUILD_INTERNAL");
#endif
}

struct MutableFunctor {
    int count;
    MutableFunctor() : count(0) {}
    int operator()() { return ++count; }
};

void tst_QObject::mutableFunctor()
{
    ReturnValue o;
    MutableFunctor functor;
    QCOMPARE(functor.count, 0);
    connect(&o, &ReturnValue::returnInt, functor);
    QCOMPARE(emit o.returnInt(0), 1);
    QCOMPARE(emit o.returnInt(0), 2); // each emit should increase the internal count

    QCOMPARE(functor.count, 0); // but the original object should have been copied at connect time
}

void tst_QObject::checkArgumentsForNarrowing()
{
    enum UnscopedEnum { UnscopedEnumV1 = INT_MAX, UnscopedEnumV2};
    enum SignedUnscopedEnum { SignedUnscopedEnumV1 = INT_MIN, SignedUnscopedEnumV2 = INT_MAX };

    static constexpr bool IsUnscopedEnumSigned = std::is_signed_v<std::underlying_type_t<UnscopedEnum>>;

#define NARROWS_IF(x, y, test) static_assert((QtPrivate::AreArgumentsConvertibleWithoutNarrowingBase<x, y>::value) != (test))
#define FITS_IF(x, y, test)    static_assert((QtPrivate::AreArgumentsConvertibleWithoutNarrowingBase<x, y>::value) == (test))
#define NARROWS(x, y)          NARROWS_IF(x, y, true)
#define FITS(x, y)             FITS_IF(x, y, true)

    static_assert(sizeof(UnscopedEnum) <= sizeof(int));
    static_assert(sizeof(SignedUnscopedEnum) <= sizeof(int));

    // floating point to integral

    // GCC < 9 does not consider floating point to bool to be narrowing,
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65043
#if !defined(Q_CC_GNU) || Q_CC_GNU >= 900
    NARROWS(float, bool);
    NARROWS(double, bool);
    NARROWS(long double, bool);
#endif

    NARROWS(float, char);
    NARROWS(double, char);
    NARROWS(long double, char);

    NARROWS(float, short);
    NARROWS(double, short);
    NARROWS(long double, short);

    NARROWS(float, int);
    NARROWS(double, int);
    NARROWS(long double, int);

    NARROWS(float, long);
    NARROWS(double, long);
    NARROWS(long double, long);

    NARROWS(float, long long);
    NARROWS(double, long long);
    NARROWS(long double, long long);


    // floating point to a smaller floating point
    NARROWS(double, float);
    NARROWS(long double, float);
    FITS(float, double);
    FITS(float, long double);

    // GCC thinks this is narrowing only on architectures where
    // sizeof(long double) > sizeof(double)
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=92856
#if defined(Q_CC_GNU)
    NARROWS_IF(long double, double, sizeof(long double) > sizeof(double));
#else
    NARROWS(long double, double);
#endif
    FITS(double, long double);


    // integral to floating point
    NARROWS(bool, float);
    NARROWS(bool, double);
    NARROWS(bool, long double);

    NARROWS(char, float);
    NARROWS(char, double);
    NARROWS(char, long double);

    NARROWS(short, float);
    NARROWS(short, double);
    NARROWS(short, long double);

    NARROWS(int, float);
    NARROWS(int, double);
    NARROWS(int, long double);

    NARROWS(long, float);
    NARROWS(long, double);
    NARROWS(long, long double);

    NARROWS(long long, float);
    NARROWS(long long, double);
    NARROWS(long long, long double);


    // enum to floating point
    NARROWS(UnscopedEnum, float);
    NARROWS(UnscopedEnum, double);
    NARROWS(UnscopedEnum, long double);

    NARROWS(SignedUnscopedEnum, float);
    NARROWS(SignedUnscopedEnum, double);
    NARROWS(SignedUnscopedEnum, long double);


    // integral to smaller integral
    FITS(bool, bool);
    FITS(char, char);
    FITS(signed char, signed char);
    FITS(signed char, short);
    FITS(signed char, int);
    FITS(signed char, long);
    FITS(signed char, long long);
    FITS(unsigned char, unsigned char);
    FITS(unsigned char, unsigned short);
    FITS(unsigned char, unsigned int);
    FITS(unsigned char, unsigned long);
    FITS(unsigned char, unsigned long long);

    NARROWS_IF(bool, unsigned char, (sizeof(bool) > sizeof(char) || std::is_signed<bool>::value));
    NARROWS_IF(bool, unsigned short, (sizeof(bool) > sizeof(short) || std::is_signed<bool>::value));
    NARROWS_IF(bool, unsigned int, (sizeof(bool) > sizeof(int) || std::is_signed<bool>::value));
    NARROWS_IF(bool, unsigned long, (sizeof(bool) > sizeof(long) || std::is_signed<bool>::value));
    NARROWS_IF(bool, unsigned long long, (sizeof(bool) > sizeof(long long) || std::is_signed<bool>::value));

    NARROWS_IF(short, char, (sizeof(short) > sizeof(char) || std::is_unsigned<char>::value));
    NARROWS_IF(short, unsigned char, (sizeof(short) > sizeof(char)));
    NARROWS_IF(short, signed char, (sizeof(short) > sizeof(char)));

    NARROWS_IF(unsigned short, char, (sizeof(short) > sizeof(char) || std::is_signed<char>::value));
    NARROWS_IF(unsigned short, unsigned char, (sizeof(short) > sizeof(char)));
    NARROWS_IF(unsigned short, signed char, (sizeof(short) > sizeof(char)));

    FITS(short, short);
    FITS(short, int);
    FITS(short, long);
    FITS(short, long long);

    FITS(unsigned short, unsigned short);
    FITS(unsigned short, unsigned int);
    FITS(unsigned short, unsigned long);
    FITS(unsigned short, unsigned long long);

    NARROWS_IF(int, char, (sizeof(int) > sizeof(char) || std::is_unsigned<char>::value));
    NARROWS(int, unsigned char);
    NARROWS_IF(int, signed char, (sizeof(int) > sizeof(char)));
    NARROWS_IF(int, short, (sizeof(int) > sizeof(short)));
    NARROWS(int, unsigned short);

    NARROWS_IF(unsigned int, char, (sizeof(int) > sizeof(char) || std::is_signed<char>::value));
    NARROWS_IF(unsigned int, unsigned char, (sizeof(int) > sizeof(char)));
    NARROWS(unsigned int, signed char);
    NARROWS(unsigned int, short);
    NARROWS_IF(unsigned int, unsigned short, (sizeof(int) > sizeof(short)));

    FITS(int, int);
    FITS(int, long);
    FITS(int, long long);

    FITS(unsigned int, unsigned int);
    FITS(unsigned int, unsigned long);
    FITS(unsigned int, unsigned long long);

    NARROWS_IF(long, char, (sizeof(long) > sizeof(char) || std::is_unsigned<char>::value));
    NARROWS(long, unsigned char);
    NARROWS_IF(long, signed char, (sizeof(long) > sizeof(char)));
    NARROWS_IF(long, short, (sizeof(long) > sizeof(short)));
    NARROWS(long, unsigned short);
    NARROWS_IF(long, int, (sizeof(long) > sizeof(int)));
    NARROWS(long, unsigned int);

    NARROWS_IF(unsigned long, char, (sizeof(long) > sizeof(char) || std::is_signed<char>::value));
    NARROWS_IF(unsigned long, unsigned char, (sizeof(long) > sizeof(char)));
    NARROWS(unsigned long, signed char);
    NARROWS(unsigned long, short);
    NARROWS_IF(unsigned long, unsigned short, (sizeof(long) > sizeof(short)));
    NARROWS(unsigned long, int);
    NARROWS_IF(unsigned long, unsigned int, (sizeof(long) > sizeof(int)));

    FITS(long, long);
    FITS(long, long long);

    FITS(unsigned long, unsigned long);
    FITS(unsigned long, unsigned long long);

    NARROWS_IF(long long, char, (sizeof(long long) > sizeof(char) || std::is_unsigned<char>::value));
    NARROWS(long long, unsigned char);
    NARROWS_IF(long long, signed char, (sizeof(long long) > sizeof(char)));
    NARROWS_IF(long long, short, (sizeof(long long) > sizeof(short)));
    NARROWS(long long, unsigned short);
    NARROWS_IF(long long, int, (sizeof(long long) > sizeof(int)));
    NARROWS(long long, unsigned int);
    NARROWS_IF(long long, long, (sizeof(long long) > sizeof(long)));
    NARROWS(long long, unsigned long);

    NARROWS_IF(unsigned long long, char, (sizeof(long long) > sizeof(char) || std::is_signed<char>::value));
    NARROWS_IF(unsigned long long, unsigned char, (sizeof(long long) > sizeof(char)));
    NARROWS(unsigned long long, signed char);
    NARROWS(unsigned long long, short);
    NARROWS_IF(unsigned long long, unsigned short, (sizeof(long long) > sizeof(short)));
    NARROWS(unsigned long long, int);
    NARROWS_IF(unsigned long long, unsigned int, (sizeof(long long) > sizeof(int)));
    NARROWS(unsigned long long, long);
    NARROWS_IF(unsigned long long, unsigned long, (sizeof(long long) > sizeof(long)));

    FITS(long long, long long);
    FITS(unsigned long long, unsigned long long);


    // integral to integral with different signedness. smaller ones tested above
    NARROWS(signed char, unsigned char);
    NARROWS(signed char, unsigned short);
    NARROWS(signed char, unsigned int);
    NARROWS(signed char, unsigned long);
    NARROWS(signed char, unsigned long long);

    NARROWS(unsigned char, signed char);
    FITS(unsigned char, short);
    FITS(unsigned char, int);
    FITS(unsigned char, long);
    FITS(unsigned char, long long);

    NARROWS(short, unsigned short);
    NARROWS(short, unsigned int);
    NARROWS(short, unsigned long);
    NARROWS(short, unsigned long long);

    NARROWS(unsigned short, short);
    FITS(unsigned short, int);
    FITS(unsigned short, long);
    FITS(unsigned short, long long);

    NARROWS(int, unsigned int);
    NARROWS(int, unsigned long);
    NARROWS(int, unsigned long long);

    NARROWS(unsigned int, int);
    NARROWS_IF(unsigned int, long, (sizeof(int) >= sizeof(long)));
    FITS(unsigned int, long long);

    NARROWS(long, unsigned long);
    NARROWS(long, unsigned long long);

    NARROWS(unsigned long, long);
    NARROWS_IF(unsigned long, long long, (sizeof(long) >= sizeof(long long)));

    NARROWS(long long, unsigned long long);
    NARROWS(unsigned long long, long long);

    // enum to smaller integral
    // (note that we know that sizeof(UnscopedEnum) <= sizeof(int)
    FITS(UnscopedEnum, UnscopedEnum);
    FITS(SignedUnscopedEnum, SignedUnscopedEnum);

    NARROWS_IF(UnscopedEnum, char, ((sizeof(UnscopedEnum) > sizeof(char)) || (sizeof(UnscopedEnum) == sizeof(char) && IsUnscopedEnumSigned == std::is_signed<char>::value)));
    NARROWS_IF(UnscopedEnum, signed char, ((sizeof(UnscopedEnum) > sizeof(char)) || (sizeof(UnscopedEnum) == sizeof(char) && !IsUnscopedEnumSigned)));
    NARROWS_IF(UnscopedEnum, unsigned char, ((sizeof(UnscopedEnum) > sizeof(char)) || IsUnscopedEnumSigned));

    NARROWS_IF(UnscopedEnum, short, ((sizeof(UnscopedEnum) > sizeof(short)) || (sizeof(UnscopedEnum) == sizeof(short) && !IsUnscopedEnumSigned)));
    NARROWS_IF(UnscopedEnum, unsigned short, ((sizeof(UnscopedEnum) > sizeof(short)) || IsUnscopedEnumSigned));

    NARROWS_IF(UnscopedEnum, int, sizeof(UnscopedEnum) > sizeof(int) || (sizeof(UnscopedEnum) == sizeof(int) && !IsUnscopedEnumSigned));
    NARROWS_IF(UnscopedEnum, unsigned int, IsUnscopedEnumSigned);

    NARROWS_IF(UnscopedEnum, long, sizeof(UnscopedEnum) > sizeof(long) || (sizeof(UnscopedEnum) == sizeof(long) && !IsUnscopedEnumSigned));
    NARROWS_IF(UnscopedEnum, unsigned long, IsUnscopedEnumSigned);

    NARROWS_IF(UnscopedEnum, long long, sizeof(UnscopedEnum) > sizeof(long long) || (sizeof(UnscopedEnum) == sizeof(long long) && !IsUnscopedEnumSigned));
    NARROWS_IF(UnscopedEnum, unsigned long long, IsUnscopedEnumSigned);

    static_assert(std::is_signed<typename std::underlying_type<SignedUnscopedEnum>::type>::value);

    NARROWS_IF(SignedUnscopedEnum, signed char, (sizeof(SignedUnscopedEnum) > sizeof(char)));
    NARROWS_IF(SignedUnscopedEnum, short, (sizeof(SignedUnscopedEnum) > sizeof(short)));
    FITS(SignedUnscopedEnum, int);
    FITS(SignedUnscopedEnum, long);
    FITS(SignedUnscopedEnum, long long);

    // other types which should be always unaffected
    FITS(void *, void *);

    FITS(QString, QString);
    FITS(QObject, QObject);
    FITS(QObject *, QObject *);
    FITS(const QObject *, const QObject *);

    FITS(std::nullptr_t, std::nullptr_t);

    // classes with conversion operators undergoing implicit conversions
    struct ConvertingToDouble {
        /* implicit */ operator double() const { return 42.0; }
    };

    NARROWS(ConvertingToDouble, char);
    NARROWS(ConvertingToDouble, short);
    NARROWS(ConvertingToDouble, int);
    NARROWS(ConvertingToDouble, long);
    NARROWS(ConvertingToDouble, long long);
    NARROWS(ConvertingToDouble, float);
    FITS(ConvertingToDouble, double);
    FITS(ConvertingToDouble, long double);


    // no compiler still implements this properly.
#if 0
    struct ConstructibleFromInt {
        /* implicit */ ConstructibleFromInt(int) {}
    };

    FITS(char, ConstructibleFromInt);
    FITS(short, ConstructibleFromInt);
    FITS(int, ConstructibleFromInt);
    NARROWS(unsigned int, ConstructibleFromInt);
    NARROWS_IF(long, ConstructibleFromInt, sizeof(long) > sizeof(int));
    NARROWS_IF(long long, ConstructibleFromInt, sizeof(long long) > sizeof(int));
    NARROWS(float, ConstructibleFromInt);
    NARROWS(double, ConstructibleFromInt);
#endif

    // forward declared classes must work
    class ForwardDeclared;
    FITS(ForwardDeclared, ForwardDeclared);

#if 0 // waiting for official compiler releases that implement P1957...
    {
        // wg21.link/P1957
        NARROWS(char*, bool);
        NARROWS(void (QObject::*)(), bool);
    }
#endif

#undef NARROWS_IF
#undef FITS_IF
#undef NARROWS
#undef FITS
}

void tst_QObject::nullReceiver()
{
    QObject o;
    QObject *nullObj = nullptr; // Passing nullptr directly doesn't compile with gcc 4.8
    QVERIFY(!connect(&o, &QObject::destroyed, nullObj, &QObject::deleteLater));
    QVERIFY(!connect(&o, &QObject::destroyed, nullObj, [] {}));
    QVERIFY(!connect(&o, &QObject::destroyed, nullObj, Functor_noexcept()));
    QVERIFY(!connect(&o, SIGNAL(destroyed()), nullObj, SLOT(deleteLater())));
}

void tst_QObject::functorReferencesConnection()
{
    countedStructObjectsCount = 0;
    QMetaObject::Connection globalCon;
    {
        GetSenderObject obj;
        CountedStruct counted(&obj);
        QCOMPARE(countedStructObjectsCount, 1);
        auto c = QSharedPointer<QMetaObject::Connection>::create();
        int slotCalled = 0;
        *c = connect(&obj, &GetSenderObject::aSignal, &obj, [&slotCalled, c, counted] {
            QObject::disconnect(*c);
            slotCalled++;
        });
        globalCon = *c; // keep a handle to the connection somewhere;
        QVERIFY(globalCon);
        QCOMPARE(countedStructObjectsCount, 2);
        obj.triggerSignal();
        QCOMPARE(slotCalled, 1);
        QCOMPARE(countedStructObjectsCount, 1);
        QVERIFY(!globalCon);
        obj.triggerSignal();
        QCOMPARE(slotCalled, 1);
        QCOMPARE(countedStructObjectsCount, 1);
    }
    QCOMPARE(countedStructObjectsCount, 0);

    {
        GetSenderObject obj;
        CountedStruct counted(&obj);
        QCOMPARE(countedStructObjectsCount, 1);
        auto *rec = new QObject;
        int slotCalled = 0;
        globalCon = connect(&obj, &GetSenderObject::aSignal, rec, [&slotCalled, rec, counted] {
            delete rec;
            slotCalled++;
        });
        QCOMPARE(countedStructObjectsCount, 2);
        obj.triggerSignal();
        QCOMPARE(slotCalled, 1);
        QCOMPARE(countedStructObjectsCount, 1);
        QVERIFY(!globalCon);
        obj.triggerSignal();
        QCOMPARE(slotCalled, 1);
        QCOMPARE(countedStructObjectsCount, 1);
    }
    QCOMPARE(countedStructObjectsCount, 0);
    {
        int slotCalled = 0;
        QEventLoop eventLoop;
        {
            // Sender will be destroyed when the labda goes out of scope lambda, so it will exit the event loop
            auto sender = QSharedPointer<GetSenderObject>::create();
            connect(sender.data(), &QObject::destroyed, &eventLoop, &QEventLoop::quit, Qt::QueuedConnection);
            globalCon = connect(sender.data(), &GetSenderObject::aSignal, this, [&slotCalled, sender, &globalCon, this] {
                ++slotCalled;
                // This signal will be connected, but should never be called as the sender will be destroyed before
                auto c2 = connect(sender.data(), &GetSenderObject::aSignal, [] { QFAIL("Should not be called"); });
                QVERIFY(c2);
                QVERIFY(QObject::disconnect(sender.data(), nullptr, this, nullptr));
                QVERIFY(!globalCon); // this connection has been disconnected
                QVERIFY(c2); // sender should not have been deleted yet, only after the emission is done
            });
            QMetaObject::invokeMethod(sender.data(), &GetSenderObject::triggerSignal, Qt::QueuedConnection);
            QMetaObject::invokeMethod(sender.data(), &GetSenderObject::triggerSignal, Qt::QueuedConnection);
            QMetaObject::invokeMethod(sender.data(), &GetSenderObject::triggerSignal, Qt::QueuedConnection);
        }
        eventLoop.exec();
        QCOMPARE(slotCalled, 1);
    }

    {
        GetSenderObject obj;
        CountedStruct counted(&obj);
        QCOMPARE(countedStructObjectsCount, 1);
        auto c1 = QSharedPointer<QMetaObject::Connection>::create();
        auto c2 = QSharedPointer<QMetaObject::Connection>::create();
        int slot1Called = 0;
        int slot3Called = 0;
        *c1 = connect(&obj, &GetSenderObject::aSignal, &obj, [&slot1Called, &slot3Called, &obj, c1, c2, counted] {
            auto c3 = connect(&obj, &GetSenderObject::aSignal, [counted, &slot3Called] {
                slot3Called++;
            });
            // top-level + the one in the 3 others lambdas
            QCOMPARE(countedStructObjectsCount, 4);
            QObject::disconnect(*c2);
            slot1Called++;
        });
        connect(&obj, &GetSenderObject::aSignal, [] {}); // just a dummy signal to fill the connection list
        *c2 = connect(&obj, &GetSenderObject::aSignal, [counted, c2] { QFAIL("should not be called"); });
        QVERIFY(c1 && c2);
        QCOMPARE(countedStructObjectsCount, 3); // top-level + c1 + c2
        obj.triggerSignal();
        QCOMPARE(slot1Called, 1);
        QCOMPARE(slot3Called, 0);
        QCOMPARE(countedStructObjectsCount, 3); // top-level + c1 + c3
        QObject::disconnect(*c1);
        QCOMPARE(countedStructObjectsCount, 2); // top-level + c3
        obj.triggerSignal();
        QCOMPARE(slot1Called, 1);
        QCOMPARE(slot3Called, 1);
    }
    {
        struct DestroyEmit {
            Q_DISABLE_COPY(DestroyEmit);
            explicit DestroyEmit(SenderObject *obj) : obj(obj) {}
            SenderObject *obj;
            ~DestroyEmit() {
                obj->emitSignal1();
            }
        };
        SenderObject obj;
        int slot1Called = 0;
        int slot2Called = 0;
        int slot3Called = 0;
        auto c1 = QSharedPointer<QMetaObject::Connection>::create();
        auto de = QSharedPointer<DestroyEmit>::create(&obj);
        *c1 = connect(&obj, &SenderObject::signal1, [&slot1Called, &slot3Called, de, c1, &obj] {
            connect(&obj, &SenderObject::signal1, [&slot3Called] { slot3Called++; });
            slot1Called++;
            QObject::disconnect(*c1);
        });
        de.clear();
        connect(&obj, &SenderObject::signal1, [&slot2Called] { slot2Called++; });
        obj.emitSignal1();
        QCOMPARE(slot1Called, 1);
        QCOMPARE(slot2Called, 2); // because also called from ~DestroyEmit
        QCOMPARE(slot3Called, 1);
    }
}

void tst_QObject::disconnectDisconnects()
{
    // Test what happens if the destructor of an functor slot also disconnects more slot;

    SenderObject s1;
    QScopedPointer<QObject> receiver(new QObject);

    auto s2 = QSharedPointer<SenderObject>::create();
    QPointer<QObject> s2_tracker = s2.data();
    int count = 0;
    connect(&s1, &SenderObject::signal1, [&count] { count++; }); // α
    connect(&s1, &SenderObject::signal1, receiver.data(), [s2] { QFAIL("!!"); }); // β
    connect(s2.data(), &SenderObject::signal1, receiver.data(), [] { QFAIL("!!"); });
    connect(&s1, &SenderObject::signal2, receiver.data(), [] { QFAIL("!!"); });
    connect(s2.data(), &SenderObject::signal2, receiver.data(), [] { QFAIL("!!"); });
    connect(&s1, &SenderObject::signal1, [&count] { count++; }); // γ
    connect(&s1, &SenderObject::signal2, [&count] { count++; }); // δ
    s2.clear();

    QVERIFY(s2_tracker);
    receiver
        .reset(); // this will delete the receiver which must also delete s2 as β is disconnected
    QVERIFY(!s2_tracker);
    // test that the data structures are still in order
    s1.emitSignal1();
    QCOMPARE(count, 2); // α + γ
    s1.emitSignal2();
    QCOMPARE(count, 3); // + δ
}

class ReceiverDisconnecting : public QObject
{
    W_OBJECT(ReceiverDisconnecting)

public:
    SenderObject *sender;
    int slotCalledCount = 0;

public slots:
    void aSlotByName()
    {
        ++slotCalledCount;
        QVERIFY(!disconnect(sender, SIGNAL(signal1()), this, SLOT(aSlotByName())));
    }
    W_SLOT(aSlotByName)

    void aSlotByPtr()
    {
        ++slotCalledCount;
        QVERIFY(!disconnect(sender, &SenderObject::signal1, this, &ReceiverDisconnecting::aSlotByPtr));
    }
    W_SLOT(aSlotByPtr)
};

class DeleteThisReceiver : public QObject
{
    W_OBJECT(DeleteThisReceiver)

public:
    static int counter;

public slots:
    void deleteThis()
    {
        ++counter;
        delete this;
    }
    W_SLOT(deleteThis)
};

int DeleteThisReceiver::counter = 0;

void tst_QObject::singleShotConnection()
{
    {
        // Non single shot behavior: slot called every time the signal is emitted
        SenderObject sender;
        QMetaObject::Connection c = connect(&sender, &SenderObject::signal1,
                                            &sender, &SenderObject::aPublicSlot);
        QVERIFY(c);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 1);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 2);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 3);
    }

    {
        // Non single shot behavior: multiple connections cause multiple invocations
        SenderObject sender;
        QMetaObject::Connection c = connect(&sender, &SenderObject::signal1,
                                            &sender, &SenderObject::aPublicSlot);
        QVERIFY(c);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QVERIFY(c);
        QCOMPARE(sender.aPublicSlotCalled, 1);

        sender.emitSignal1();
        QVERIFY(c);
        QCOMPARE(sender.aPublicSlotCalled, 2);

        QMetaObject::Connection c2 = connect(&sender, &SenderObject::signal1,
                                            &sender, &SenderObject::aPublicSlot);
        QVERIFY(c);
        QVERIFY(c2);
        QCOMPARE(sender.aPublicSlotCalled, 2);

        sender.emitSignal1();
        QVERIFY(c);
        QVERIFY(c2);
        QCOMPARE(sender.aPublicSlotCalled, 4);

        sender.emitSignal1();
        QVERIFY(c);
        QVERIFY(c2);
        QCOMPARE(sender.aPublicSlotCalled, 6);
    }

    {
        // Single shot behavior: slot called only once
        SenderObject sender;
        QMetaObject::Connection c = connect(&sender, &SenderObject::signal1,
                                            &sender, &SenderObject::aPublicSlot,
                                            static_cast<Qt::ConnectionType>(Qt::SingleShotConnection));
        QVERIFY(c);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QVERIFY(!c);
        QCOMPARE(sender.aPublicSlotCalled, 1);

        sender.emitSignal1();
        QVERIFY(!c);
        QCOMPARE(sender.aPublicSlotCalled, 1);

        sender.emitSignal1();
        QVERIFY(!c);
        QCOMPARE(sender.aPublicSlotCalled, 1);
    }

    {
        // Same, without holding a Connection object
        SenderObject sender;
        bool ok = connect(&sender, &SenderObject::signal1,
                          &sender, &SenderObject::aPublicSlot,
                          static_cast<Qt::ConnectionType>(Qt::SingleShotConnection));
        QVERIFY(ok);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 1);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 1);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 1);
    }

    {
        // Single shot, disconnect before emitting
        SenderObject sender;
        QMetaObject::Connection c = connect(&sender, &SenderObject::signal1,
                                            &sender, &SenderObject::aPublicSlot,
                                            static_cast<Qt::ConnectionType>(Qt::SingleShotConnection));
        QVERIFY(c);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        QVERIFY(QObject::disconnect(c));
        QVERIFY(!c);

        sender.emitSignal1();
        QVERIFY(!c);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QVERIFY(!c);
        QCOMPARE(sender.aPublicSlotCalled, 0);
    }

    {
        // Single shot together with another connection
        SenderObject sender;
        QVERIFY(connect(&sender, &SenderObject::signal1,
                        &sender, &SenderObject::aPublicSlot));
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 1);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 2);

        QVERIFY(connect(&sender, &SenderObject::signal1,
                        &sender, &SenderObject::aPublicSlot,
                        static_cast<Qt::ConnectionType>(Qt::SingleShotConnection)));
        QCOMPARE(sender.aPublicSlotCalled, 2);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 4);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 5);
    }

    {
        // Two single shot, from the same signal, to the same slot
        SenderObject sender;
        QVERIFY(connect(&sender, &SenderObject::signal1,
                        &sender, &SenderObject::aPublicSlot,
                        static_cast<Qt::ConnectionType>(Qt::SingleShotConnection)));
        QVERIFY(connect(&sender, &SenderObject::signal1,
                        &sender, &SenderObject::aPublicSlot,
                        static_cast<Qt::ConnectionType>(Qt::SingleShotConnection)));

        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 2);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 2);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 2);
    }

    {
        // Two single shot, from different signals, to the same slot
        SenderObject sender;
        QVERIFY(connect(&sender, &SenderObject::signal1,
                        &sender, &SenderObject::aPublicSlot,
                        static_cast<Qt::ConnectionType>(Qt::SingleShotConnection)));
        QVERIFY(connect(&sender, &SenderObject::signal2,
                        &sender, &SenderObject::aPublicSlot,
                        static_cast<Qt::ConnectionType>(Qt::SingleShotConnection)));

        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 1);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 1);

        sender.emitSignal2();
        QCOMPARE(sender.aPublicSlotCalled, 2);

        sender.emitSignal2();
        QCOMPARE(sender.aPublicSlotCalled, 2);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 2);
    }

    {
        // Same signal, different connections
        SenderObject sender;
        ReceiverObject receiver1, receiver2;
        receiver1.reset();
        receiver2.reset();

        QVERIFY(connect(&sender, &SenderObject::signal1,
                        &receiver1, &ReceiverObject::slot1));
        QVERIFY(connect(&sender, &SenderObject::signal1,
                        &receiver2, &ReceiverObject::slot1,
                        static_cast<Qt::ConnectionType>(Qt::SingleShotConnection)));
        QCOMPARE(receiver1.count_slot1, 0);
        QCOMPARE(receiver2.count_slot1, 0);

        sender.emitSignal1();
        QCOMPARE(receiver1.count_slot1, 1);
        QCOMPARE(receiver2.count_slot1, 1);

        sender.emitSignal1();
        QCOMPARE(receiver1.count_slot1, 2);
        QCOMPARE(receiver2.count_slot1, 1);

        sender.emitSignal1();
        QCOMPARE(receiver1.count_slot1, 3);
        QCOMPARE(receiver2.count_slot1, 1);

        // Reestablish a single shot
        QVERIFY(connect(&sender, &SenderObject::signal1,
                        &receiver2, &ReceiverObject::slot1,
                        static_cast<Qt::ConnectionType>(Qt::SingleShotConnection)));
        QCOMPARE(receiver1.count_slot1, 3);
        QCOMPARE(receiver2.count_slot1, 1);

        sender.emitSignal1();
        QCOMPARE(receiver1.count_slot1, 4);
        QCOMPARE(receiver2.count_slot1, 2);

        sender.emitSignal1();
        QCOMPARE(receiver1.count_slot1, 5);
        QCOMPARE(receiver2.count_slot1, 2);
    }

    {
        // Check that the slot is invoked with the connection already disconnected
        SenderObject sender;
        QMetaObject::Connection c;
        auto breakSlot = [&]() {
            QVERIFY(!c);
            ++sender.aPublicSlotCalled;
        };

        c = connect(&sender, &SenderObject::signal1,
                    &sender, breakSlot,
                    static_cast<Qt::ConnectionType>(Qt::SingleShotConnection));

        QVERIFY(c);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 1);
        QVERIFY(!c);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 1);
        QVERIFY(!c);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 1);
        QVERIFY(!c);
    }

    {
        // Same
        SenderObject sender;
        ReceiverDisconnecting receiver;
        receiver.sender = &sender;
        bool ok = connect(&sender, SIGNAL(signal1()),
                          &receiver, SLOT(aSlotByName()),
                          static_cast<Qt::ConnectionType>(Qt::SingleShotConnection));
        QVERIFY(ok);
        QCOMPARE(receiver.slotCalledCount, 0);

        sender.emitSignal1();
        QCOMPARE(receiver.slotCalledCount, 1);

        sender.emitSignal1();
        QCOMPARE(receiver.slotCalledCount, 1);

        // reconnect
        ok = connect(&sender, SIGNAL(signal1()),
                     &receiver, SLOT(aSlotByName()),
                     static_cast<Qt::ConnectionType>(Qt::SingleShotConnection));
        QVERIFY(ok);
        QCOMPARE(receiver.slotCalledCount, 1);

        sender.emitSignal1();
        QCOMPARE(receiver.slotCalledCount, 2);

        sender.emitSignal1();
        QCOMPARE(receiver.slotCalledCount, 2);
    }

    {
        // Same
        SenderObject sender;
        ReceiverDisconnecting receiver;
        receiver.sender = &sender;
        bool ok = connect(&sender, &SenderObject::signal1,
                          &receiver, &ReceiverDisconnecting::aSlotByPtr,
                          static_cast<Qt::ConnectionType>(Qt::SingleShotConnection));

        QVERIFY(ok);
        QCOMPARE(receiver.slotCalledCount, 0);

        sender.emitSignal1();
        QCOMPARE(receiver.slotCalledCount, 1);

        sender.emitSignal1();
        QCOMPARE(receiver.slotCalledCount, 1);

        // reconnect
        ok = connect(&sender, &SenderObject::signal1,
                     &receiver, &ReceiverDisconnecting::aSlotByPtr,
                     static_cast<Qt::ConnectionType>(Qt::SingleShotConnection));
        QVERIFY(ok);
        QCOMPARE(receiver.slotCalledCount, 1);

        sender.emitSignal1();
        QCOMPARE(receiver.slotCalledCount, 2);

        sender.emitSignal1();
        QCOMPARE(receiver.slotCalledCount, 2);
    }

    {
        // Reconnect from inside the slot
        SenderObject sender;
        std::function<void()> reconnectingSlot;
        bool reconnect = false;
        reconnectingSlot = [&]() {
            ++sender.aPublicSlotCalled;
            if (reconnect) {
                QObject::connect(&sender, &SenderObject::signal1,
                                 &sender, reconnectingSlot,
                                 static_cast<Qt::ConnectionType>(Qt::SingleShotConnection));
            }
        };

        bool ok = connect(&sender, &SenderObject::signal1,
                          &sender, reconnectingSlot,
                          static_cast<Qt::ConnectionType>(Qt::SingleShotConnection));
        QVERIFY(ok);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 1);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 1);

        reconnect = true;
        ok = connect(&sender, &SenderObject::signal1,
                                  &sender, reconnectingSlot,
                                  static_cast<Qt::ConnectionType>(Qt::SingleShotConnection));
        QVERIFY(ok);
        QCOMPARE(sender.aPublicSlotCalled, 1);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 2);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 3);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 4);

        reconnect = false;
        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 5);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 5);
    }

    {
        // Delete the receiver from inside the slot
        SenderObject sender;
        QPointer<DeleteThisReceiver> p = new DeleteThisReceiver;
        DeleteThisReceiver::counter = 0;

        QVERIFY(connect(&sender, &SenderObject::signal1,
                        p.get(), &DeleteThisReceiver::deleteThis,
                        static_cast<Qt::ConnectionType>(Qt::SingleShotConnection)));

        QVERIFY(p);
        QCOMPARE(DeleteThisReceiver::counter, 0);

        sender.emitSignal1();
        QCOMPARE(DeleteThisReceiver::counter, 1);
        QVERIFY(!p);

        sender.emitSignal1();
        QCOMPARE(DeleteThisReceiver::counter, 1);
        QVERIFY(!p);
    }

    {
        // Queued, non single shot
        SenderObject sender;
        QVERIFY(connect(&sender, &SenderObject::signal1,
                        &sender, &SenderObject::aPublicSlot,
                        static_cast<Qt::ConnectionType>(Qt::QueuedConnection)));
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 0);

        QTRY_COMPARE(sender.aPublicSlotCalled, 3);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 3);

        QTRY_COMPARE(sender.aPublicSlotCalled, 4);
    }

    {
        // Queued, single shot
        SenderObject sender;
        QVERIFY(connect(&sender, &SenderObject::signal1,
                        &sender, &SenderObject::aPublicSlot,
                        static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::SingleShotConnection)));
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 0);

        QTRY_COMPARE(sender.aPublicSlotCalled, 1);
        QTest::qWait(0);
        QCOMPARE(sender.aPublicSlotCalled, 1);
    }

    {
        // Queued, single shot, checking the connection handle
        SenderObject sender;
        QMetaObject::Connection c = connect(&sender, &SenderObject::signal1,
                                            &sender, &SenderObject::aPublicSlot,
                                            static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::SingleShotConnection));
        QVERIFY(c);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QVERIFY(!c);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QVERIFY(!c);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QVERIFY(!c);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        QTRY_COMPARE(sender.aPublicSlotCalled, 1);
        QVERIFY(!c);
        QTest::qWait(0);
        QVERIFY(!c);
        QCOMPARE(sender.aPublicSlotCalled, 1);
    }

    {
        // Queued, single shot, disconnect before emitting
        SenderObject sender;
        QVERIFY(connect(&sender, &SenderObject::signal1,
                        &sender, &SenderObject::aPublicSlot,
                        static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::SingleShotConnection)));
        QCOMPARE(sender.aPublicSlotCalled, 0);

        QVERIFY(QObject::disconnect(&sender, &SenderObject::signal1,
                                    &sender, &SenderObject::aPublicSlot));

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QCOMPARE(sender.aPublicSlotCalled, 0);

        QTest::qWait(0);
        QCOMPARE(sender.aPublicSlotCalled, 0);
    }

    {
        // Queued, single shot, disconnect before emitting by using the connection handle
        SenderObject sender;
        QMetaObject::Connection c = connect(&sender, &SenderObject::signal1,
                                            &sender, &SenderObject::aPublicSlot,
                                            static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::SingleShotConnection));
        QVERIFY(c);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        QVERIFY(QObject::disconnect(c));
        QVERIFY(!c);

        sender.emitSignal1();
        QVERIFY(!c);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        sender.emitSignal1();
        QVERIFY(!c);
        QCOMPARE(sender.aPublicSlotCalled, 0);

        QTest::qWait(0);
        QVERIFY(!c);
        QCOMPARE(sender.aPublicSlotCalled, 0);
    }

    {
        // Queued, single shot, delete the receiver from inside the slot
        SenderObject sender;
        QPointer<DeleteThisReceiver> p = new DeleteThisReceiver;
        DeleteThisReceiver::counter = 0;

        QVERIFY(connect(&sender, &SenderObject::signal1,
                        p.get(), &DeleteThisReceiver::deleteThis,
                        static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::SingleShotConnection)));
        QCOMPARE(DeleteThisReceiver::counter, 0);

        sender.emitSignal1();
        QVERIFY(p);
        QCOMPARE(DeleteThisReceiver::counter, 0);

        sender.emitSignal1();
        QVERIFY(p);
        QCOMPARE(DeleteThisReceiver::counter, 0);

        sender.emitSignal1();
        QVERIFY(p);
        QCOMPARE(DeleteThisReceiver::counter, 0);

        QTRY_COMPARE(DeleteThisReceiver::counter, 1);
        QVERIFY(!p);
        QTest::qWait(0);
        QCOMPARE(DeleteThisReceiver::counter, 1);
        QVERIFY(!p);
    }
}

// Test for QtPrivate::HasQ_OBJECT_Macro
static_assert(QtPrivate::HasQ_OBJECT_Macro<tst_QObject>::Value);
static_assert(!QtPrivate::HasQ_OBJECT_Macro<SiblingDeleter>::Value);

QTEST_MAIN(tst_QObject)

W_OBJECT_IMPL(tst_QObject)
W_OBJECT_IMPL(SenderObject)
W_OBJECT_IMPL(ReceiverObject)
W_OBJECT_IMPL(AutoConnectSender)
W_OBJECT_IMPL(AutoConnectReceiver)
W_OBJECT_IMPL(QObjectWithIncomplete)
W_OBJECT_IMPL(ConnectByNameNotifySenderObject)
W_OBJECT_IMPL(ConnectByNameNotifyReceiverObject)
W_OBJECT_IMPL(ConnectDisconnectNotifyShadowObject)
W_OBJECT_IMPL(SequenceObject)
W_OBJECT_IMPL(QCustomTypeChecker)
W_OBJECT_IMPL(PropertyObject)
W_OBJECT_IMPL(TestThread)
W_OBJECT_IMPL(MoveToThreadObject)
W_OBJECT_IMPL(QObjectTest::TestObject)
W_OBJECT_IMPL(SuperObject)
W_OBJECT_IMPL(FooObject)
W_OBJECT_IMPL(BlehObject)
W_OBJECT_IMPL(DestroyedListener)
W_OBJECT_IMPL(DefaultArguments)
W_OBJECT_IMPL(NormalizeObject)
W_OBJECT_IMPL(EventSpy)
W_OBJECT_IMPL(EmitThread)
W_OBJECT_IMPL(QObjectTest::DeleteObject)
W_OBJECT_IMPL(DisconnectObject)
W_OBJECT_IMPL(ConnectToSender)
W_OBJECT_IMPL(OverloadObject)
W_OBJECT_IMPL(ManySignals)
W_OBJECT_IMPL(ConfusingObject)
W_OBJECT_IMPL(Constructable)
W_OBJECT_IMPL(BaseDestroyed)
W_OBJECT_IMPL(LotsOfSignalsAndSlots)
W_OBJECT_IMPL(StringVariant)
W_OBJECT_IMPL(ConnectWithReferenceObject)
W_OBJECT_IMPL(ManyArgumentObject)
W_OBJECT_IMPL(ForwardDeclareArguments)
W_GADGET_IMPL(NoDefaultConstructor)
W_OBJECT_IMPL(NoDefaultContructorArguments)
W_OBJECT_IMPL(ReturnValue)
W_OBJECT_IMPL(VirtualSlotsObjectBase)
W_OBJECT_IMPL(VirtualSlotsObject)
#ifdef QT_BUILD_INTERNAL
W_OBJECT_IMPL(ConnectToPrivateSlot)
#endif
W_OBJECT_IMPL(ContextObject)
W_OBJECT_IMPL(StaticSlotChecker)
W_OBJECT_IMPL(FunctorArgDifferenceObject)
W_OBJECT_IMPL(GetSenderObject)
W_OBJECT_IMPL(SubSender)
W_OBJECT_IMPL(CountedExceptionThrower)
W_OBJECT_IMPL(ReceiverDisconnecting)
W_OBJECT_IMPL(ExceptionThrower)
W_OBJECT_IMPL(DeleteThisReceiver)
