cmake_minimum_required(VERSION 3.25)


## CMake Options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(ENABLE_DEVELOPER_MODE TRUE CACHE BOOL "Enable 'developer mode'")
set(OPT_WARNINGS_AS_ERRROS_DEVELOPER_DEFAULT TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)


## Project
file(READ "version.txt" ENUMGEN_VERSION)

project(
    enumgen
    VERSION ${ENUMGEN_VERSION}
    DESCRIPTION "Code generator for C++"
    HOMEPAGE_URL "https://github.com/JonathanHiggs/enumgen"
    LANGUAGES CXX
)


## Options
option(ENUMGEN_ENABLE_TESTS "Enable tests" OFF)


## Build Options
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

set(ENUMGEN_INSTALL_TOOLS_DIR tools CACHE PATH "enumgen install directory")


## CMake modules & package options
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)


include(platform)
detect_platform()


# enable_testing()


## Dependencies
find_package(nlohmann_json  CONFIG REQUIRED)
find_package(inja           CONFIG REQUIRED)
find_package(spdlog         CONFIG REQUIRED)



## Projects
add_subdirectory(projects)