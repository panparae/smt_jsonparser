# Miminum required version of CMake.
cmake_minimum_required(VERSION 3.11)

# Project info
project(
    smt_jsonparser_demo
    VERSION 1.0.0
    DESCRIPTION "smt_jsonparser_demo"
    LANGUAGES CXX
)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set variable 'srcPath' to ./src and 'incPath' to ./include 'testPath' to ./test
set(srcPath ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(incPath ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(testPath ${CMAKE_CURRENT_SOURCE_DIR}/test)

# Unit test cache variable.
set(UNIT_TEST false CACHE BOOL "for unit test.")

# Find Catch2.
if (${UNIT_TEST})
find_package(Catch2 CONFIG REQUIRED)
endif()

find_package(smt_jsonparser CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

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

# Make executable file.
message(STATUS "Unit Testing set to '${UNIT_TEST}'")
if (${UNIT_TEST})
    add_executable(
        ${PROJECT_NAME}
        ${testPath}/test_main.cpp
    )
else ()
    add_executable(
        ${PROJECT_NAME}
        ${srcPath}/main.cpp
        ${srcPath}/jsoncpp_module.cpp
        ${srcPath}/nlohmann_module.cpp
        ${srcPath}/smt_jsonparser_module.cpp
    )
endif ()

# Set output path by build type.
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/Debug
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/Release
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo
)

# Set library include directories
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${incPath}
    ${srcPath}
)

# Link tcmalloc, spdlog and Catch2.
if (${UNIT_TEST})
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        smt_jsonparser::smt_jsonparser
        Catch2::Catch2
    )
else ()
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        smt_jsonparser::smt_jsonparser
        JsonCpp::JsonCpp
        nlohmann_json::nlohmann_json
    )
endif()

# Compile options.
target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
)

if (MSVC)
    # Set start project for Visual Studio.
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
    
    # Set reserved stack size for Visual Studio.
    target_link_options(
        ${PROJECT_NAME}
        PRIVATE
        /STACK:10000000
    )
endif ()