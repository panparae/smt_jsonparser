# Miminum required version of CMake.
cmake_minimum_required(VERSION 3.11)

# Project info
project(
    smt_jsonparser
    VERSION 0.1.0
    DESCRIPTION "smt_jsonparser"
    LANGUAGES CXX
)

# Cache variables
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set variable 'incPath' to ./include and 'srcPath' to ./src
set(incPath ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(srcPath ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Checking build type
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "Build type specified as '${CMAKE_BUILD_TYPE}'")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    set(CMAKE_BUILD_TYPE MinSizeRel)
    message(STATUS "Build type specified as '${CMAKE_BUILD_TYPE}'")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "Build type specified as '${CMAKE_BUILD_TYPE}'")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
    message(STATUS "Build type specified as '${CMAKE_BUILD_TYPE}'")
else ()
    # Build type is not set.
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "Build type was unspecified, set to Debug")
endif ()

# Making debug definition.
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set_directory_properties(PROPERTIES COMPILE_DEFINITIONS "_DEBUG")
else ()
    set_directory_properties(PROPERTIES COMPILE_DEFINITIONS "NDEBUG")
endif ()

# Make library file.
add_library(
    ${PROJECT_NAME}
    STATIC
    ${srcPath}/smt_jsonparser.cpp
)

# Set project linker language to c++
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Set library include directories
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${incPath}
)

# Compile options.
target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
)

if (MSVC)
    # Set start project for Visual Studio.
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif ()

# Making package
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION 1)
set_property(TARGET ${PROJECT_NAME} PROPERTY
  INTERFACE_${PROJECT_NAME}_MAJOR_VERSION 1)
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING ${PROJECT_NAME}_MAJOR_VERSION
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION "${version}"
  COMPATIBILITY AnyNewerVersion
)

install(
    TARGETS
    ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES
        ${incPath}/smt_jsonparser.h
        ${incPath}/smt_jsonparser_json.h
        ${incPath}/smt_jsonparser_malloc.h
        ${incPath}/smt_jsonparser_parse.h
        ${incPath}/smt_jsonparser_token.h
        ${incPath}/smt_jsonparser_value.h
        ${incPath}/smt_jsonparser_write.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
    EXPORT
    ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

export(
    EXPORT
    ${PROJECT_NAME}Targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
)