#include <codegen/EnumGenerator.h>

#include <fmt/core.h>
#include <fmt/printf.h>
#include <fmt/std.h>
#include <fmt/format.h>
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <functional>
#include <vector>


using namespace fmt::literals;
using namespace nlohmann;
using namespace std::filesystem;
using namespace std::string_view_literals;


namespace codegen
{
    namespace
    {

        static constexpr std::string_view enumsField = "enums"sv;
        static constexpr std::string_view nameField = "name"sv;
        static constexpr std::string_view descriptionField = "description"sv;
        static constexpr std::string_view outputPathField = "outputPath"sv;
        static constexpr std::string_view headerPathField = "headerPath"sv;
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
                errors.emplace_back(fmt::vformat(missingMessage, fmt::make_format_args("name"_a = name, "field"_a = field)));
                return false;
            }

            auto const & value = node[field];
            for (auto const & requirement : requirements)
            {
                if (!requirement.check(value))
                {
                    errors.emplace_back(fmt::vformat(requirement.errorMessage, fmt::make_format_args("name"_a = name, "field"_a = field)));
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
                    errors.emplace_back(fmt::vformat(requirement.errorMessage, fmt::make_format_args("name"_a = name, "field"_a = field)));
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
                requirements{ requirement{ &json::is_number_integer, "Enum item '{name}::{field}' has non-int value" } },
                errors);

            optional(
                node,
                name,
                itemAltsArray,
                requirements{
                    requirement{ &json::is_array, "Enum item '{name}::{field}' has non-array value" },
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

            constexpr auto requiredFields = std::array{ outputPathField, headerPathField, namespaceField };

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

        path prepareOutputDirectory(path const & root, json const & description)
        {
            auto result = absolute(root / description["outputPath"].get<std::string>()).make_preferred();
            if (!exists(result))
            {
                fmt::print("Creating output directory: {}\n", result);
                create_directories(result);
            }

            return result;
        }

        path prepareHeaderFile(path const & outputDirectory, std::string_view name)
        {
            return outputDirectory / fmt::format("{}.h", name);
        }

        path prepareCodeFile(path const & outputDirectory, std::string_view name)
        {
            return outputDirectory / fmt::format("{}.cpp", name);
        }

        bool generateEnum(path const & root, inja::Environment & env, json const & description, Config const & config)
        {
            auto name = description["name"].get<std::string_view>();
            auto outputDirectory = prepareOutputDirectory(root, description);
            auto headerFile = prepareHeaderFile(outputDirectory, name);
            auto codeFile = prepareCodeFile(outputDirectory, name);

            json renderData;
            renderData["enum"] = description;

            try
            {
                fmt::print(
                    "Generating header for {}\n    from: {}\n    to:   {}\n",
                    name,
                    config.enumConfig.headerTemplateFile,
                    headerFile);

                env.write(config.enumConfig.headerTemplateFile.string(), renderData, headerFile.string());

                fmt::print(
                    "Generating code for {}\n    from: {}\n    to:   {}\n",
                    name,
                    config.enumConfig.codeTemplateFile,
                    codeFile);

                env.write(config.enumConfig.codeTemplateFile.string(), renderData, codeFile.string());


                return true;
            }
            catch (std::exception const & ex)
            {
                fmt::print("Error generating enums:\n    {}\n", ex.what());

                remove(headerFile);
                remove(codeFile);

                return false;
            }
        }

    }  // namespace

    void generateEnums(path const & inputFile, Config const & config) noexcept
    {
        fmt::print("Input file:          {}\n", inputFile);
        fmt::print("Config file:         {}\n", config.configFile);
        fmt::print("Templates directory: {}\n", config.templatesDirectory);

        fmt::print("Initialize inja\n");
        auto env = inja::Environment();
        env.set_trim_blocks(true);

        fmt::print("Loading input data from:\n    {}\n", inputFile);
        auto inputData = env.load_json(inputFile.string());

        auto validationResults = validateEnums(inputData);
        if (!validationResults.empty())
        {
            fmt::print("Errors while validating enums input\n");
            for (auto const & error : validationResults)
            {
                fmt::print("    {}\n", error);
            }

            return;
        }

        auto root = inputFile.parent_path();
        for (auto const & description : inputData["enums"sv])
        {
            generateEnum(root, env, description, config);
        }

        fmt::print("Templates generated\n");
    }

}  // namespace codegen