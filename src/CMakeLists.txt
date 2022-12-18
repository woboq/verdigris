
add_library(verdigris INTERFACE)
target_sources(verdigris INTERFACE
  "wobjectcpp.h"
  "wobjectdefs.h"
  "wobjectimpl.h"
)
target_compile_features(verdigris INTERFACE
  cxx_std_20
)
target_include_directories(verdigris INTERFACE
  "."
)
target_link_libraries(verdigris INTERFACE
  Qt6::Core
)
