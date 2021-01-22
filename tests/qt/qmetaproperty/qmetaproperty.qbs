import qbs

Application {
    name: "qmetaproperty"
    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.test" }

    Group {
        name: "qt5"
        condition: Qt.core.versionMajor < 6
        files: [
            "tst_qmetaproperty5.cpp",
        ]
    }
    Group {
        name: "qt6"
        condition: Qt.core.versionMajor >= 6
        files: [
            "tst_qmetaproperty6.cpp",
        ]
    }
}
