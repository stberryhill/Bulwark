cmake_minimum_required(VERSION 3.0.0)
project(Bulwark VERSION 0.1.0)

include(CTest)
include(GNUInstallDirs)
enable_testing()

file(GLOB_RECURSE publicHeaders include/*.h)
file(GLOB_RECURSE sourceFiles src/*.c src/*.h)

include_directories(${PROJECT_NAME}
  PRIVATE
    ./include
    ./src
  )

add_library(${PROJECT_NAME} SHARED
  ${sourceFiles}
  )

target_include_directories(${PROJECT_NAME} PRIVATE ./include ./src)

# Copy public headers during install
# install(FILES ${publicHeaders} DESTINATION include)

# Make shared lib version follow project version.
set_target_properties(${PROJECT_NAME} PROPERTIES
VERSION ${PROJECT_VERSION}
PUBLIC_HEADER ${publicHeaders}
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
