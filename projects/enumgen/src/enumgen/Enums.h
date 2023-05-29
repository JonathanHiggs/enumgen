#pragma once

#include <enumgen/Config.h>

#include <filesystem>
#include <string_view>


namespace enumgen
{

    /// <summary>
    /// Generates enum files from the given input file
    /// </summary>
    bool generateEnums(std::filesystem::path const & inputFile, std::filesystem::path const & outputRoot, Config const & config) noexcept;


    /// <summary>
    /// Generates enum files from the given input file
    /// </summary>
    bool generateEnums(std::string_view inputFile, std::string_view configFile, std::string_view outputPath) noexcept;

}  // namespace enumgen