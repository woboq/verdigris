
add_executable(basic
  "anothertu.cpp"
  "tst_basic.cpp"
  "anothertu.h"
)
target_link_libraries(basic PRIVATE
  verdigris
  Qt6::Test
)
