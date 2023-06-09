#include <enumgen/Enums.hpp>
#include <enumgen/Logging.hpp>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <fmt/std.h>
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <functional>
#include <vector>


using namespace fmt::literals;
using namespace nlohmann;
using namespace std::filesystem;
using namespace std::string_view_literals;


namespace enumgen
{
    namespace
    {

        static constexpr std::string_view headerRootField = "headerRoot"sv;
        static constexpr std::string_view codeRootField = "codeRoot"sv;
        static constexpr std::string_view enumsField = "enums"sv;

        static constexpr std::string_view nameField = "name"sv;
        static constexpr std::string_view descriptionField = "description"sv;
        static constexpr std::string_view headerPathField = "headerPath"sv;
        static constexpr std::string_view codePathField = "codePath"sv;
        static constexpr std::string_view includePathField = "includePath"sv;
        static constexpr std::string_view namespaceField = "namespace"sv;
        static constexpr std::string_view underlyingTypeField = "underlyingType"sv;
        static constexpr std::string_view defaultField = "default"sv;

        static constexpr std::string_view itemsArray = "items"sv;
        static constexpr std::string_view itemNameField = "name"sv;
        static constexpr std::string_view itemValueField = "value"sv;
        static constexpr std::string_view itemStringField = "string"sv;
        static constexpr std::string_view itemAltsArray = "alts"sv;

        struct requirement final
        {
            std::function<bool(json const &)> check;
            std::string errorMessage;
        };

        using errors = std::vector<std::string>;
        using requirements = std::vector<requirement>;

        bool required(
            json const & node,
            std::string_view name,
            std::string_view field,
            std::string missingMessage,
            requirements requirements,
            errors & errors) noexcept
        {
            if (!node.contains(field))
            {
                errors.emplace_back(
                    fmt::vformat(missingMessage, fmt::make_format_args("name"_a = name, "field"_a = field)));
                return false;
            }

            auto const & value = node[field];
            for (auto const & requirement : requirements)
            {
                if (!requirement.check(value))
                {
                    errors.emplace_back(fmt::vformat(
                        requirement.errorMessage,
                        fmt::make_format_args("name"_a = name, "field"_a = field)));
                    return false;
                }
            }

            return true;
        }

        bool optional(
            json const & node,
            std::string_view name,
            std::string_view field,
            requirements requirements,
            errors & errors) noexcept
        {
            if (!node.contains(field))
            {
                return true;
            }

            auto success = true;
            auto const & value = node[field];
            for (auto const & requirement : requirements)
            {
                if (!requirement.check(value))
                {
                    errors.emplace_back(fmt::vformat(
                        requirement.errorMessage,
                        fmt::make_format_args("name"_a = name, "field"_a = field)));
                    success = false;
                }
            }

            return success;
        }

        bool stringNotEmpty(json const & node) noexcept
        {
            auto value = node.get<std::string_view>();
            return !value.empty() && value.size() != 0ul;
        }

        bool allItemsStrings(json const & node) noexcept
        {
            for (auto const & item : node)
            {
                if (!item.is_string())
                {
                    return false;
                }
            }

            return true;
        }

        void validateItem(std::string_view enumName, json const & node, errors & errors) noexcept
        {
            auto hasItemName = required(
                node,
                enumName,
                itemNameField,
                "Enum '{name}' has item with missing '{field}' field",
                requirements{ requirement{ &json::is_string, "Enum '{name}' item has non-string '{field}' field" },
                              requirement{ stringNotEmpty, "Enum '{name}' item has empty string '{field}' field" } },
                errors);

            if (!hasItemName)
            {
                return;
            }

            auto name = fmt::format("{}::{}", enumName, node[nameField].get<std::string_view>());

            required(
                node,
                name,
                itemStringField,
                "Enum item '{name}' is missing '{field}' field",
                requirements{ requirement{ &json::is_string, "Enum item '{name}::{field}' has non-string value" },
                              requirement{ stringNotEmpty, "Enum item '{name}::{field}' has empty string value" } },
                errors);

            optional(
                node,
                name,
                itemValueField,
                requirements{
                    requirement{ &json::is_number_integer, "Enum item '{name}::{field}' has non-int value" } },
                errors);

            optional(
                node,
                name,
                itemAltsArray,
                requirements{ requirement{ &json::is_array, "Enum item '{name}::{field}' has non-array value" },
                              requirement{ allItemsStrings, "Enum item '{name}::{field}' has non-string values" } },
                errors);
        }

        void validateEnum(json const & node, errors & errors) noexcept
        {
            auto hasName = required(
                node,
                ""sv,
                nameField,
                "Enum description is missing required '{field}' field",
                requirements{ requirement{ &json::is_string, "Enum description has non-string '{field}' field" },
                              requirement{ stringNotEmpty, "Enum description has empty '{field}' field" } },
                errors);

            if (!hasName)
            {
                return;
            }

            auto name = node[nameField].get<std::string_view>();

            constexpr auto requiredFields
                = std::array{ headerPathField, codePathField, includePathField, namespaceField };

            for (auto const & field : requiredFields)
            {
                required(
                    node,
                    name,
                    field,
                    "Enum '{name}' is missing required '{field}' field",
                    requirements{ requirement{ &json::is_string, "Enum '{name}::{field}' should hold a string" },
                                  requirement{ stringNotEmpty, "Enum '{name}::{field}' cannot be empty" } },
                    errors);
            }

            constexpr auto optionalFields = std::array{ descriptionField, underlyingTypeField, defaultField };

            for (auto const & field : optionalFields)
            {
                optional(
                    node,
                    name,
                    field,
                    requirements{ requirement{ &json::is_string, "Enum '{name}::{field}' should hold a string" },
                                  requirement{ stringNotEmpty, "Enum '{name}::{field}' cannot be empty" } },
                    errors);
            }

            auto hasItems = required(
                node,
                name,
                itemsArray,
                "Enum '{name}' is missing required '{field}' field",
                requirements{ requirement{ &json::is_array, "Enum '{name}::field}' is not an array" } },
                errors);

            if (!hasItems)
            {
                return;
            }

            for (auto const & item : node[itemsArray])
            {
                validateItem(name, item, errors);
            }
        }

