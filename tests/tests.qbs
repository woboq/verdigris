import qbs

Project {
    name: "tests"

    references: [
        "internal",
        "basic",
        "qt",
        "templates",
        "manyproperties"
    ]
}
