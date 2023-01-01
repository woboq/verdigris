
add_executable(qmetaenum
  "tst_qmetaenum.cpp"
)
target_link_libraries(qmetaenum PRIVATE
  verdigris
  Qt6::Test
)
