#include <enumgen/Config.h>
#include <enumgen/Enums.h>

#include <fmt/printf.h>
#include <fmt/std.h>


void showUsage()
{
    fmt::print("usage: enumgen <inputFile> [<configFile>]\n");
}


int main(int argc, char** argv, int envc, char** envv)
{
    if (!(argc == 2 || argc == 3))
    {
        showUsage();
        return 0;
    }

    auto inputFile = std::filesystem::absolute(argv[1]).make_preferred();
    if (!std::filesystem::exists(inputFile))
    {
        fmt::print("Input file not found:\n    {}\n", inputFile);
        return -1;
    }

    auto configFile = argc == 3 ? std::filesystem::absolute(argv[2]).make_preferred() : enumgen::findConfigFile(inputFile);
    auto config = enumgen::tryReadConfig(configFile);
    if (!config || !enumgen::validateConfig(*config))
    {
        return -1;
    }

    try
    {
        enumgen::generateEnums(inputFile, *config);
    }
    catch (const std::exception& e)
    {
        fmt::print("Error: {}\n", e.what());
        return -1;
    }

    return 0;
}