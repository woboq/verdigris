#pragma once
#include "api1.h"
#include "api2.h"
#include "wobjectdefs.h"

#include <QObject>

class Object : public QObject, public Api1, public Api2 {
    W_OBJECT(Object)

public:
    Object(QObject* parent = {});

    void emit1();
    void emit2();
    void emit3();

    // Api1 interface
    inline void handleEvent1() W_SIGNAL(handleEvent1);

    // Api2 interface
    inline void handleEvent2() W_SIGNAL(handleEvent2);
    inline void handleEvent3() W_SIGNAL(handleEvent3);
};
