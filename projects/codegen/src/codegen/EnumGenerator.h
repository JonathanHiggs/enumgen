#pragma once

#include <codegen/Config.h>

#include <filesystem>


namespace codegen
{

    void generateEnums(std::filesystem::path const& inputFile, Config const& config) noexcept;

}