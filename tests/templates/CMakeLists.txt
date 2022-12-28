
add_executable(templates
  "tst_templates.cpp"
)
target_link_libraries(templates PRIVATE
  verdigris
  Qt6::Test
)
