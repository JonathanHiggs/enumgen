add_executable(codegen)

set_target_properties(codegen PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

target_sources(codegen PRIVATE
    ${SOURCE_FILES}
)

if(CODEGEN_COMPILER_MSVC)
    target_compile_options(codegen PRIVATE
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
    target_compile_options(codegen PRIVATE
        "-std=c++20"
        "-Wall"
        "-Wextra"
        "-pedantic"
        "-Werror"
    )
endif()


target_include_directories(codegen PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(codegen PUBLIC
    pantor::inja
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)