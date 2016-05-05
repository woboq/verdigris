// I'm looking for feedback on how to declare signals with verdigris.
// (verdigris introduction:  https://salt.woboq.im/~codebrowser/verdigris/verdigris/tutorial/tutorial.cpp.html

#if !1 //pseudo code.
class MyClass : public QObject {

public:
    // For reference, this is how you declare a slot with copperspice:
    CS_SLOT_1(Public, void closePB(QObject *someObject = 0) )
    CS_SLOT_2(closePB)

    // This is how you do it in verdigris
    void closePB(QObject *someObject = 0); // normal declaration
    W_SLOT(closePB)

    // Also possible to put the implementation inline (which is not possible wih CopperSpice
    void closePB(QObject *someObject) { m_pb->close(someObject) }
    W_SLOT(closePB)

public:
    // Anyway now let's move to signals.
    //Here is how it works with CopperSpice:
    CS_SIGNAL_1(Public, void clicked(bool checked) )
    CS_SIGNAL_2(clicked, checked)
    // Pretty consistant with CS_SLOT, only you need to add the name of the argument in the second macro.

    //The question is how should we do with signals.

    // Option 1:  almost consistant with W_SLOT
    void clicked(bool checked)    // no semicolon!
    W_SIGNAL(clicked, checked)
    // this looks like W_SLOT but does not because there must not be a semicolon after the declaration.
    // The reason is that technicly, W_SIGNAL contains the implementation of the signal.
    // (i.e: W_SIGNAL starts with '{')

    // Option 2: do it like copperspice
    W_SIGNAL_1(void clicked(bool checked))
    W_SIGNAL_2(clicked, checked)
    // This option then justify the lack of semicolon
    // But then it's inconsistant with W_SLOT. Is that a problem?
    // It also look not so nice.

    // Option 3: Since W_SIGNAL starts with { and  we could also tell to put the macro inside the body.
    void clicked(bool checked)
    { W_SIGNAL(clicked, checked) }
    // Now the lack of semicolon is not a problem because of the curly braces, but it's also a
    // wierd way to declare a signal.
    // And the body must be empty with the exception of the W_SIGNAL macro.
    // Also it is not allowed to put that outside of the class definition in the .cpp file or so.


    // In every cases, if the user add a semicolon too many, or do something wrong, the error message
    // will be a very ugly messages about template something being wrong in the instentation of the macro.

    // So I need your help to tell me what is the best alternative.
    // Option 1  is the simplest to write but the fact that you must omit the semicolon in annoying.
    // Option 2 looks ugly but is what copperspice do.
    // Option 3 looks nice but is strange for the declaration of a signal.

    // What do you say?
};

#endif
