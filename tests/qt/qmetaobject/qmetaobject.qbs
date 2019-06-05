import qbs

Application {
    name: "qmetaobject"
    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.test" }

    files: [
        "tst_qmetaobject.cpp",
    ]
}
