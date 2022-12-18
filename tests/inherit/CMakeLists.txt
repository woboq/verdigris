
add_executable(inherit
  "api1.h"
  "api2.h"
  "object.cpp"
  "object.h"
  "tst_inherit.cpp"
)
target_link_libraries(inherit PRIVATE
  verdigris
  Qt6::Test
)
