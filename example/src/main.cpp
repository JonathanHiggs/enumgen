#include <example/Verbosity.h>

#include <format>
#include <iostream>
#include <string_view>
#include <vector>


using namespace example;
using namespace std::string_view_literals;


int main()
{
    std::cout << "Verbosity Values:\n";

    for (auto const & verbosity : Verbosity::Values())
    {
        std::cout << std::format("  {}::{} = {}\n", Verbosity::Name(), verbosity.ToString(), verbosity.ToUnderlying());
    }


    std::cout << "\n\nParse strings:\n";

    auto strings = std::vector{ "QUITE"sv, "Minimal"sv, "normal"sv, "Detailed"sv, "Diagnostic"sv, "not-a-verbosity"sv };

    for (auto str : strings)
    {
        auto parseResult = Verbosity::TryParse(str);
        if (parseResult)
        {
            std::cout << std::format("  {} parses to {}::{}\n", str, Verbosity::Name(), parseResult->ToString());
        }
        else
        {
            try
            {
                std::ignore = Verbosity::Parse(str);
            }
            catch (std::exception const & ex)
            {
                std::cout << std::format("  {} does not parse with exception:\n    {}\n", str, ex.what());
            }
        }
    }


    return 0;
}