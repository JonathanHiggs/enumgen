add_executable(enumgen)
add_executable(enumgen::enumgen ALIAS enumgen)

set_target_properties(enumgen PROPERTIES
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


target_include_directories(enumgen
    PUBLIC
    $<INSTALL_INTERFACE:include>
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
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
    RUNTIME DESTINATION tools/enumgen
)

install(
    FILES ${ENUMGEN_VERSION_HEADER}
    DESTINATION include/enumgen
)

install(
    EXPORT enumgen_target
    NAMESPACE enumgen::
    FILE enumgen-config.cmake
    DESTINATION share/enumgen
)

if (NOT ENUMGEN_PACKAGE_BUILD)
    # Need to manually copy over the dlls if we're not using vcpkg or conan

    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(ENUMGEN_FMT_LIBRARY "fmtd.dll")
        set(ENUMGEN_SPDLOG_LIBRARY "spdlogd.dll")
        set(ENUMGEN_LIBRARY_SEARCH_PATH ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin)
    else()
        set(ENUMGEN_FMT_LIBRARY "fmt.dll")
        set(ENUMGEN_SPDLOG_LIBRARY "spdlog.dll")
        set(ENUMGEN_LIBRARY_SEARCH_PATH ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin)
    endif()

    find_file(ENUMGEN_FMT_LIBRARY_PATH
        NAMES ${ENUMGEN_FMT_LIBRARY}
        PATHS ${ENUMGEN_LIBRARY_SEARCH_PATH}
        REQUIRED
    )

    find_file(ENUMGEN_SPDLOG_LIBRARY_PATH
        NAMES ${ENUMGEN_SPDLOG_LIBRARY}
        PATHS ${ENUMGEN_LIBRARY_SEARCH_PATH}
        REQUIRED
    )

    install(
        FILES ${ENUMGEN_FMT_LIBRARY_PATH} ${ENUMGEN_SPDLOG_LIBRARY_PATH}
        DESTINATION tools/enumgen
    )
endif()