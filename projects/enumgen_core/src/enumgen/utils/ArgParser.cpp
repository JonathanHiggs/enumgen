#include <enumgen/utils/ArgParser.hpp>


namespace enumgen::utils
{

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

        return Output<Verb>(token, remainder);
    }

    Result<OptionFlag> parseOptionFlag(Tokens const & input) noexcept
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

        if (token[0ul] != '/' && token[0ul] != '-')
        {
            return ParseError(input, errors::isNotOptionFlag);
        }

        return Output<OptionFlag>(token.substr(1ul), remainder);
    }

    Result<OptionValue> parseOptionValue(Tokens const & input) noexcept
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

        if (token[0ul] == '"')
        {
            if (token.size() == 1)
            {
                return ParseError(input, "");  // ToDo: error message
            }

            if (token[token.size() - 1ul] != '"')
            {
                return ParseError(input, "");  // ToDo: error message
            }

            token = token.substr(1ul, token.size() - 2ul);
        }

        return Output<OptionValue>(token, remainder);
    }

    Result<Option> parseOption(Tokens const & input) noexcept
    {
        if (input.empty())
        {
            return ParseError(input, errors::noRemainingTokens);
        }

        auto flagResult = parseOptionFlag(input);
        if (!flagResult)
        {
            return ParseError(input, errors::notOption);
        }

        if (auto pos = flagResult->value.find('='); pos != std::string_view::npos)
        {
            auto flagToken = flagResult->value.substr(0ul, pos);
            auto valueToken = flagResult->value.substr(pos + 1ul);

            if (valueToken.empty())
            {
                return ParseError(input, "option token is empty");  // ToDo: error message
            }

            if (valueToken[0ul] == '"')
            {
                if (valueToken.size() == 1)
                {
                    return ParseError(input, "");  // ToDo: error message
                }

                if (valueToken[valueToken.size() - 1ul] != '"')
                {
                    return ParseError(input, "");  // ToDo: error message
                }

                valueToken = valueToken.substr(1ul, valueToken.size() - 2ul);
            }

            return Output<Option>(flagToken, valueToken, flagResult->remainder);
        }

        auto valueResult = parseOptionValue(flagResult->remainder);
        if (!valueResult)
        {
            return ParseError(input, errors::notOption);
        }

        return Output<Option>(flagResult->value, valueResult->value, valueResult->remainder);
    }

}  // namespace enumgen::utils