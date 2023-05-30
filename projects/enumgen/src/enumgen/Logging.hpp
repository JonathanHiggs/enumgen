#pragma once

#include <filesystem>


namespace enumgen
{

    void initLogging(std::filesystem::path const & outputDirectory);

}  // namespace enumgen