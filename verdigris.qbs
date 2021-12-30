import qbs

Project {
    name: "Verdigris"

    references: [
        "tutorial",
        "tests",
        "benchmarks",
    ]

    AutotestRunner {}

    Product {
        name: "Verdigris"

        files: [
            "src/wobjectcpp.h",
            "src/wobjectdefs.h",
            "src/wobjectimpl.h",
        ]

        Export {
            Depends { name: "cpp" }
            Depends { name: "Qt.core" }
            cpp.cxxLanguageVersion: "c++14"
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
                    "-Wall", "-Wextra", "-Wno-noexcept-type", // enable more warnings
                    "-ftemplate-backtrace-limit=0" // do not cut template backtraces
                )
            }
        }
    }

    Product {
        name: "[Extra Files]"
        files: [
            ".appveyor.yml",
            ".github/workflows/clang.yml",
            ".github/workflows/gcc.yml",
            ".github/workflows/windows.yml",
            ".travis.yml",
            "ChangeLog",
            "LICENSE.LGPLv3",
            "README.md",
        ]
    }
}
