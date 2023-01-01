
add_executable(manyproperties
  "tst_manyproperties.cpp"
)
target_link_libraries(manyproperties PRIVATE
  verdigris
  Qt6::Test
)
