#include <enumgen/Config.h>

#include <fmt/printf.h>
#include <fmt/std.h>
#include <nlohmann/json.hpp>

#include <fstream>


using namespace std::filesystem;


namespace enumgen
{

    std::filesystem::path findConfigFile(std::filesystem::path const & inputFile) noexcept
    {
        return absolute(inputFile.parent_path() / "enumgen.config.json").make_preferred();
    }

    std::filesystem::path findTemplateDirectory(std::filesystem::path const & configFile, std::string const & hint)
    {
        auto path = absolute(configFile.parent_path() / hint).make_preferred();
        if (exists(path))
        {
            return path;
        }

        return absolute(hint).make_preferred();
    }

    std::optional<Config> tryReadConfig(path const & configFile) noexcept
    {
        auto configFileFullPath = absolute(configFile).make_preferred();
        if (!exists(configFileFullPath))
        {
            fmt::print("Config file not found: {}\n", configFileFullPath);
            return std::nullopt;
        }

        Config config;
        config.configFile = configFileFullPath;

        try
        {
            auto const & json = nlohmann::json::parse(std::ifstream{ configFileFullPath, std::ios::in });

            config.templatesDirectory = findTemplateDirectory(
                configFileFullPath,
                json.contains("templateDirectory") ? json["templateDirectory"].get<std::string>() : "./templates");

            if (json.contains("enums"))
            {
                auto const & enumConfigJson = json["enums"];

                auto headerTemplateName = enumConfigJson.contains("headerTemplateName")
                                              ? enumConfigJson["headerTemplateName"].get<std::string>()
                                              : "enum.h.inja";

                config.enumConfig.headerTemplateFile
                    = absolute(config.templatesDirectory / headerTemplateName).make_preferred();

                auto codeTemplateName = enumConfigJson.contains("codeTemplateName")
                                            ? enumConfigJson["codeTemplateName"].get<std::string>()
                                            : "enum.cpp.inja";

                config.enumConfig.codeTemplateFile
                    = absolute(config.templatesDirectory / codeTemplateName).make_preferred();
            }

            return config;
        }
        catch (std::exception const & ex)
        {
            fmt::print("Error reading config file:\n    file:  {}\n    error: {}\n", configFileFullPath, ex.what());
            return std::nullopt;
        }
    }

    bool validateConfig(Config const & config) noexcept
    {
        bool success = true;

        if (!exists(config.templatesDirectory))
        {
            fmt::print("Templates directory not found:\n    {}\n", config.templatesDirectory);
            success = false;
        }
        else
        {
            if (!exists(config.enumConfig.headerTemplateFile))
            {
                fmt::print("Enum header template not found:\n    {}\n", config.enumConfig.headerTemplateFile);
                success = false;
            }

            if (!exists(config.enumConfig.codeTemplateFile))
            {
                fmt::print("Enum code template not found:\n    {}\n", config.enumConfig.codeTemplateFile);
                success = false;
            }
        }

        return success;
    }

}  // namespace enumgen