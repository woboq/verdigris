import qbs

Application {
    name: "cppapi"
    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.test" }

    files: [
        "tst_cppapi.cpp",
    ]
}
