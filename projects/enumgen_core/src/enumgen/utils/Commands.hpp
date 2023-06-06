#pragma once

#include <enumgen/utils/Parser.hpp>
#include <enumgen/utils/Traits.hpp>

#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include <tuple>


using namespace std::string_view_literals;


namespace enumgen
{
    namespace detail
    {

        /// <summary>
        /// Loops over elements of a tuple and applies the given function to each element
        /// </summary>
        template <std::size_t Index = 0ul, typename Func, typename... Tp>
        constexpr void forEach(std::tuple<Tp...> const & tuple, Func && func) noexcept
        {
            if constexpr (Index < sizeof...(Tp))
            {
                func(std::get<Index>(tuple));  // ToDo: handle throwing functions
                forEach<Index + 1>(tuple, std::forward<Func>(func));
            }
        }

        /// <summary>
        /// Loops over elements of a tuple until the condition evaluates to true and applies the given function to each
        /// element
        /// </summary>
        template <std::size_t Index = 0ul, typename Func, typename Condition, typename... Tp>
        constexpr auto forEachUntil(std::tuple<Tp...> const & tuple, Func && func, Condition && condition) noexcept
        {
            using result_t = std::invoke_result_t<Func, std::tuple_element_t<Index, std::tuple<Tp...>>>;

            result_t result = func(std::get<Index>(tuple));  // ToDo: handle throwing functions
            if (condition(result))
            {
                return result;
            }

            if constexpr (Index + 1ul < sizeof...(Tp))
            {
                return forEachUntil<Index + 1ul>(tuple, std::forward<Func>(func), std::forward<Condition>(condition));
            }
            else
            {
                return result_t{};
            }
        }

    }  // namespace detail


    /// <summary>
    /// Helper class to convert a string_view into a value of the given type
    /// </summary>
    template <typename ValueType>
    struct ConvertValue;

    template <>
    struct ConvertValue<std::string_view>
    {
        using value_type = std::string_view;

        [[nodiscard]] static std::optional<value_type> tryConvert(std::string_view value) noexcept
        {
            return value;
        }
    };

    template <>
    struct ConvertValue<bool>
    {
        using value_type = bool;

        [[nodiscard]] static std::optional<value_type> tryConvert(std::string_view value) noexcept
        {
            constexpr auto trueValues
                = std::array<std::string_view, 7ul>{ "TRUE"sv, "True"sv, "true"sv, "ON"sv, "On"sv, "on"sv, "1"sv };

            constexpr auto falseValues
                = std::array<std::string_view, 7ul>{ "FALSE"sv, "False"sv, "false", "OFF"sv, "Off"sv, "off"sv, "0"sv };

            auto matchesValue = [&](auto v) -> bool {
                return value == v;
            };

            if (std::ranges::any_of(trueValues, matchesValue))
            {
                return true;
            }

            if (std::ranges::any_of(falseValues, matchesValue))
            {
                return false;
            }

            return std::nullopt;
        }
    };


    /// <summary>
    /// Contains details needed to parse a command line argument matching a parameter into a command instance
    /// </summary>
    template <auto MemberValue>
        requires member_value<MemberValue>
    struct Parameter final
    {
        // ToDo: bool required = std::is_optional<member_value_type>
        //       track values set
        // ToDo: validation

        using command_type = member_value_class_t<MemberValue>;
        using member_value_type = member_value_t<MemberValue>;

        std::string_view shortName;
        std::string_view longName;
        std::optional<member_value_type> defaultValue;

        constexpr Parameter(std::string_view shortName, std::string_view longName) noexcept
          : shortName(shortName), longName(longName), defaultValue(std::nullopt)
        { }

        constexpr Parameter(
            std::string_view shortName, std::string_view longName, member_value_type defaultValue) noexcept
          : shortName(shortName), longName(longName), defaultValue(defaultValue)
        { }

        /// <summary>
        /// Returns true if the parameter is a flag; false otherwise
        /// </summary>
        [[nodiscard]] constexpr bool isFlag() const noexcept
        {
            return std::same_as<member_value_type, bool>;
        }

        /// <summary>
        /// Returns true if the parameter is an option; false otherwise
        /// </summary>
        [[nodiscard]] constexpr bool isOption() const noexcept
        {
            return not std::same_as<member_value_type, bool>;
        }

        /// <summary>
        /// Checks if the given field matches the short or long name of the parameter
        /// </summary>
        [[nodiscard]] bool matchesName(std::string_view field) const noexcept
        {
            return field == shortName || field == longName;
        }

        /// <summary>
        /// Sets the default value for the parameter where applicable
        /// </summary>
        void setDefault(command_type & command) const noexcept
        {
            if (defaultValue)
            {
                command.*MemberValue = *defaultValue;
            }
        }

        /// <summary>
        /// Sets the value of the parameter to true
        /// </summary>
        [[nodiscard]] bool setFlag(command_type & command) const noexcept
        {
            if constexpr (std::same_as<member_value_type, bool>)
            {
                command.*MemberValue = true;
                return true;
            }
            else
            {
                // Should never happen
                return false;
            }
        }

        /// <summary>
        /// Sets the value of the parameter to the given value
        /// </summary>
        [[nodiscard]] bool setValue(command_type & command, std::string_view value) const noexcept
        {
            auto converted = ConvertValue<member_value_type>::tryConvert(value);
            if (!converted)
            {
                return false;
            }

            command.*MemberValue = std::move(converted).value();
            return true;
        }
    };


