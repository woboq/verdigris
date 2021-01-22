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

        Properties {
            condition: qbs.toolchain.contains('msvc')
            cpp.cxxFlags: base.concat(
                "/wd4573", // vs2019 warns about static connect/disconnect in lambdas
                "/wd4340" // vs2019 warns about wrapped enum value
            )
        }

        Group {
            name: "qt5"
            condition: Qt.core.versionMajor < 6
            files: [
                "tst_qobject5.cpp",
            ]
        }
        Group {
            name: "qt6"
            condition: Qt.core.versionMajor >= 6
            files: [
                "tst_qobject6.cpp",
            ]
        }
    }

}
