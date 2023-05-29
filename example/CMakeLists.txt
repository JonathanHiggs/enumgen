cmake_minimum_required(VERSION 3.25)

project(
    enumgen_example
    LANGUAGES CXX
)

## Add enumgen locally - expects enumgen to be in the same directory as this project- usually a package manager would
## take care of this
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../out/install/${ENUMGEN_CURRENT_PRESET}/share/enumgen")
set(enumgen_DIR "${CMAKE_SOURCE_DIR}/../out/install/${ENUMGEN_CURRENT_PRESET}/share/enumgen")

find_package(enumgen REQUIRED)

include(enumgen)

add_executable(example)

set_target_properties(example PROPERTIES
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

target_sources(example PRIVATE ${SOURCE_FILES})

target_compile_options(example PRIVATE
    "/std:c++20"                    # c++ standard
)

target_include_directories(example PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)