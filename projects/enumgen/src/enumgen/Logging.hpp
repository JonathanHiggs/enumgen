#pragma once

#include <spdlog/spdlog.h>

#include <filesystem>


namespace enumgen
{

    [[nodiscard]] std::shared_ptr<spdlog::logger> & logger() noexcept;

    void initLogging(std::filesystem::path const & outputDirectory);

}  // namespace enumgen