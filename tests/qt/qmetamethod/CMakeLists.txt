
add_executable(qmetamethod
  "tst_qmetamethod6.cpp"
)
target_link_libraries(qmetamethod PRIVATE
  verdigris
  Qt6::Test
)
