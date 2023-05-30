cmake_minimum_required(VERSION 3.25)

project(
    vcpkg-example
    LANGUAGES CXX
)

# find_package auto-includes the enumgen function: `enumgen_generate`
find_package(enumgen CONFIG REQUIRED)

add_executable(vcpkg-example)

set_target_properties(vcpkg-example PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

set(INPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/enumgen/enums.json")
set(CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/enumgen/config.json")

enumgen_generate(
    ${INPUT_FILE}
    ${CONFIG_FILE}
)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/src/*.cpp"
)

target_sources(vcpkg-example PRIVATE ${SOURCE_FILES})

target_compile_options(vcpkg-example PRIVATE
    "/std:c++20"                    # c++ standard
)

target_include_directories(vcpkg-example PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)