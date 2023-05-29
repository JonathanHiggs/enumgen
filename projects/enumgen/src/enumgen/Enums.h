#pragma once

#include <enumgen/Config.h>

#include <filesystem>


namespace enumgen
{

    /// <summary>
    /// Generates enum files from the given input file
    /// </summary>
    void generateEnums(std::filesystem::path const & inputFile, Config const & config) noexcept;

}  // namespace enumgen