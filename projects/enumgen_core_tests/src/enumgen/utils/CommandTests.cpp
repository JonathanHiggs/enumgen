#include <enumgen/tests/CommandLineHelpers.hpp>
#include <enumgen/utils/Commands.hpp>

#include <gtest/gtest.h>


namespace enumgen::tests
{
    namespace
    {

        struct TestCommand final
        {
            std::string_view value;
            std::string_view valueWithDefault;
            bool flag;
            bool flagWithDefault;

            static constexpr auto parameters = parametersFor<TestCommand>(
                Parameter<&TestCommand::value>("-v", "--value"),
                Parameter<&TestCommand::valueWithDefault>("-w", "--valueWithDefault", "default"),
                Parameter<&TestCommand::flag>("-f", "--flag"),
                Parameter<&TestCommand::flagWithDefault>("-g", "--flagWithDefault", true));
        };

        static_assert(std::default_initializable<TestCommand>);
        static_assert(std::move_constructible<TestCommand>);
        static_assert(parameter_set_for<decltype(TestCommand::parameters), TestCommand>);

    }  // namespace

    TEST(CommandTests, parseCommand_WithDefaults)
    {
        // Arrange
        const auto args = createArgs({ "-v"sv, "value"sv, "-f"sv });
        auto preprocessed = preprocess(args.argc, args.argv);
        auto tokens = Tokens(0, std::span{ preprocessed });

        // Act
        const auto result = parseCommand<TestCommand>(tokens);

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->value, "value"sv);
        EXPECT_EQ(result->valueWithDefault, "default"sv);
        EXPECT_EQ(result->flag, true);
        EXPECT_EQ(result->flagWithDefault, true);
    }

    TEST(CommandTests, parseCommand_WithSetOverDefault)
    {
        // Arrange
        const auto args = createArgs({ "-v"sv, "value"sv, "-w", "not-default", "-f"sv });
        auto preprocessed = preprocess(args.argc, args.argv);
        auto tokens = Tokens(0, std::span{ preprocessed });

        // Act
        const auto result = parseCommand<TestCommand>(tokens);

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->value, "value"sv);
        EXPECT_EQ(result->valueWithDefault, "not-default"sv);
        EXPECT_EQ(result->flag, true);
        EXPECT_EQ(result->flagWithDefault, true);
    }

    TEST(CommandTests, parseCommand_WithSetFlagFalse)
    {
        // Arrange
        const auto args = createArgs({ "-v"sv, "value"sv, "-f"sv, "-g"sv, "FALSE"sv });
        auto preprocessed = preprocess(args.argc, args.argv);
        auto tokens = Tokens(0, std::span{ preprocessed });

        // Act
        const auto result = parseCommand<TestCommand>(tokens);

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->value, "value"sv);
        EXPECT_EQ(result->valueWithDefault, "default"sv);
        EXPECT_EQ(result->flag, true);
        EXPECT_EQ(result->flagWithDefault, false);
    }

}  // namespace enumgen::tests