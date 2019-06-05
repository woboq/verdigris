import qbs

Application {
    name: "qmetaenum"
    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.test" }

    files: [
        "tst_qmetaenum.cpp",
    ]
}
