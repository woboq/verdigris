
find_package(Qt6 COMPONENTS Quick REQUIRED)

add_executable(tutorial
  "tutorial.cpp"
)
target_link_libraries(tutorial PRIVATE
  verdigris
)

add_executable(cpp_tutorial
  "cpp_tutorial.cpp"
  "cpp_tutorial.qrc"
)
target_link_libraries(cpp_tutorial PRIVATE
  verdigris
  Qt6::Quick
)
set_target_properties(cpp_tutorial PROPERTIES AUTORCC ON)
