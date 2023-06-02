add_subdirectory(enumgen)
add_subdirectory(enumgen_core)

if(ENUMGEN_BUILD_TESTS)
    add_subdirectory(enumgen_core_tests)
endif()