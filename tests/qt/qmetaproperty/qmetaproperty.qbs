import qbs

Application {
    name: "qmetaproperty"
    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.test" }

    files: [
        "tst_qmetaproperty.cpp",
    ]
}
