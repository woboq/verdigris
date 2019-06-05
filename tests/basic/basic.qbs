import qbs

Application {
    name: "basic"
    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.test" }

    Group {
        name: "source"
        fileTags: ["unmocable"]
        overrideTags: false
        files: [
            "anothertu.cpp",
            "tst_basic.cpp",
            "anothertu.h",
        ]
    }
}
