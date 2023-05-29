add_executable(enumgen)
add_executable(enumgen::enumgen ALIAS enumgen)

set_target_properties(enumgen PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

target_sources(enumgen PRIVATE
    ${SOURCE_FILES}
)

if(ENUMGEN_COMPILER_MSVC)
    target_compile_options(enumgen PRIVATE
        "/std:c++20"                    # c++ standard
        "/bigobj"                       # increases the number of sections in .obj files
        "/FC"                           # display full path in diagnostics
        "/WX"                           # warnings as errors
        "/W4"                           # warning level [0,4]
        "/wd4099"                       # exclude: type first seen using #
        "/wd4100"                       # exclude: unreferenced parameter
        "/wd4201"                       # exclude: nameless struct/union
    #   "/INCREMENTAL:NO"               # disable incremental compilation
        "$<$<CONFIG:DEBUG>:/Oi>"        # replace calls with intrinsics
        "$<$<CONFIG:DEBUG>:/Zi>"        # generate complete debug info
        "$<$<CONFIG:DEBUG>:/JMC>"       # just my code
        "$<$<CONFIG:RELEASE>:/Ot>"      # prefer fast optimizations
    )
else()
    target_compile_options(enumgen PRIVATE
        "-std=c++20"
        "-Wall"
        "-Wextra"
        "-pedantic"
        "-Werror"
    )
endif()


target_include_directories(enumgen PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(enumgen PRIVATE
    fmt::fmt
    pantor::inja
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)


install(
    TARGETS enumgen
    EXPORT enumgen_target
    CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
    RUNTIME DESTINATION ${ENUMGEN_INSTALL_TOOLS_DIR}/${PROJECT_NAME}
)

install(
    EXPORT enumgen_target
    NAMESPACE enumgen::
    FILE enumgen-config.cmake
    DESTINATION share/enumgen
)