    // Maybe: parameter builder class to give a nicer interface?


    /// <summary>
    /// Matches when the given type is an parameter for the command
    /// </summary>
    template <typename Type, typename Command>
    concept parameter_for = std::same_as<typename Type::command_type, Command>;


    /// <summary>
    /// Collection of parameters used to parse command line arguments into a command instance
    /// </summary>
    template <typename Command, parameter_for<Command>... Parameters>
    struct ParameterSet final
    {
        using command_type = Command;

        // clang-format off
        struct NoMatch final : std::monostate {};
        struct Success final : std::monostate {};
        struct Error final : std::string_view {};
        using TrySetResult = std::variant<NoMatch, Success, Error>;
        // clang-format on

        std::tuple<Parameters...> parameters;

        constexpr ParameterSet(Parameters &&... parameters) noexcept
          : parameters(std::forward<Parameters>(parameters)...)
        { }

        /// <summary>
        /// Sets default values for all parameters
        /// </summary>
        void setDefaults(command_type & command) const noexcept
        {
            auto setDefault = [&](auto const & argument) noexcept {
                argument.setDefault(command);
            };

            detail::forEach(parameters, setDefault);
        }

        /// <summary>
        /// Attempts to set the value of a bool parameter matching the given flag
        /// </summary>
        [[nodiscard]] std::optional<std::string_view> trySetFlag(
            command_type & command, Flag const & flag) const noexcept
        {
            auto trySetFlag = [&](auto const & argument) noexcept -> TrySetResult {
                if (!argument.matchesName(flag.token))
                {
                    return NoMatch{};
                }

                if (argument.isOption())
                {
                    return Error{ "Argument is not a flag"sv };
                }

                if (!argument.setFlag(command))
                {
                    // Should never happen
                    return Error{ "Argument is not a flag"sv };
                }

                return Success{};
            };

            auto whileNoMatch = [](TrySetResult const & result) noexcept -> bool {
                // Stop on success or error
                return !std::holds_alternative<NoMatch>(result);
            };

            auto trySetResult = detail::forEachUntil(parameters, trySetFlag, whileNoMatch);
            if (std::holds_alternative<NoMatch>(trySetResult))
            {
                return "Unknown flag"sv;
            }
            if (auto * error = std::get_if<Error>(&trySetResult))
            {
                return *error;
            }

            return std::nullopt;  // Success
        }

        /// <summary>
        /// Attempts to set the value of the parameter matching the given option
        /// </summary>
        [[nodiscard]] std::optional<std::string_view> trySetOption(
            command_type & command, Option const & option) const noexcept
        {
            auto trySetValue = [&](auto const & argument) noexcept -> TrySetResult {
                if (!argument.matchesName(option.flag.token))
                {
                    return NoMatch{};
                }

                // Don't need to check if the argument is a flag because that's handled by converting the value to bool

                if (!argument.setValue(command, option.value.value))
                {
                    return Error{ "Unable to convert value"sv };
                }

                return Success{};
            };

            auto whileNoMatch = [](TrySetResult const & result) noexcept -> bool {
                // Stop on success or error
                return !std::holds_alternative<NoMatch>(result);
            };

            auto trySetResult = detail::forEachUntil(parameters, trySetValue, whileNoMatch);
            if (std::holds_alternative<NoMatch>(trySetResult))
            {
                return "Unknown argument"sv;
            }
            if (auto * error = std::get_if<Error>(&trySetResult))
            {
                return *error;
            }

            return std::nullopt;  // Success
        }
    };


    /// <summary>
    /// Create a parameter set for the given command
    /// </summary>
    template <typename Command, parameter_for<Command>... Parameters>
    [[nodiscard]] static constexpr ParameterSet<Command, Parameters...> parametersFor(
        Parameters &&... parameters) noexcept
    {
        return ParameterSet<Command, Parameters...>(std::forward<Parameters>(parameters)...);
    }


    /// <summary>
    /// Matches when the given type is a parameter set for the command
    /// </summary>
    template <typename Type, typename Command>
    concept parameter_set_for = std::same_as<typename Type::command_type, Command>;


    // clang-format off
    /// <summary>
    /// Matches types that are commands
    /// </summary>
    template <typename Type>
    concept is_command =
        std::default_initializable<Type>
        && std::move_constructible<Type>
        && parameter_set_for<decltype(Type::parameters), Type>;
    // clang-format on


    /// <summary>
    /// Parses a command instance from command line arguments
    /// </summary>
    template <is_command Type>
    Result<Type> parseCommand(Tokens const & tokens) noexcept
    {
        using command_type = Type;
        constexpr auto parameters = command_type::parameters;

        auto command = command_type{};
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
                auto setFlagError = parameters.trySetFlag(command, *flagResult);
                if (setFlagError)
                {
                    return ParseError(remainder, *setFlagError);
                }

                remainder = flagResult.remainder();
                continue;
            }

            return ParseError(remainder, "Unable to parse");
        }

        // ToDo: validation
        // if (auto validateError = parameters.validate(command); validateError)
        // {
        //     return ParseError(tokens, *validateError);
        // }

        return Output<command_type>(std::move(command), remainder);
    }

}  // namespace enumgen