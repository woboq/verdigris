
Application {
    name: "metatypes_extractor"
    // TODO: condition: qbs is native build
    type: base.concat(["metatypes-extractor"])
    version: "1.0.0"
    consoleApplication: true

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    cpp.cxxLanguageVersion: "c++20"
    cpp.includePaths: ['src']

    files: [
        "main.cpp",
        "BufferSpan.h",
        "NtSection.h",
        "ElfSection.h",
    ]

    Group {
        name: "Binary"
        fileTagsFilter: product.type
        fileTags: "metatypes-extractor"
    }
}
