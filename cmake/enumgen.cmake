# EnumGen CMake module


# Find the enumgen package
find_package(enumgen REQUIRED)


# Generate enum code files for given input and config files
function(enumgen_generate input_file config_file)
    get_target_property(ENUMGEN_EXE enumgen::enumgen LOCATION)

    set(ENUMGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

    execute_process(
        COMMAND ${ENUMGEN_EXE} ${input_file} ${config_file} ${ENUMGEN_OUTPUT_DIR}
    )
endfunction()