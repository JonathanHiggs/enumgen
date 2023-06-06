#include <enumgen/tests/CommandLineHelpers.hpp>
#include <enumgen/utils/Parser.hpp>

#include <gtest/gtest.h>

#include <memory>


namespace enumgen::tests
{

    TEST(ArgParserTests, preprocess)
    {
        // Arrange
        const auto args = createArgs({ "program", "verb", "-option", "value1", "-option", "value2" });

        // Act
        auto result = preprocess(args.argc, args.argv);

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
        const auto args = createArgs({ "program", "verb", "-option", "value1", "-option", "value2" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseVerb(tokens.skip(1));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->value, "verb");
        EXPECT_EQ(result.remainder().size(), 4ul);
    }

    TEST(ArgParserTests, parseVerb_WithDashFlag_Fails)
    {
        // Arrange
        const auto args = createArgs({ "program", "verb", "-option", "--option", "/option" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseVerb(tokens.skip(2));

        // Assert
        EXPECT_FALSE(result);
    }

    TEST(ArgParserTests, parseVerb_WithDoubleDashFlag_Fails)
    {
        // Arrange
        const auto args = createArgs({ "program", "verb", "-option", "--option", "/option" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseVerb(tokens.skip(3));

        // Assert
        EXPECT_FALSE(result);
    }

    TEST(ArgParserTests, parseVerb_WithSlashFlag_Fails)
    {
        // Arrange
        const auto args = createArgs({ "program", "verb", "-option", "--option", "/option" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseVerb(tokens.skip(4));

        // Assert
        EXPECT_FALSE(result);
    }

    TEST(ArgParserTests, parseFlag_WithDash)
    {
        // Arrange
        const auto args = createArgs({ "program", "verb", "-f", "--op", "/M" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseFlag(tokens.skip(2));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->value, "f");
        EXPECT_EQ(result.remainder().size(), 2ul);
    }

    TEST(ArgParserTests, parseFlag_WithDoubleDash)
    {
        // Arrange
        const auto args = createArgs({ "program", "verb", "-f", "--op", "/M" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseFlag(tokens.skip(3));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->value, "op");
        EXPECT_EQ(result.remainder().size(), 1ul);
    }

    TEST(ArgParserTests, parseFlag_WithSlash)
    {
        // Arrange
        const auto args = createArgs({ "program", "verb", "-f", "--op", "/M" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseFlag(tokens.skip(4));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->value, "M");
        EXPECT_EQ(result.remainder().size(), 0ul);
    }

    TEST(ArgParserTests, parseOption_WithDashFlag)
    {
        // Arrange
        const auto args = createArgs({ "program", "verb", "-option", "value1", "-option", "value2" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseOption(tokens.skip(2));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->flag.value, "option");
        EXPECT_EQ(result->value.value, "value1");
        EXPECT_EQ(result.remainder().size(), 2ul);
    }

    TEST(ArgParserTests, parseOption_WithDoubleDashFlag)
    {
        // Arrange
        const auto args = createArgs({ "program", "verb", "--option", "value1", "-option", "value2" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseOption(tokens.skip(2));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->flag.value, "option");
        EXPECT_EQ(result->value.value, "value1");
        EXPECT_EQ(result.remainder().size(), 2ul);
    }

    TEST(ArgParserTests, parseOption_WithSlashFlag)
    {
        // Arrange
        const auto args = createArgs({ "program", "verb", "/option", "value1", "-option", "value2" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseOption(tokens.skip(2));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->flag.value, "option");
        EXPECT_EQ(result->value.value, "value1");
        EXPECT_EQ(result.remainder().size(), 2ul);
    }

    TEST(ArgParserTests, parseOption_WithQuotes)
    {
        // Arrange
        const auto args = createArgs({ "program", "verb", "-option", "\"value1\"", "-option", "value2" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseOption(tokens.skip(2));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->flag.value, "option");
        EXPECT_EQ(result->value.value, "value1");
        EXPECT_EQ(result.remainder().size(), 2ul);
    }

    TEST(ArgParserTests, parseOption_WithEquals)
    {
        // Arrange
        const auto args = createArgs({ "program", "verb", "-option=value1", "-option", "value2" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseOption(tokens.skip(2));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->flag.value, "option");
        EXPECT_EQ(result->value.value, "value1");
        EXPECT_EQ(result.remainder().size(), 2ul);
    }

    TEST(ArgParserTests, parseOption_WithEqualsAndQuotes)
    {
        // Arrange
        const auto args = createArgs({ "program", "verb", "-option=\"value1\"", "-option", "value2" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act
        const auto result = parseOption(tokens.skip(2));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->flag.value, "option");
        EXPECT_EQ(result->value.value, "value1");
        EXPECT_EQ(result.remainder().size(), 2ul);
    }

    TEST(ArgParserTests, parseArgs)
    {
        // Arrange
        const auto args = createArgs({ "program", "verb", "-option=\"value1\"", "-option", "value2", "-f", "--F2" });
        auto processed = preprocess(args.argc, args.argv);

        auto tokens = Tokens(0, std::span{ processed });

        // Act & Assert
        const auto verbResult = parseVerb(tokens.skip(1));
        ASSERT_TRUE(verbResult);
        EXPECT_EQ(verbResult->value, "verb"sv);

        const auto optionResult1 = parseOption(verbResult.remainder());
        ASSERT_TRUE(optionResult1);
        EXPECT_EQ(optionResult1->flag.value, "option");
        EXPECT_EQ(optionResult1->value.value, "value1");

        const auto optionResult2 = parseOption(optionResult1.remainder());
        ASSERT_TRUE(optionResult2);
        EXPECT_EQ(optionResult2->flag.value, "option");
        EXPECT_EQ(optionResult2->value.value, "value2");

        const auto flagResult1 = parseFlag(optionResult2.remainder());
        ASSERT_TRUE(flagResult1);
        EXPECT_EQ(flagResult1->value, "f");

        const auto flagResult2 = parseFlag(flagResult1.remainder());
        ASSERT_TRUE(flagResult2);
        EXPECT_EQ(flagResult2->value, "F2");

        const auto eofResult = parseOption(flagResult2.remainder());
        ASSERT_FALSE(eofResult);
    }

}  // namespace enumgen::tests