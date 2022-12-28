
add_executable(metatypes_extractor
  "main.cpp"
  "BufferSpan.h"
  "NtSection.h"
  "ElfSection.h"
)
target_compile_features(metatypes_extractor
  PRIVATE cxx_std_20
)
target_link_libraries(metatypes_extractor
  PRIVATE Qt6::Core
)
