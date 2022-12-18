
Application {
    name: "cpp_api_tutorial"
    consoleApplication: true

    Depends { name: "Verdigris" }
    Depends { name: "Qt.quick" }

    files: [
        "main.cpp",
        "main.qml",
        "resources.qrc",
    ]
}
