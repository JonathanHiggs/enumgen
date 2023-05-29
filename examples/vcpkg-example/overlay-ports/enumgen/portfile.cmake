# Note: bit of a hack to use the current repo checkout as the source url for vcpkg

set(VCPKG_USE_HEAD_VERSION ON)
get_filename_component(ENUMGEN_REPO_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../.." REALPATH)

vcpkg_from_git(
    OUT_SOURCE_PATH SOURCE_PATH
    URL "${ENUMGEN_REPO_DIR}"
    HEAD_REF main
)

set(VCPKG_BUILD_TYPE release)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DENUMGEN_PACKAGE_BUILD=ON
        -DENUMGEN_ENABLE_TESTS=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup()
vcpkg_copy_tools(TOOL_NAMES enumgen SEARCH_DIR ${CURRENT_PACKAGES_DIR}/tools/enumgen)
vcpkg_copy_tool_dependencies(${CURRENT_PACKAGES_DIR}/tools/enumgen)

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
file(COPY "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")