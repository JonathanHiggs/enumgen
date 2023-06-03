#pragma once

#include <enumgen/utils/ArgParser.hpp>

#include <optional>
#include <string_view>


using namespace std::string_view_literals;


namespace enumgen::generate
{

    struct GenerateCommand final
    {
        std::string_view specifications = ""sv;
        std::string_view config = ""sv;
        std::string_view binaryDirectory = ""sv;
        std::string_view sourceDirectory = ""sv;
    };

    utils::Result<GenerateCommand> parseGenerateCommand(utils::Tokens const & tokens) noexcept;

}  // namespace enumgen::generate