import qbs

Application {
    name: "qmetaobject"
    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.test" }

    Group {
        name: "qt5"
        condition: Qt.core.versionMajor < 6
        files: [
            "tst_qmetaobject5.cpp",
        ]
    }
    Group {
        name: "qt6"
        condition: Qt.core.versionMajor >= 6
        files: [
            "tst_qmetaobject6.cpp",
        ]
    }
}
