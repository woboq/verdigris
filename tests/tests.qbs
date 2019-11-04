import qbs

Project {
    name: "tests"

    references: [
        "basic",
        "cppapi",
        "internal",
        "manyproperties",
        "qt",
        "templates",
    ]
}
