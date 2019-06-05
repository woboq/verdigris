import qbs


Application {
    name: "signalbug"
    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.test" }

    files: [
        "signalbug.cpp",
        "signalbug.h",
    ]
}
