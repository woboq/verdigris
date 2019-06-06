import qbs

Application {
    name: "qobject_bench"
    consoleApplication: true
    type: ["application"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.test" }

    files: [
        "main.cpp",
        "object.cpp",
        "object.h",
    ]
}
