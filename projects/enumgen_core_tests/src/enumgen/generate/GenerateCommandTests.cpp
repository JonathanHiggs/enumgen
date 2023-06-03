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
        const auto args = createArgs({ "enumgen.exe",
                                       "generate",
                                       "-i",
                                       "input.json",
                                       "-c",
                                       "config.json",
                                       "-b",
                                       "./path/to/binary",
                                       "-s",
                                       "./path/to/source" });

        auto preprocessed = preprocess(args.argc, args.argv);
        auto tokens = Tokens(0, std::span{ preprocessed });

        // Act
        const auto result = parseGenerateCommand(tokens.skip(2));

        // Assert
        ASSERT_TRUE(result);
        EXPECT_EQ(result->specifications, "input.json");
        EXPECT_EQ(result->config, "config.json");
        EXPECT_EQ(result->binaryDirectory, "./path/to/binary");
        EXPECT_EQ(result->sourceDirectory, "./path/to/source");
    }

}  // namespace enumgen::generate::tests