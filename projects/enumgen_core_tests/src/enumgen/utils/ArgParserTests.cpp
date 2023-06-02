#include <enumgen/utils/ArgParser.hpp>

#include <gtest/gtest.h>

#include <memory>


namespace enumgen::utils
{
    namespace
    {

        struct ArgsHelper final
        {
            int argc;
            char ** argv;
            std::unique_ptr<char *[]> holder;

            explicit ArgsHelper(std::initializer_list<std::string_view> strings) noexcept
                : argc(static_cast<int>(strings.size()))
            {
                holder = std::unique_ptr<char *[]>(new char *[argc]);

                int index = 0;
                for (auto const & str : strings)
                {
                    holder[index] = const_cast<char *>(str.data());
                    ++index;
                }

                argv = holder.get();
            }
        };

        ArgsHelper createArgs(std::initializer_list<std::string_view> strings)
        {
            return ArgsHelper(std::move(strings));
        }

    }  // namespace

    TEST(ArgParserTests, preprocess)
    {
        // Arrange
        auto args = createArgs({ "program", "verb", "-option", "value1", "-option", "value2" });

        // Act
        const auto result = preprocess(args.argc, args.argv);

        ASSERT_EQ(result.size(), args.argc);
        EXPECT_EQ(result[0ul], "program");
        EXPECT_EQ(result[1ul], "verb");
        EXPECT_EQ(result[2ul], "-option");
        EXPECT_EQ(result[3ul], "value1");
        EXPECT_EQ(result[4ul], "-option");
        EXPECT_EQ(result[5ul], "value2");
    }

    TEST(ArgParserTests, parseVerb)
    {
        // Arrange
        auto args = createArgs({ "program", "verb", "-option", "value1", "-option", "value2" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseVerb(tokens.skip(1));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->value, "verb");
        EXPECT_EQ(result->remainder.size(), 4ul);
    }

    TEST(ArgParserTests, parseOption)
    {
        // Arrange
        auto args = createArgs({ "program", "verb", "-option", "value1", "-option", "value2" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseOption(tokens.skip(2));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->flag, "option");
        EXPECT_EQ(result->value, "value1");
        EXPECT_EQ(result->remainder.size(), 2ul);
    }

    TEST(ArgParserTests, parseOptionWithQuotes)
    {
        // Arrange
        auto args = createArgs({ "program", "verb", "-option", "\"value1\"", "-option", "value2" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseOption(tokens.skip(2));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->flag, "option");
        EXPECT_EQ(result->value, "value1");
        EXPECT_EQ(result->remainder.size(), 2ul);
    }

    TEST(ArgParserTests, parseOptionWithEquals)
    {
        // Arrange
        auto args = createArgs({ "program", "verb", "-option=value1", "-option", "value2" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseOption(tokens.skip(2));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->flag, "option");
        EXPECT_EQ(result->value, "value1");
        EXPECT_EQ(result->remainder.size(), 2ul);
    }

    TEST(ArgParserTests, parseOptionWithEqualsAndQuotes)
    {
        // Arrange
        auto args = createArgs({ "program", "verb", "-option=\"value1\"", "-option", "value2" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseOption(tokens.skip(2));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->flag, "option");
        EXPECT_EQ(result->value, "value1");
        EXPECT_EQ(result->remainder.size(), 2ul);
    }

    TEST(ArgParserTests, parseArgs)
    {
        // Arrange
        auto args = createArgs({ "program", "verb", "-option=\"value1\"", "-option", "value2", "-f", "-f2" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act & Assert
        const auto verbResult = parseVerb(tokens.skip(1));
        ASSERT_TRUE(verbResult);
        EXPECT_EQ(verbResult->value, "verb"sv);

        const auto optionResult1 = parseOption(verbResult->remainder);
        ASSERT_TRUE(optionResult1);
        EXPECT_EQ(optionResult1->flag, "option");
        EXPECT_EQ(optionResult1->value, "value1");

        const auto optionResult2 = parseOption(optionResult1->remainder);
        ASSERT_TRUE(optionResult2);
        EXPECT_EQ(optionResult2->flag, "option");
        EXPECT_EQ(optionResult2->value, "value2");

        const auto flagResult1 = parseOptionFlag(optionResult2->remainder);
        ASSERT_TRUE(flagResult1);
        EXPECT_EQ(flagResult1->value, "f");

        const auto flagResult2 = parseOptionFlag(flagResult1->remainder);
        ASSERT_TRUE(flagResult2);
        EXPECT_EQ(flagResult2->value, "f2");

        const auto eofResult = parseOption(flagResult2->remainder);
        ASSERT_FALSE(eofResult);
    }

}  // namespace enumgen::utils