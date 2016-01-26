/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SIGNAL_BUG_H
#define SIGNAL_BUG_H


#include <QObject>
#include <wobjectdefs.h>

class Sender;


class Receiver : public QObject
{
    W_OBJECT(Receiver)

public:
    Receiver ();
    virtual ~Receiver () {}

protected slots:
    void received ();
    W_SLOT(received)

public:
    Sender *s;
};

class Disconnector : public QObject
{
    W_OBJECT(Disconnector)

public:
    Disconnector ();
    virtual ~Disconnector () {}

protected slots:
    void received ();
    W_SLOT(received)

public:
    Sender *s;
};

class Sender : public QObject
{
    W_OBJECT(Sender)

public:
    Sender (Receiver *r, Disconnector *d);
    virtual ~Sender () {}

    void fire ();
    W_SLOT(fire)

signals:
    W_SIGNAL_1(void fired ())
    W_SIGNAL_2(fired)

public:
    Receiver *r;
    Disconnector *d;
};

#endif  // SIGNAL_BUG_H
