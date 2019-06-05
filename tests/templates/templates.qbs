import qbs

Application {
    name: "templates"
    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "Verdigris" }
    Depends { name: "Qt.test" }

    files: [
        "tst_templates.cpp",
    ]
}
