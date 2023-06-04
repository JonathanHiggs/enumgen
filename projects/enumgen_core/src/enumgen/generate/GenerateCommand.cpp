#include <enumgen/generate/GenerateCommand.hpp>
#include <enumgen/utils/Traits.hpp>

#include <tuple>


using namespace enumgen::utils;


namespace enumgen::generate
{
    namespace
    {

        template <std::size_t Index = 0ul, typename Func, typename ... Tp>
        constexpr void for_each(std::tuple<Tp...> const& tuple, Func&& func) noexcept
        {
            if constexpr (Index < sizeof...(Tp))
            {
                func(std::get<Index>(tuple));
                for_each<Index + 1>(tuple, std::forward<Func>(func));
            }
        }

        template <std::size_t Index = 0ul, typename Func, typename Condition, typename ... Tp>
        constexpr bool for_each(std::tuple<Tp...> const& tuple, Func&& func, Condition && condition) noexcept
        {
            if constexpr (Index < sizeof...(Tp))
            {
                auto result = func(std::get<Index>(tuple));
                if (condition(result))
                {
                    return true;
                }

                return for_each<Index + 1>(tuple, std::forward<Func>(func), std::forward<Condition>(condition));
            }
            else
            {
                return false;
            }
        }

        template <auto MemberValue>
            requires utils::member_value<decltype(MemberValue)>
        struct Argument final
        {
            using command_type = utils::member_value_class_t<MemberValue>;
            using member_value_type = utils::member_value_t<MemberValue>;

            std::string_view shortName;
            std::string_view longName;
            std::optional<member_value_type> defaultValue;
            // ToDo: bool required = std::is_optional<member_value_type> or std::function<bool(member_value_type)> validate
            // Maybe: track values set?

            constexpr Argument(std::string_view shortName, std::string_view longName) noexcept
              : shortName(shortName), longName(longName), defaultValue(std::nullopt)
            { }

            constexpr Argument(std::string_view shortName, std::string_view longName, member_value_type defaultValue) noexcept
              : shortName(shortName), longName(longName), defaultValue(defaultValue)
            { }

            [[nodiscard]] bool matchesName(std::string_view field) const noexcept
            {
                return field == shortName || field == longName;
            }

            void setDefault(command_type & command) const noexcept
            {
                if (defaultValue)
                {
                    command.*MemberValue = *defaultValue;
                }
            }

            [[nodiscard]] void setValue(
                command_type & command, std::string_view value) const noexcept
            {
                command.*MemberValue = value;
            }
        };


        template <typename Type, typename Command>
        concept ArgumentFor = std::same_as<typename Type::command_type, Command>;


        template <typename Command, ArgumentFor<Command> ... Args>
        struct Arguments final
        {
            using command_t = Command;

            std::tuple<Args...> arguments;

            constexpr Arguments(Args &&... args) noexcept : arguments(std::forward<Args>(args)...)
            { }

            void setDefaults(command_t & command) const noexcept
            {
                auto setDefault = [&](auto const& argument) {
                    argument.setDefault(command);
                };

                for_each(arguments, setDefault);
            }

            [[nodiscard]] std::optional<std::string_view> trySetOption(
                command_t & command, std::string_view name, std::string_view value) const noexcept
            {
                auto trySetValue = [&](auto const& argument) -> bool {
                    if (!argument.matchesName(name))
                    {
                        return false;
                    }

                    argument.setValue(command, value);
                    return true;
                };

                auto stopCondition = [](bool valueSet) -> bool {
                    return valueSet;
                };

                auto valueSet = for_each(arguments, trySetValue, stopCondition);
                if (valueSet)
                {
                    return std::nullopt;
                }

                return "Unknown argument"sv;
            }

            [[nodiscard]] std::optional<std::string_view> trySetOption(command_t& command, Option const& option) const noexcept
            {
                return this->trySetOption(command, option.flag.token, option.value.value);
            }
        };


        template <typename Command>
        struct Arguments<Command>
        {
            using command_t = Command;

            template <ArgumentFor<command_t> ... Args>
            static constexpr Arguments<command_t, Args...> Add(Args &&... args)
            {
                return Arguments<command_t, Args...>(std::forward<Args>(args)...);
            }
        };


        static constexpr auto arguments = Arguments<GenerateCommand>::Add(
            Argument<&GenerateCommand::specifications>("-i", "--input", "./enumgen/enums.json"),
            Argument<&GenerateCommand::config>("-c", "--config", "./enumgen/config.json"),
            Argument<&GenerateCommand::binaryDirectory>("-b", "--binaryDir"),
            Argument<&GenerateCommand::sourceDirectory>("-s", "--sourceDir"));


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
        arguments.setDefaults(command);

        auto remainder = tokens;

        while (remainder.any())
        {
            auto optionResult = parseOption(remainder);
            if (optionResult)
            {
                auto setOptionError = arguments.trySetOption(command, *optionResult);
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