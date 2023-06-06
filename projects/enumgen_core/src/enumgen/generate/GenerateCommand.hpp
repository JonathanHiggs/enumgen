#pragma once

#include <enumgen/utils/Commands.hpp>
#include <enumgen/utils/Parser.hpp>

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

        static constexpr auto parameters = parametersFor<GenerateCommand>(
            Parameter<&GenerateCommand::specifications>("-i", "--input", "./enumgen/enums.json"),
            Parameter<&GenerateCommand::config>("-c", "--config", "./enumgen/config.json"),
            Parameter<&GenerateCommand::binaryDirectory>("-b", "--binaryDir"),
            Parameter<&GenerateCommand::sourceDirectory>("-s", "--sourceDir"));
    };

    Result<GenerateCommand> parseGenerateCommand(Tokens const & tokens) noexcept;

}  // namespace enumgen::generate