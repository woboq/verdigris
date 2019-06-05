import qbs

Application {
    name: "tutorial"
    consoleApplication: true
    type: ["application"]

    Depends { name: "Verdigris" }

    files: [
        "tutorial.cpp"
    ]
}
