#include <enumgen/generate/GenerateCommand.hpp>


using namespace enumgen::utils;


namespace enumgen::generate
{
    namespace
    {

        std::optional<std::string_view> setOption(GenerateCommand & command, Option const & option) noexcept
        {
            if (option.flag == "--input"sv || option.flag == "-i"sv)
            {
                command.specifications = option.value.value;
                return {};
            }
            if (option.flag == "--config"sv || option.flag == "-c"sv)
            {
                command.config = option.value.value;
                return {};
            }
            if (option.flag == "--binaryDir"sv || option.flag == "-b"sv)
            {
                command.binaryDirectory = option.value.value;
                return {};
            }
            if (option.flag == "--sourceDir"sv || option.flag == "-s"sv)
            {
                command.sourceDirectory = option.value.value;
                return {};
            }

            return "Unknown option"sv;
        }

        std::optional<std::string_view> setFlag(
            [[maybe_unused]] GenerateCommand & command, [[maybe_unused]] Flag const & flag) noexcept
        {
            return "Unknown option flag"sv;
        }

        std::optional<std::string_view> validate(GenerateCommand const & command) noexcept
        {
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
        auto command = GenerateCommand{};
        auto remainder = tokens;

        while (remainder.any())
        {
            auto optionResult = parseOption(remainder);
            if (optionResult)
            {
                auto setOptionError = setOption(command, *optionResult);
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