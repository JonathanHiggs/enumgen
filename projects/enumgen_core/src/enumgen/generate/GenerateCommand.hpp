#pragma once

#include <enumgen/utils/ArgParser.hpp>
#include <enumgen/utils/Commands.hpp>

#include <optional>
#include <string_view>


using namespace std::string_view_literals;


namespace enumgen::generate
{

    /// <summary>
    /// Command parameters to execute the generation process
    /// </summary>
    struct GenerateCommand final
    {
        std::string_view specifications;
        std::string_view config;
        std::string_view binaryDirectory;
        std::string_view sourceDirectory;

        static constexpr auto parameters = utils::parametersFor<GenerateCommand>(
            utils::Parameter<&GenerateCommand::specifications>("-i", "--input", "./enumgen/enums.json"),
            utils::Parameter<&GenerateCommand::config>("-c", "--config", "./enumgen/config.json"),
            utils::Parameter<&GenerateCommand::binaryDirectory>("-b", "--binaryDir"),
            utils::Parameter<&GenerateCommand::sourceDirectory>("-s", "--sourceDir"));
    };

    utils::Result<GenerateCommand> parseGenerateCommand(utils::Tokens const & tokens) noexcept;

}  // namespace enumgen::generate