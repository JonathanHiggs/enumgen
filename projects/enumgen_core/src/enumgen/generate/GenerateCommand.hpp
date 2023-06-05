#pragma once

#include <enumgen/utils/ArgParser.hpp>
#include <enumgen/utils/Commands.hpp>

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

        static constexpr auto parameters = utils::Parameters<GenerateCommand>::Add(
            utils::Parameter<&GenerateCommand::specifications>("-i", "--input", "./enumgen/enums.json"),
            utils::Parameter<&GenerateCommand::config>("-c", "--config", "./enumgen/config.json"),
            utils::Parameter<&GenerateCommand::binaryDirectory>("-b", "--binaryDir"),
            utils::Parameter<&GenerateCommand::sourceDirectory>("-s", "--sourceDir"));
    };

    utils::Result<GenerateCommand> parseGenerateCommand(utils::Tokens const & tokens) noexcept;

}  // namespace enumgen::generate