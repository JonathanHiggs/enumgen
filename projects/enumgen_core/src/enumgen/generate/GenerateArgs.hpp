#pragma once


#include <optional>
#include <string_view>


namespace enumgen::generate
{

    struct GenerateArgs final
    {
        std::optional<std::string_view> specifications;
        std::optional<std::string_view> config;
        std::optional<std::string_view> binaryDirectory;
        std::optional<std::string_view> sourceDirectory;
    };

}  // namespace enumgen::generate