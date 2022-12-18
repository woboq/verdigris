
add_executable(qmetaproperty
  "tst_qmetaproperty6.cpp"
)
target_link_libraries(qmetaproperty PRIVATE
  verdigris
  Qt6::Test
)
