#pragma once

#include <filesystem>
#include <optional>
#include <string>


namespace codegen
{

    /// <summary>
    /// Global enum generation config
    /// </summary>
    struct EnumConfig final
    {
        std::filesystem::path headerTemplateFile;
        std::filesystem::path codeTemplateFile;
    };

    /// <summary>
    /// Global codegen config
    /// </summary>
    struct Config final
    {
        std::filesystem::path configFile;
        std::filesystem::path templatesDirectory;
        EnumConfig enumConfig;
    };

    std::filesystem::path findConfigFile(std::filesystem::path const& inputFile) noexcept;

    /// <summary>
    /// Reads a config file
    /// </summary>
    std::optional<Config> tryReadConfig(std::filesystem::path const & configFile) noexcept;

    /// <summary>
    /// Validates the contents of a config file
    /// </summary>
    bool validateConfig(Config const& config) noexcept;

}