import qbs

Application {
    name: "manyproperties"
    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.test" }

    files: [
        "tst_manyproperties.cpp",
    ]
}
