#pragma once

#include <enumgen/utils/ArgParser.hpp>
#include <enumgen/utils/Traits.hpp>

#include <optional>
#include <string_view>
#include <tuple>


using namespace std::string_view_literals;


namespace enumgen::utils
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
                func(std::get<Index>(tuple));
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
                auto result = func(std::get<Index>(tuple));
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


    /// <summary>
    /// Contains details needed to parse a command line argument matching a parameter into a command instance
    /// </summary>
    /// <typeparam name="MemberValue"></typeparam>
    template <auto MemberValue>
        requires utils::member_value<MemberValue>
    struct Parameter final
    {
        using command_type = utils::member_value_class_t<MemberValue>;
        using member_value_type = utils::member_value_t<MemberValue>;

        std::string_view shortName;
        std::string_view longName;
        std::optional<member_value_type> defaultValue;
        // ToDo: bool required = std::is_optional<member_value_type> or std::function<bool(member_value_type)>
        // validate Maybe: track values set?

        constexpr Parameter(std::string_view shortName, std::string_view longName) noexcept
          : shortName(shortName), longName(longName), defaultValue(std::nullopt)
        { }

        constexpr Parameter(
            std::string_view shortName, std::string_view longName, member_value_type defaultValue) noexcept
          : shortName(shortName), longName(longName), defaultValue(defaultValue)
        { }

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


    /// <summary>
    /// Matches when the given type is an parameter for the command
    /// </summary>
    template <typename Type, typename Command>
    concept ParameterFor = std::same_as<typename Type::command_type, Command>;


    /// <summary>
    /// Collection of parameters used to parse command line arguments into a command instance
    /// </summary>
    template <typename Command, ParameterFor<Command>... Args>
    struct Parameters final
    {
        using command_t = Command;

        std::tuple<Args...> arguments;

        constexpr Parameters(Args &&... args) noexcept : arguments(std::forward<Args>(args)...)
        { }

        /// <summary>
        /// Sets default values for all parameters
        /// </summary>
        /// <param name="command"></param>
        void setDefaults(command_t & command) const noexcept
        {
            auto setDefault = [&](auto const & argument) {
                argument.setDefault(command);
            };

            detail::forEach(arguments, setDefault);
        }

        /// <summary>
        /// Attempts to set the value of the parameter matching the given name
        /// </summary>
        [[nodiscard]] std::optional<std::string_view> trySetOption(
            command_t & command, std::string_view name, std::string_view value) const noexcept
        {
            auto trySetValue = [&](auto const & argument) -> bool {
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
            command_t & command, Option const & option) const noexcept
        {
            return this->trySetOption(command, option.flag.token, option.value.value);
        }
    };


    /// <summary>
    /// Base type for parameters with a bu
    /// </summary>
    template <typename Command>
    struct Parameters<Command>
    {
        using command_t = Command;

        /// <summary>
        /// Helper function to create an collection of command parameters
        /// </summary>
        template <ParameterFor<command_t>... Args>
        static constexpr Parameters<command_t, Args...> Add(Args &&... args)
        {
            return Parameters<command_t, Args...>(std::forward<Args>(args)...);
        }
    };

}  // namespace enumgen::utils