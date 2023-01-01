
add_subdirectory("signalbug")
find_package(Qt6 COMPONENTS Network REQUIRED)

add_executable(qobject
  "tst_qobject6.cpp"
)
target_link_libraries(qobject PRIVATE
  verdigris
  Qt6::Test
  Qt6::TestPrivate
  Qt6::Network
)
target_compile_definitions(qobject PRIVATE
  QT_DISABLE_DEPRECATED_BEFORE=0
  QT_NO_DEBUG
  W_SIGNALBUG="$<TARGET_FILE:signalbug>"
)
