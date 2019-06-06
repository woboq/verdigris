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
        cpp.defines: base.concat("QT_DISABLE_DEPRECATED_BEFORE=0")
        Depends { name: "Verdigris" }
        Depends { name: "Qt.network" }
        Depends { name: "Qt.test" }

        files: [
            "tst_qobject.cpp",
        ]
    }

}
