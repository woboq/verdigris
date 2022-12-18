import qbs

Project {
    name: "tutorial"

    references: [
        "features",
        "cpp_api",
        // QML Element does not work with Qbs (PR welcome)
        // "qml_element",
    ]
}
