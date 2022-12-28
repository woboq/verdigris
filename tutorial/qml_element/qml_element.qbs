
// Background: This does not work out of the box!
// * Qbs orders rules by input and output tags
// * We have a circle in tags: `cpp` -[compile]-> `obj` -[extractor]-> `metatypes.json` -[registar]-> `cpp`
// * `excludedInputs` excludes all rules that may produce the given tags
// * There is no known way to restrict rules to certain inputs

// As a workaround we use two products:
// 1st Product with output type `obj`
//     - add sources with QML_ELEMENTS here
//     - `cpp` -[compile]-> `obj` -[extractor]-> `verdigris.metatypes`
// 2nd Library/Application
//     - setup for QML plugin
//     - `verdigris.metatypes` -> `metatypes.json` -[registar]-> `cpp`
//     - build and link with `obj` from first product

// Important Note:
// ===============
// This requires some patching of the Qbs cpp module
// - add `explicitlyDependsOnFromDependencies: ['obj']` to all linker Rules
// - concat `Cpp.collectLinkerObjectPaths(explicitlyDependsOn)` with regular `Cpp.collectLinkerObjectPaths(inputs)`
//   (also add `explicitlyDependsOn` argument to the function in js files if necessary)

Project {
    Product {
        name: "qml_element_obj"
        condition: project.objlibrary_support

        files: [
            "MyObject.h",
            "MyObject.cpp",
        ]

        Depends { name: "Verdigris" }
        Depends { name: "Qt.quick" }
        Depends { name: "wqmlelement.objlibrary" }

        Export {
            Depends { name: "Verdigris" }
            Depends { name: "Qt.quick" }
        }
    }

    Application {
        name: "qml_element_tutorial"
        condition: project.objlibrary_support
        consoleApplication: true
        version: "1.0.0"
        Qt.qml.importName: "QmlTutorial"

        files: [
            "main.cpp",
        ]
        Group {
            name: "qrc:/QmlTutorial"
            files: [
                "main.qml",
            ]
            fileTags: ["qt.qml.qml", "qt.core.resource_data"]
            Qt.core.resourcePrefix: "/QmlTutorial"
        }

        Depends { name: "qml_element_obj" }
        Depends { name: "wqmlelement.metatypes" }
    }
}
