#include <codegen/Config.h>
#include <codegen/EnumGenerator.h>

#include <fmt/printf.h>
#include <fmt/std.h>


void showUsage()
{
    fmt::print("usage: codegen <inputFile> [<configFile>]\n");
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

    auto configFile = argc == 3 ? std::filesystem::absolute(argv[2]).make_preferred() : codegen::findConfigFile(inputFile);
    auto config = codegen::tryReadConfig(configFile);
    if (!config || !codegen::validateConfig(*config))
    {
        return -1;
    }

    codegen::generateEnum(inputFile, *config);

    return 0;
}