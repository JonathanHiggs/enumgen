#include <codegen/EnumGenerator.h>

#include <fmt/core.h>
#include <fmt/printf.h>
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>


namespace codegen
{

    void EnumGenerator::Generate() const
    {
        try
        {
            auto workingPath = std::filesystem::current_path();
            fmt::print("Current Path: {}\n", workingPath.string());

            fmt::print("Initialize inja\n");
            auto env = inja::Environment();
            env.set_trim_blocks(true);
            // env.set_lstrip_blocks(true);

            auto inputFile = std::filesystem::absolute(workingPath / "data/enum.json").make_preferred();
            fmt::print("Loading input data from:\n    {}\n", inputFile.string());
            auto json = env.load_json(inputFile.string());

            auto templatesPath = std::filesystem::absolute(workingPath / "templates").make_preferred();
            auto headerTemplatePath = std::filesystem::absolute(templatesPath / "enum.h.inja").make_preferred();
            auto codeTemplatePath = std::filesystem::absolute(templatesPath / "enum.cpp.inja").make_preferred();

            auto outputPath = std::filesystem::path(json["outputPath"].get<std::string>());
            auto outputName = json["outputName"].get<std::string>();

            auto headerPath = std::filesystem::absolute(outputPath / fmt::format("{}.h", outputName)).make_preferred();
            auto codePath = std::filesystem::absolute(outputPath / fmt::format("{}.cpp", outputName)).make_preferred();

            fmt::print("Writing output to:\n    {}\n    {}\n", headerPath.string(), codePath.string());

            fmt::print("Generating header\n");
            env.write(headerTemplatePath.string(), json, headerPath.string());

            fmt::print("Generating code\n");
            env.write(codeTemplatePath.string(), json, codePath.string());

            fmt::print("Templates generated\n");
        }
        catch (std::exception const& ex)
        {
            fmt::print("Error: {}\n", ex.what());
            // throw;
        }
    }

}