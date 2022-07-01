import qbs

Application {
    name: "inherit"
    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.test" }

    files: [
        "api1.h",
        "api2.h",
        "object.cpp",
        "object.h",
        "tst_inherit.cpp",
    ]
}
