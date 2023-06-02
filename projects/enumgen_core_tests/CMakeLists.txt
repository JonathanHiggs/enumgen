include(GoogleTest)

add_executable(enumgen_core_tests)

file(GLOB_RECURSE TEST_FILES CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

target_sources(enumgen_core_tests PRIVATE ${TEST_FILES})

if(ENUMGEN_COMPILER_MSVC)
    target_compile_options(enumgen_core_tests PRIVATE
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
    target_compile_options(enumgen_core_tests PRIVATE
        "-std=c++20"
        "-Wall"
        "-Wextra"
        "-pedantic"
        "-Werror"
    )
endif()

target_include_directories(enumgen_core_tests
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(enumgen_core_tests
    PRIVATE
        enumgen::core
        GTest::gtest
        GTest::gtest_main
)

add_test(NAME enumgen_core_tests COMMAND enumgen_core_tests)

gtest_discover_tests(enumgen_core_tests)