        errors validateEnums(json const & node) noexcept
        {
            errors errors;

            auto hasEnums = required(
                node,
                "",
                enumsField,
                "'{field}' root node is missing",
                requirements{ requirement{
                    &json::is_array,
                    "'{field}' should contain an array of enum descriptions",
                } },
                errors);

            if (!hasEnums)
            {
                return errors;
            }

            for (auto const & description : node[enumsField])
            {
                validateEnum(description, errors);
            }

            return errors;
        }

        path resolveHeaderFile(
            path const & root, json const & inputData, json const & description, std::string_view name)
        {
            auto directory = [&]() {
                if (inputData.contains(headerRootField))
                {
                    return root / inputData[headerRootField].get<std::string>()
                           / description[headerPathField].get<std::string>();
                }

                return root / description[headerPathField].get<std::string>();
            }();

            directory = absolute(std::move(directory)).make_preferred();

            if (!exists(directory))
            {
                create_directories(directory);
            }

            return directory / fmt::format("{}.h", name);
        }

        path resolveCodeFile(path const & root, json const & inputData, json const & description, std::string_view name)
        {
            auto directory = [&]() {
                if (inputData.contains(codeRootField))
                {
                    return root / inputData[codeRootField].get<std::string>()
                           / description[codePathField].get<std::string>();
                }

                return root / description[codePathField].get<std::string>();
            }();

            directory = absolute(std::move(directory)).make_preferred();

            if (!exists(directory))
            {
                create_directories(directory);
            }

            return directory / fmt::format("{}.cpp", name);
        }

        bool generateEnum(
            path const & root,
            inja::Environment & env,
            json const & inputData,
            json const & description,
            Config const & config)
        {
            auto logger = spdlog::get("logger");

            auto name = description[nameField].get<std::string_view>();
            auto headerFile = resolveHeaderFile(root, inputData, description, name);
            auto codeFile = resolveCodeFile(root, inputData, description, name);

            json renderData;
            renderData["enum"] = description;

            try
            {
                logger->info("Generating header for {}", name);
                logger->debug("    from: {}", config.enumConfig.headerTemplateFile);
                logger->debug("    to:   {}", headerFile);

                env.write(config.enumConfig.headerTemplateFile.string(), renderData, headerFile.string());

                logger->info("Generating code for {}", name);
                logger->debug("   from: {}", config.enumConfig.codeTemplateFile);
                logger->debug("   to:   {}", codeFile);

                env.write(config.enumConfig.codeTemplateFile.string(), renderData, codeFile.string());

                return true;
            }
            catch (std::exception const & ex)
            {
                logger->error("Error generating enums: {}", ex.what());
                logger->trace("Cleaning header {}", headerFile);
                logger->trace("Cleaning code {}", codeFile);

                remove(headerFile);
                remove(codeFile);

                return false;
            }
        }

        path resolveInputFile(std::string_view inputFile)
        {
            auto result = absolute(path(inputFile)).make_preferred();

            if (!exists(result))
            {
                throw std::runtime_error(fmt::format("Input file '{}' does not exist", result));
            }

            return result;
        }

        path resolveConfigFile(std::string_view configFile)
        {
            auto result = absolute(path(configFile)).make_preferred();

            if (!exists(result))
            {
                throw std::runtime_error(fmt::format("Config file '{}' does not exist", result));
            }

            return result;
        }

        path resolveOutputPath(std::string_view outputPath)
        {
            auto result = absolute(path(outputPath)).make_preferred();

            if (!exists(result))
            {
                create_directories(result);
            }

            return result;
        }

    }  // namespace

    bool generateEnums(path const & inputFile, std::filesystem::path const & outputRoot, Config const & config) noexcept
    {
        auto logger = spdlog::get("logger");

        logger->debug("Input file:          {}", inputFile);
        logger->debug("Config file:         {}", config.configFile);
        logger->debug("Templates directory: {}", config.templatesDirectory);
        logger->debug("Output directory:    {}", outputRoot);

        logger->info("Initialize inja templating engine");
        auto env = inja::Environment();
        env.set_trim_blocks(true);

        logger->info("Loading input data from: {}", inputFile);
        auto inputData = env.load_json(inputFile.string());

        auto validationResults = validateEnums(inputData);
        if (!validationResults.empty())
        {
            logger->error("Errors while validating enums input");

            for (auto const & message : validationResults)
            {
                logger->debug("    {}", message);
            }

            return false;
        }

        for (auto const & description : inputData[enumsField])
        {
            generateEnum(outputRoot, env, inputData, description, config);
        }

        logger->info("Templates generated");

        return true;
    }

    bool generateEnums(std::string_view inputFile, std::string_view configFile, std::string_view outputPath) noexcept
    {
        auto output = resolveOutputPath(outputPath);

        initLogging(output);

        auto logger = spdlog::get("logger");

        logger->info("Running enumgen generate");

        auto input = resolveInputFile(inputFile);
        auto configPath = resolveConfigFile(configFile);

        auto config = tryReadConfig(configPath);
        if (!config || !validateConfig(*config))
        {
            logger->error("Invalid config");
            return false;
        }


        return generateEnums(input, output, *config);
    }

}  // namespace enumgen