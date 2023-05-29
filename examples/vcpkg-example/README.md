# Enumgen Example

This is a simple example of the `enumgen` tool in action

Enum [specifications](enumgen/enums.json), [config](enumgen/config.json) and [template files](enumgen/templates) are in
then enumgen directory

[CMakeLists.txt](CMakeLists.txt) includes the `enumgen` package and calls `enumgen_generate` passing in the 
specification and config files. `enumgen` then generates header and source files in the build directory that are added
to the target executable

A simple [main.cpp](src/main.cpp) has examples iterating through the generated enum values and parsing from strings

All of this happens with a single call to cmake configure

## Building

[vcpkg](https://vcpkg.io/en/index.html) is required to build this example, the toolchain file is specified by the 
`VCPKG_ROOT` environment variable and passed to cmake via the [CMake presets file](CMakePresets.json)

[overlay-ports](overlay-ports) contains a vcpkg port of the `enumgen` package that uses the current checkout of the
repository as a source. THIS IS A HACK and should not be copied
