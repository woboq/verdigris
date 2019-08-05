import qbs

Project {

    Application {
        name: "tutorial"
        consoleApplication: true

        Depends { name: "Verdigris" }

        files: [
            "tutorial.cpp"
        ]
    }

    Application {
        name: "cpp_tutorial"
        consoleApplication: true

        Depends { name: "Verdigris" }
        Depends { name: "Qt.quick" }

        files: [
            "cpp_tutorial.cpp",
            "cpp_tutorial.qml",
            "cpp_tutorial.qrc",
        ]
    }
}
