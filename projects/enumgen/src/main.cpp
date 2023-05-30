#include <enumgen/Config.hpp>
#include <enumgen/Enums.hpp>
#include <enumgen/Version.hpp>

#include <fmt/printf.h>
#include <fmt/std.h>


void showUsage()
{
    fmt::print("usage: enumgen <inputFile> <configFile> <outputPath>\n");
}


int generate([[maybe_unused]] int argc, char ** argv)
{
    auto inputFile = std::string_view(argv[1ul], std::strlen(argv[1ul]));
    auto configFile = std::string_view(argv[2ul], std::strlen(argv[2ul]));
    auto outputPath = std::string_view(argv[3ul], std::strlen(argv[3ul]));

    try
    {
        auto result = enumgen::generateEnums(inputFile, configFile, outputPath);
        if (!result)
        {
            return -1;
        }
    }
    catch (const std::exception & e)
    {
        fmt::print("Error: {}\n", e.what());
        return -1;
    }

    return 0;
}


int main(int argc, char ** argv)
{
    if (argc == 2)
    {
        auto arg = std::string_view(argv[1ul], std::strlen(argv[1ul]));
        if (arg == "version"sv || arg == "-v"sv || arg == "--version"sv)
        {
            fmt::print("enumgen version: {}\n", enumgen::Version);
            return 0;
        }
    }

    if (argc == 4)
    {
        return generate(argc, argv);
    }

    showUsage();
    return 0;
}