#include <codegen/EnumGenerator.h>
#include <fmt/core.h>
#include <fmt/printf.h>
#include <fmt/std.h>
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>


using namespace std::filesystem;


namespace codegen
{

    void generateEnum(std::filesystem::path const & inputFile, Config const & config) noexcept
    {
        try
        {
            fmt::print("Input file:          {}\n", inputFile);
            fmt::print("Config file:         {}\n", config.configFile);
            fmt::print("Templates directory: {}\n", config.templatesDirectory);

            fmt::print("Initialize inja\n");
            auto env = inja::Environment();
            env.set_trim_blocks(true);

            fmt::print("Loading input data from:\n    {}\n", inputFile);
            auto json = env.load_json(inputFile.string());


            auto outputDirectory = std::filesystem::absolute(inputFile.parent_path() / json["outputPath"].get<std::string>()).make_preferred();

            if (!std::filesystem::exists(outputDirectory))
            {
                fmt::print("Creating output directory: {}\n", outputDirectory);
                std::filesystem::create_directories(outputDirectory);
            }

            auto outputName = json["outputName"].get<std::string>();

            auto headerPath
                = std::filesystem::absolute(outputDirectory / fmt::format("{}.h", outputName)).make_preferred();
            auto codePath
                = std::filesystem::absolute(outputDirectory / fmt::format("{}.cpp", outputName)).make_preferred();

            fmt::print(
                "Generating header for {}\n    from: {}\n    to:   {}\n",
                json["enum"]["className"],
                config.enumConfig.headerTemplateFile,
                headerPath);

            env.write(config.enumConfig.headerTemplateFile.string(), json, headerPath.string());

            fmt::print(
                "Generating code for {}\n    from: {}\n    to:   {}\n",
                json["enum"]["className"],
                config.enumConfig.codeTemplateFile,
                codePath);

            env.write(config.enumConfig.codeTemplateFile.string(), json, codePath.string());

            fmt::print("Templates generated\n");
        }
        catch (std::exception const & ex)
        {
            fmt::print("Error generating enums:\n    {}\n", ex.what());
        }
    }

}  // namespace codegen