import qbs

Project {
    name: "Verdigris"

    // use `qbs build project.objlibrary_support:true` if qbs is patched (needed for W_QML_ELEMENT support)
    property bool objlibrary_support: false

    references: [
        "tutorial",
        "tests",
        "benchmarks",
        "tools",
    ]
    qbsSearchPaths: "qbs/"

    AutotestRunner {}

    Product {
        name: "Verdigris"

        Depends { name: "cpp" }
        Depends { name: "Qt.core" }
        Qt.core.enableMoc: false
        cpp.cxxLanguageVersion: "c++20"
        cpp.includePaths: ['src']

        files: [
            "src/wobjectcpp.h",
            "src/wobjectdefs.h",
            "src/wobjectimpl.h",
            "src/wqmlelement.h",
        ]

        Export {
            Depends { name: "cpp" }
            Depends { name: "Qt.core" }
            cpp.cxxLanguageVersion: "c++20"
            cpp.includePaths: ['src']

            Properties {
                condition: qbs.toolchain.contains('msvc')
                cpp.cxxFlags: base.concat(
                    "/permissive-", "/Zc:__cplusplus", "/Zc:externConstexpr", "/Zc:inline", "/Zc:preprocessor", "/Zc:throwingNew", // best C++ compatibility
                    "/diagnostics:caret" // better errors
                )
            }
            Properties {
                condition: qbs.toolchain.contains('clang')
                cpp.cxxFlags: base.concat(
                    "--pedantic", // best C++ compatibility
                    "-Wall", "-Wextra", // enable more warnings
                    "-ftemplate-backtrace-limit=0", // do not cut template backtraces
                    "-Wno-gnu-zero-variadic-macro-arguments" // accept this extension
                )
                // note: use qbs build parameters if you need this!
                // cpp.cxxStandardLibrary: "libc++"
                // cpp.staticLibraries: ["c++", "c++abi"]
            }
            Properties {
                condition: qbs.toolchain.contains('gcc')
                cpp.cxxFlags: base.concat(
                    "--pedantic", // best C++ compatibility
                    "-Wall", "-Wextra", "-Wno-noexcept-type", "-Wno-address", // enable more warnings
                    "-ftemplate-backtrace-limit=0" // do not cut template backtraces
                )
            }
        }
    }

    Product {
        name: "[Extra Files]"
        files: [
            ".github/workflows/clang.yml",
            ".github/workflows/gcc.yml",
            ".github/workflows/windows.yml",
            "ChangeLog",
            "LICENSE.LGPLv3",
            "README.md",
            "qbs/modules/wqmlelement/metatypes/metatypes.qbs",
            "qbs/modules/wqmlelement/objlibrary/objlibrary.qbs",
        ]
    }
}
