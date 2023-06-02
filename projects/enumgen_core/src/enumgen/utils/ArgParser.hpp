#pragma once

#include <concepts>
#include <span>
#include <string_view>
#include <variant>
#include <vector>


using namespace std::string_view_literals;


namespace enumgen::utils
{

    using token_type = std::string_view;

    std::vector<token_type> preprocess(int argc, char ** argv) noexcept;

    struct Tokens final
    {
        int index;
        std::span<token_type> tokens;

        Tokens(int index, std::span<token_type> tokens) noexcept;

        [[nodiscard]] bool empty() const noexcept;
        [[nodiscard]] size_t size() const noexcept;
        [[nodiscard]] token_type front() const noexcept;
        [[nodiscard]] std::pair<token_type, Tokens> takeOne() const noexcept;
        [[nodiscard]] Tokens skip(int count = 1) const noexcept;
    };

    struct ParseError final
    {
        Tokens input;
        std::string_view error;

        ParseError(Tokens input, std::string_view error) noexcept;
    };

    template <typename Value>
    struct Output final
    {
        using value_type = Value;

        value_type value;
        Tokens remainder;

        explicit Output(value_type value, Tokens remainder) noexcept : value(value), remainder(remainder)
        { }
    };

    template <typename Value>
    class Result final
    {
        using value_type = Value;
        using remainder_type = Tokens;
        using output_type = Output<value_type>;
        using error_type = ParseError;
        using storage_type = std::variant<output_type, error_type>;

    private:
        storage_type storage;

    public:
        Result(output_type output) : storage(std::move(output))
        { }

        Result(error_type error) : storage(std::move(error))
        { }

        [[nodiscard]] operator bool() const noexcept
        {
            return std::holds_alternative<output_type>(storage);
        }

        [[nodiscard]] output_type * operator->()
        {
            return &std::get<output_type>(storage);
        }

        [[nodiscard]] output_type const * operator->() const
        {
            return &std::get<output_type>(storage);
        }

        [[nodiscard]] const output_type & Output() const
        {
            return std::get<output_type>(storage);
        }

        [[nodiscard]] const error_type & Error() const
        {
            return std::get<error_type>(storage);
        }
    };


    namespace errors
    {

        constexpr std::string_view noRemainingTokens = "No remaining tokens to parse"sv;
        constexpr std::string_view tokenIsEmpty = "Current token is empty"sv;
        constexpr std::string_view isOptionFlag = "Current token is an option flag"sv;
        constexpr std::string_view isNotOptionFlag = "Current token is not an option flag"sv;
        constexpr std::string_view notOption = "Current token is not an option"sv;

    }  // namespace errors


    using Verb = std::string_view;
    using OptionFlag = std::string_view;
    using OptionValue = std::string_view;

    struct Option
    {
        std::string_view flag;
        std::string_view value;

        Option() = default;

        explicit Option(std::string_view flag, std::string_view value) noexcept : flag(flag), value(value)
        { }
    };

    // Compiler is unhappy without the this specialization :(
    template <>
    struct Output<Option> final : public Option
    {
        Tokens remainder;

        Output(std::string_view flag, std::string_view value, Tokens remainder) noexcept : Option(flag, value), remainder(remainder)
        { }
    };


    Result<Verb> parseVerb(Tokens const & input) noexcept;

    Result<OptionFlag> parseOptionFlag(Tokens const & input) noexcept;

    Result<OptionValue> parseOptionValue(Tokens const & input) noexcept;

    Result<Option> parseOption(Tokens const & input) noexcept;


}  // namespace enumgen::utils