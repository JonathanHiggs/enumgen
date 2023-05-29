# include(CMakePackageConfigHelpers)
#
# configure_package_config_file()

# write_basic_package_version_file(
#     ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
#     VERSION ${PROJECT_VERSION}
#     COMPATIBILITY SameMajorVersion
# )

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}-config.cmake
    DESTINATION share/${PROJECT_NAME}
)

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}-functions.cmake
    CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
    DESTINATION share/${PROJECT_NAME}
)