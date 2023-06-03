#include <enumgen/utils/ArgParser.hpp>


namespace enumgen::utils
{
    namespace
    {

        std::string_view stripQuotes(std::string_view token) noexcept
        {
            if (token.size() == 1 || token[0ul] != '"' || token[token.size() - 1ul] != '"')
            {
                return token;
            }

            return token.substr(1ul, token.size() - 2ul);
        }

        bool startsWithFlagMarker(std::string_view str) noexcept
        {
            if (str.size() < 2ul)
            {
                return false;
            }

            if (str[0ul] == '/')
            {
                return true;
            }

            if (str[0ul] == '-')
            {
                return true;
            }

            return false;
        }

        std::string_view stripFlagMarker(std::string_view str) noexcept
        {
            return str.starts_with("--"sv) ? str.substr(2ul) : str.substr(1ul);
        }

    }  // namespace


    std::vector<std::string_view> preprocess(int argc, char ** argv) noexcept
    {
        if (argc < 0)
        {
            return {};
        }

        std::vector<std::string_view> args;
        args.reserve(argc);

        for (auto index = 0ul; index < static_cast<size_t>(argc); ++index)
        {
            args.emplace_back(std::string_view(argv[index], std::strlen(argv[index])));
        }

        return args;
    }


    Tokens::Tokens(int index, std::span<token_type> tokens) noexcept : index(index), tokens(tokens)
    { }

    bool Tokens::empty() const noexcept
    {
        return tokens.empty();
    }

    size_t Tokens::size() const noexcept
    {
        return tokens.size();
    }

    token_type Tokens::front() const noexcept
    {
        return tokens.front();
    }

    std::pair<token_type, Tokens> Tokens::takeOne() const noexcept
    {
        return std::make_pair(front(), Tokens(index + 1, tokens.subspan(1ul)));
    }

    Tokens Tokens::skip(int count) const noexcept
    {
        return Tokens(index + count, tokens.subspan(count));
    }


    ParseError::ParseError(Tokens input, std::string_view error) noexcept : input(input), error(error)
    { }


    Verb::Verb(std::string_view value) noexcept : value(value)
    { }


    Flag::Flag(std::string_view token, std::string_view value) noexcept : token(token), value(value)
    { }


    Value::Value(std::string_view token, std::string_view value) noexcept : token(token), value(value)
    { }


    Option::Option(Flag flag, Value value) noexcept : flag(std::move(flag)), value(std::move(value))
    { }


    Result<Verb> parseVerb(Tokens const & input) noexcept
    {
        if (input.empty())
        {
            return ParseError(input, errors::noRemainingTokens);
        }

        auto [token, remainder] = input.takeOne();

        if (token.empty())
        {
            return ParseError(input, errors::tokenIsEmpty);
        }

        if (token[0ul] == '/' || token[0ul] == '-')
        {
            return ParseError(input, errors::isOptionFlag);
        }

        return Output<Verb>(Verb(token), remainder);
    }


    Result<Flag> parseFlag(Tokens const & input) noexcept
    {
        if (input.empty())
        {
            return ParseError(input, errors::noRemainingTokens);
        }

        auto [token, remainder] = input.takeOne();

        if (token.empty())
        {
            return ParseError(input, errors::tokenIsEmpty);
        }

        if (!startsWithFlagMarker(token))
        {
            return ParseError(input, errors::isNotOptionFlag);
        }

        auto value = stripFlagMarker(token);

        return Output<Flag>(Flag(token, value), remainder);
    }


    Result<Value> parseValue(Tokens const & input) noexcept
    {
        if (input.empty())
        {
            return ParseError(input, errors::noRemainingTokens);
        }

        auto [token, remainder] = input.takeOne();

        if (token.empty())
        {
            return ParseError(input, errors::tokenIsEmpty);
        }

        if (token[0ul] == '/' || token[0ul] == '-')
        {
            return ParseError(input, errors::isOptionFlag);
        }

        return Output<Value>(Value(token, stripQuotes(token)), remainder);
    }


    namespace
    {

        Result<Option> parseCombinedOption(Tokens const & input) noexcept
        {
            // Note: not handling some pathological cases, eg --o"="

            auto [first, remainder] = input.takeOne();

            auto equalsPos = first.find('=');

            auto flagToken = first.substr(0ul, equalsPos);
            auto flagValue = stripFlagMarker(flagToken);

            auto valueToken = first.substr(equalsPos + 1ul);
            auto valueValue = stripQuotes(valueToken);

            return Output<Option>(Option(Flag(flagToken, flagValue), Value(valueToken, valueValue)), remainder);
        }


        Result<Option> parseSeparatedOption(Tokens const & input) noexcept
        {
            auto flagResult = parseFlag(input);
            auto valueResult = parseValue(flagResult.remainder());

            return Output<Option>(Option(*flagResult, *valueResult), valueResult.remainder());
        }

    }  // namespace


    Result<Option> parseOption(Tokens const & input) noexcept
    {
        if (input.empty())
        {
            return ParseError(input, errors::noRemainingTokens);
        }

        auto [first, _] = input.takeOne();

        if (!startsWithFlagMarker(first))
        {
            return ParseError(input, errors::notOption);
        }

        auto equalsPos = first.find('=');
        return equalsPos != std::string_view::npos ? parseCombinedOption(input) : parseSeparatedOption(input);
    }

}  // namespace enumgen::utils