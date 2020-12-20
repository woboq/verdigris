import qbs

Project {
    references: [
        "signalbug"
    ]

    Application {
        name: "qobject"
        consoleApplication: true
        type: ["application", "autotest"]

        Depends { name: "cpp" }
        cpp.defines: base.concat("QT_DISABLE_DEPRECATED_BEFORE=0", "QT_NO_DEBUG")
        Depends { name: "Verdigris" }
        Depends { name: "Qt.network" }
        Depends { name: "Qt.test" }
        Depends { name: "signalbug" }

        Group {
            name: "qt5"
            condition: Qt.core.versionMajor < 6
            // fileTags: ["unmocable"]
            // overrideTags: false
            files: [
                "tst_qobject5.cpp",
            ]
        }
        Group {
            name: "qt6"
            condition: Qt.core.versionMajor >= 6
            // fileTags: ["unmocable"]
            // overrideTags: false
            files: [
                "tst_qobject6.cpp",
            ]
        }
    }

}
