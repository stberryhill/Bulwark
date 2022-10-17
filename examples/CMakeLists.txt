add_executable(Colors
  ${CMAKE_CURRENT_SOURCE_DIR}/Colors.c
)

add_executable(Castle
  ${CMAKE_CURRENT_SOURCE_DIR}/Castle.c
)

target_link_libraries(Colors PRIVATE
  Bulwark::Bulwark
)

target_link_libraries(Castle PRIVATE
  Bulwark::Bulwark
)