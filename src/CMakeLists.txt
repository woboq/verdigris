
add_library(verdigris INTERFACE)
target_sources(verdigris INTERFACE
  FILE_SET HEADERS
  FILES
  "wobjectcpp.h"
  "wobjectdefs.h"
  "wobjectimpl.h"
  "wqmlelement.h"
)
target_compile_features(verdigris INTERFACE
  cxx_std_20
)
target_include_directories(verdigris INTERFACE
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(verdigris INTERFACE
  Qt6::Core
)
