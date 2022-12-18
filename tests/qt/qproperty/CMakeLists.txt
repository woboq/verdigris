
add_executable(qproperty
  "tst_qproperty6.cpp"
)
target_link_libraries(qproperty PRIVATE
  verdigris
  Qt6::CorePrivate
  Qt6::Test
)
