
add_executable(cppapi
  "tst_cppapi.cpp"
)
target_link_libraries(cppapi PRIVATE
  verdigris
  Qt6::Test
)
