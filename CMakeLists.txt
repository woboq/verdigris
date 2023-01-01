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

add_library(verdigris::verdigris ALIAS verdigris)
include(CMakePackageConfigHelpers)
configure_package_config_file("cmake/verdigrisConfig.cmake.in"
  "verdigrisConfig.cmake"
  INSTALL_DESTINATION verdigris/cmake
)
write_basic_package_version_file("verdigrisConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
  ARCH_INDEPENDENT
)
include(GNUInstallDirs)
install(TARGETS verdigris
  EXPORT verdigris_Targets
  FILE_SET HEADERS
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/verdigris"
)
install(FILES
  "${PROJECT_BINARY_DIR}/verdigrisConfig.cmake"
  "${PROJECT_BINARY_DIR}/verdigrisConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/verdigris/cmake)
install(EXPORT verdigris_Targets
  FILE "verdigrisTargets.cmake"
  NAMESPACE verdigris::
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/verdigris/cmake
)

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.LGPLv3")
include(CPack)
