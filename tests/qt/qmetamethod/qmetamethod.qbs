import qbs

Application {
    name: "qmetamethod"
    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.test" }

    files: [
        "tst_qmetamethod.cpp",
    ]
}
