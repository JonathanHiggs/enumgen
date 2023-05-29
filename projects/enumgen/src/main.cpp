#include <enumgen/Config.h>
#include <enumgen/Enums.h>

#include <fmt/printf.h>
#include <fmt/std.h>


void showUsage()
{
    fmt::print("usage: enumgen <inputFile> <configFile> <outputPath>\n");
}


int main(int argc, char** argv, int envc, char** envv)
{
    if (!(argc == 4))
    {
        showUsage();
        return 0;
    }

    auto inputFile = std::string_view(argv[1], std::strlen(argv[1]));
    auto configFile = std::string_view(argv[2], std::strlen(argv[2]));
    auto outputPath = std::string_view(argv[3], std::strlen(argv[3]));

    try
    {
        auto result = enumgen::generateEnums(inputFile, configFile, outputPath);
        if (!result)
        {
            return -1;
        }
    }
    catch (const std::exception& e)
    {
        fmt::print("Error: {}\n", e.what());
        return -1;
    }

    return 0;
}