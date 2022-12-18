cmake_minimum_required(VERSION 3.25)

project(Verdigris
  VERSION 2.0
  DESCRIPTION "Replace Qt moc with native C++"
  HOMEPAGE_URL "https://github.com/woboq/verdigris"
  LANGUAGES CXX
)

enable_testing()

find_package(Qt6 6.2 COMPONENTS Core Test REQUIRED)

add_subdirectory("src")
add_subdirectory("tools")
add_subdirectory("tutorial")
add_subdirectory("tests")
add_subdirectory("benchmarks")

set_source_files_properties(
  "src/wobjectcpp.h"
  "src/wobjectdefs.h"
  "src/wobjectimpl.h"
  "src/wqmlelement.h"
  DIRECTORY
  "benchmarks/qobject/"
  PROPERTIES
  SKIP_AUTOMOC ON
)
