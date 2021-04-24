cmake_minimum_required(VERSION 3.12.0)
project(Bulwark VERSION 0.1.0 LANGUAGES C)

# Set default visibility to hidden (only functions bearing BULWARK_API will be exported)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

option(BUILD_SHARED_LIBS "Build shared library" ON)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# Set intall RPATH info for library.
if (NOT APPLE)
  set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

add_library(Bulwark
  src/Buffer.c
  src/BufferChangeList.c
  src/Bulwark.c
  src/Color.c
  src/Event.c
  src/EventQueue.c
  src/Input.c
  src/Internal.h
  src/Log.c
  src/Sound.c
  src/Window.c
  )

# Allows to use Bulwark as a subdirectory (and in examples)
add_library(Bulwark::Bulwark ALIAS Bulwark)

generate_export_header(Bulwark
  EXPORT_MACRO_NAME BULWARK_API
  EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/BulwarkAPIMacro.h
  )

target_include_directories(Bulwark
  PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
  )

# Make shared lib version follow project version.
set_target_properties(Bulwark PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/BulwarkConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/BulwarkConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Bulwark
  PATH_VARS
    CMAKE_INSTALL_LIBDIR
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/BulwarkConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(TARGETS Bulwark
  EXPORT BulwarkTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT Bulwark_Development
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT Bulwark_Runtime
    NAMELINK_COMPONENT Bulwark_Development
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT Bulwark_Runtime
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# install(FILES ${publicHeaders} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
  
install(EXPORT BulwarkTargets
  FILE BulwarkTargets.cmake
  COMPONENT Bulwark_Development
  NAMESPACE Bulwark::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Bulwark
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/BulwarkConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/BulwarkConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Bulwark
)

add_subdirectory(examples)
