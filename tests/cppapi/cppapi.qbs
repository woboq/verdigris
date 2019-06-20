import qbs

Application {
    name: "cppapi"
    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.test" }
    Depends { name: "cpp" }
    cpp.cxxLanguageVersion: "c++17"

    files: [
        "tst_cppapi.cpp",
    ]
}
