import qbs

Application {
    name: "qproperty"
    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.core-private" }
    Depends { name: "Qt.test" }

    files: [
        "tst_qproperty6.cpp",
    ]
}
