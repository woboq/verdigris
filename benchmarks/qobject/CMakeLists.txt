
add_executable(qobject_bench
  "main.cpp"
  "object.cpp"
  "object.h"
)
target_link_libraries(qobject_bench PRIVATE
  verdigris
  Qt6::Test
)
set_target_properties(qobject_bench PROPERTIES AUTOMOC ON)
