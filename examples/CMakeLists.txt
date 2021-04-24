add_executable(Colors
  ${CMAKE_CURRENT_SOURCE_DIR}/Colors.c
)

target_link_libraries(Colors PRIVATE
  Bulwark::Bulwark
)