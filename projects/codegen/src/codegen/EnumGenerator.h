#pragma once

#include <codegen/Config.h>

#include <filesystem>


namespace codegen
{

    /// <summary>
    /// Generates enum definitions from the supplied input file
    /// </summary>
    void generateEnum(std::filesystem::path const & inputFile, Config const & config) noexcept;

}