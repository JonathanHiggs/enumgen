#pragma once

#include <enumgen/utils/Parser.hpp>
#include <enumgen/utils/Traits.hpp>

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
        constexpr bool doUntil(std::tuple<Tp...> const & tuple, Func && func, Condition && condition) noexcept
        {
            if constexpr (Index < sizeof...(Tp))
            {
                auto result = func(std::get<Index>(tuple));  // ToDo: handle throwing functions
                if (condition(result))
                {
                    return true;
                }

                return doUntil<Index + 1>(tuple, std::forward<Func>(func), std::forward<Condition>(condition));
            }
            else
            {
                return false;
            }
        }

    }  // namespace detail


    // Maybe: handle value conversions
    // template <typename ValueType>
    // struct ConvertValue;
    //
    // template <>
    // struct ConvertValue<std::string_view>
    // {
    //     using value_type = std::string_view;
    //
    //     static constexpr value_type convert(std::string_view value) noexcept
    //     {
    //         return value;
    //     }
    // };
    //
    // template <>
    // struct ConvertValue<bool>
    // {
    //     using value_type = bool;
    //
    //     static constexpr value_type convert(std::string_view value) noexcept
    //     {
    //         // ToDo:
    //         return true;
    //     }
    // };


    /// <summary>
    /// Contains details needed to parse a command line argument matching a parameter into a command instance
    /// </summary>
    template <auto MemberValue>
        requires member_value<MemberValue>
    struct Parameter final
    {
        // ToDo: bool required = std::is_optional<member_value_type> or std::function<bool(member_value_type)>
        // ToDo: validation
        // ToDo: handle flags (std::same_as<member_value_type, bool>)
        // Maybe: track values set?

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

        // Maybe: [[nodiscard]] constexpr bool isFlag() const noexcept { return std::same_as<member_value_type, bool>; }

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
        /// Sets the value of the parameter to the given value
        /// </summary>
        [[nodiscard]] void setValue(command_type & command, std::string_view value) const noexcept
        {
            command.*MemberValue = value;
        }
    };


    // Maybe: parameter builder class to give a nicer interface


    /// <summary>
    /// Matches when the given type is an parameter for the command
    /// </summary>
    template <typename Type, typename Command>
    concept parameter_for = std::same_as<typename Type::command_type, Command>;


    /// <summary>
    /// Collection of parameters used to parse command line arguments into a command instance
    /// </summary>
    template <typename Command, parameter_for<Command>... Args>
    struct ParameterSet final
    {
        using command_type = Command;

        std::tuple<Args...> arguments;

        constexpr ParameterSet(Args &&... args) noexcept : arguments(std::forward<Args>(args)...)
        { }

        /// <summary>
        /// Sets default values for all parameters
        /// </summary>
        /// <param name="command"></param>
        void setDefaults(command_type & command) const noexcept
        {
            auto setDefault = [&](auto const & argument) noexcept {
                argument.setDefault(command);
            };

            detail::forEach(arguments, setDefault);
        }

        /// <summary>
        /// Attempts to set the value of the parameter matching the given name
        /// </summary>
        [[nodiscard]] std::optional<std::string_view> trySetOption(
            command_type & command, std::string_view name, std::string_view value) const noexcept
        {
            auto trySetValue = [&](auto const & argument) noexcept -> bool {
                if (!argument.matchesName(name))
                {
                    return false;
                }

                argument.setValue(command, value);
                return true;
            };

            auto stopCondition = [](bool valueSet) noexcept -> bool {
                return valueSet;
            };

            auto valueSet = detail::doUntil(arguments, trySetValue, stopCondition);
            if (valueSet)
            {
                return std::nullopt;
            }

            return "Unknown argument"sv;
        }

        /// <summary>
        /// Attempts to set the value of the parameter matching the given option
        /// </summary>
        [[nodiscard]] std::optional<std::string_view> trySetOption(
            command_type & command, Option const & option) const noexcept
        {
            return this->trySetOption(command, option.flag.token, option.value.value);
        }
    };


    /// <summary>
    /// Create a parameter set for the given command
    /// </summary>
    template <typename Command, parameter_for<Command>... Args>
    [[nodiscard]] static constexpr ParameterSet<Command, Args...> parametersFor(Args &&... args) noexcept
    {
        return ParameterSet<Command, Args...>(std::forward<Args>(args)...);
    }


    /// <summary>
    /// Matches when the given type is a parameter set for the command
    /// </summary>
    // ToDo: make this stronger?
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

            // ToDo: flags
            // auto flagResult = parseFlag(remainder);
            // if (flagResult)
            // {
            //     auto setFlagError = parameters.trySetFlag(command, *flagResult);
            //     if (setFlagError)
            //     {
            //         return ParseError(remainder, *setFlagError);
            //     }
            //
            //     remainder = flagResult.remainder();
            //     continue;
            // }

            return ParseError(remainder, "Unknown option or flag");
        }

        // ToDo: validation
        // if (auto validateError = parameters.validate(command); validateError)
        // {
        //     return ParseError(tokens, *validateError);
        // }

        return Output<command_type>(std::move(command), remainder);
    }

}  // namespace enumgen