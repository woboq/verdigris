
add_executable(internal
  "tst_internal.cpp"
)
target_link_libraries(internal PRIVATE
  verdigris
  Qt6::Test
)
