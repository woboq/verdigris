
Application {
    name: "metatypesjson"
    version: "1.0.0"
    consoleApplication: true

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    cpp.cxxLanguageVersion: "c++20"
    cpp.includePaths: ['src']

    files: [
        "BufferSpan.h",
        "NtSection.h",
        "metatypes_json.cpp",
    ]
}
