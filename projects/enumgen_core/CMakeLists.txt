add_library(enumgen_core STATIC)
add_library(enumgen::core ALIAS enumgen_core)

set_target_properties(enumgen_core PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

set(ENUMGEN_VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/enumgen/Version.hpp")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/templates/Version.hpp.in"
    ${ENUMGEN_VERSION_HEADER}
)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

target_sources(enumgen_core PRIVATE
    ${SOURCE_FILES}
)

if(ENUMGEN_COMPILER_MSVC)
    target_compile_options(enumgen_core PRIVATE
        "/std:c++20"                    # c++ standard
        "/FC"                           # display full path in diagnostics
        "/WX"                           # warnings as errors
        "/W4"                           # warning level [0,4]
    #   "/INCREMENTAL:NO"               # disable incremental compilation
        "$<$<CONFIG:DEBUG>:/Oi>"        # replace calls with intrinsics
        "$<$<CONFIG:DEBUG>:/Zi>"        # generate complete debug info
        "$<$<CONFIG:DEBUG>:/JMC>"       # just my code
        "$<$<CONFIG:RELEASE>:/Ot>"      # prefer fast optimizations
    )
else()
    target_compile_options(enumgen_core PRIVATE
        "-std=c++20"
        "-Wall"
        "-Wextra"
        "-pedantic"
        "-Werror"
    )
endif()


target_include_directories(enumgen_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_link_libraries(enumgen_core
    PUBLIC
        fmt::fmt-header-only
        pantor::inja
        nlohmann_json::nlohmann_json
        spdlog::spdlog_header_only
)