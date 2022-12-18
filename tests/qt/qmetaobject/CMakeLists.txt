
add_executable(qmetaobject
  "tst_qmetaobject6.cpp"
)
target_link_libraries(qmetaobject PRIVATE
  verdigris
  Qt6::Test
)
