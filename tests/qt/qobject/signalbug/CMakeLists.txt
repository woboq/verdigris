
add_executable(signalbug
  "signalbug.cpp"
  "signalbug.h"
)
target_link_libraries(signalbug PRIVATE
  verdigris
)
