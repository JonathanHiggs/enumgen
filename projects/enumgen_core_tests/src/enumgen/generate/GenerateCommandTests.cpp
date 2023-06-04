#include <enumgen/generate/GenerateCommand.hpp>
#include <enumgen/helpers/CommandLineHelpers.hpp>
#include <enumgen/utils/ArgParser.hpp>

#include <gtest/gtest.h>


using namespace enumgen::helpers;
using namespace enumgen::utils;


namespace enumgen::generate::tests
{

    TEST(GenerateCommandTests, parseGenerateCommand)
    {
        // Arrange
        const auto args = createArgs({ "enumgen.exe"sv,
                                       "generate"sv,
                                       "-i"sv,
                                       "input.json"sv,
                                       "-c"sv,
                                       "config.json"sv,
                                       "-b"sv,
                                       "./path/to/binary"sv,
                                       "-s"sv,
                                       "./path/to/source"sv });

        auto preprocessed = preprocess(args.argc, args.argv);
        auto tokens = Tokens(0, std::span{ preprocessed });

        // Act
        const auto result = parseGenerateCommand(tokens.skip(2));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->specifications, "input.json"sv);
        EXPECT_EQ(result->config, "config.json"sv);
        EXPECT_EQ(result->binaryDirectory, "./path/to/binary"sv);
        EXPECT_EQ(result->sourceDirectory, "./path/to/source"sv);
    }

    TEST(GenerateCommandTests, parseGenerateCommand_WithDefaults)
    {
        // Arrange
        const auto args = createArgs({ "enumgen.exe"sv,
                                       "generate"sv,
                                       "-b"sv,
                                       "./path/to/binary"sv,
                                       "-s"sv,
                                       "./path/to/source"sv });

        auto preprocessed = preprocess(args.argc, args.argv);
        auto tokens = Tokens(0, std::span{ preprocessed });

        // Act
        const auto result = parseGenerateCommand(tokens.skip(2));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->specifications, "./enumgen/enums.json"sv);
        EXPECT_EQ(result->config, "./enumgen/config.json"sv);
        EXPECT_EQ(result->binaryDirectory, "./path/to/binary"sv);
        EXPECT_EQ(result->sourceDirectory, "./path/to/source"sv);
    }

}  // namespace enumgen::generate::tests