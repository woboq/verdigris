find_package(Qt6 COMPONENTS Quick REQUIRED)

add_executable(cpp_api_tutorial
  "main.cpp"
  "resources.qrc"
)
target_link_libraries(cpp_api_tutorial
  PRIVATE verdigris
  PRIVATE Qt6::Quick
)
set_target_properties(cpp_api_tutorial PROPERTIES
  AUTORCC ON
)
