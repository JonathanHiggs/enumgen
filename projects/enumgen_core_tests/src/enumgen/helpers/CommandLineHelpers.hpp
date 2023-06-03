#pragma once

#include <memory>
#include <string_view>
#include <vector>


namespace enumgen::helpers
{

    struct ArgsHelper final
    {
        int argc;
        char ** argv;
        std::unique_ptr<char *[]> holder;

        explicit ArgsHelper(std::initializer_list<std::string_view> strings) noexcept;
    };

    ArgsHelper createArgs(std::initializer_list<std::string_view> strings) noexcept;

}  // namespace enumgen::helpers