#include <enumgen/generate/GenerateCommand.hpp>
#include <enumgen/utils/Traits.hpp>

#include <tuple>


using namespace enumgen::utils;


namespace enumgen::generate
{
    namespace
    {

        std::optional<std::string_view> setFlag(
            [[maybe_unused]] GenerateCommand & command, [[maybe_unused]] Flag const & flag) noexcept
        {
            // ToDo: extend command handling with flags

            return "Unknown option flag"sv;
        }

        std::optional<std::string_view> validate(GenerateCommand const & command) noexcept
        {
            // ToDo: extend command handling with validation

            if (command.specifications.empty())
            {
                return "Missing '--input' option"sv;
            }
            if (command.config.empty())
            {
                return "Missing '--config' option"sv;
            }
            if (command.binaryDirectory.empty())
            {
                return "Missing '--binaryDir' option"sv;
            }
            if (command.sourceDirectory.empty())
            {
                return "Missing '--sourceDir' option"sv;
            }

            return {};
        }

    }  // namespace


    Result<GenerateCommand> parseGenerateCommand(Tokens const & tokens) noexcept
    {
        // ToDo: extend commands with templated parsing
        using command_t = GenerateCommand;
        constexpr auto parameters = command_t::parameters;

        auto command = command_t{};
        parameters.setDefaults(command);

        auto remainder = tokens;

        while (remainder.any())
        {
            auto optionResult = parseOption(remainder);
            if (optionResult)
            {
                auto setOptionError = parameters.trySetOption(command, *optionResult);
                if (setOptionError)
                {
                    return ParseError(remainder, *setOptionError);
                }

                remainder = optionResult.remainder();
                continue;
            }

            auto flagResult = parseFlag(remainder);
            if (flagResult)
            {
                auto setFlagError = setFlag(command, *flagResult);
                if (setFlagError)
                {
                    return ParseError(remainder, *setFlagError);
                }

                remainder = flagResult.remainder();
                continue;
            }

            return ParseError(remainder, "Unknown option or flag");
        }

        if (auto validateError = validate(command); validateError)
        {
            return ParseError(tokens, *validateError);
        }

        return Output<GenerateCommand>(std::move(command), remainder);
    }

}  // namespace enumgen::generate