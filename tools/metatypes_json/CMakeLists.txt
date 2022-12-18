
add_executable(metatypesjson
  "BufferSpan.h"
  "NtSection.h"
  "metatypes_json.cpp"
)
target_compile_features(metatypesjson
  PRIVATE cxx_std_20
)
target_link_libraries(metatypesjson
  PRIVATE Qt6::Core
)
