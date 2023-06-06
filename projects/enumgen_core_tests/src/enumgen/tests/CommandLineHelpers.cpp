#include <enumgen/tests/CommandLineHelpers.hpp>


namespace enumgen::tests
{

    ArgsHelper::ArgsHelper(std::initializer_list<std::string_view> strings) noexcept
      : argc(static_cast<int>(strings.size()))
    {
        holder = std::unique_ptr<char *[]>(new char *[argc]);

        int index = 0;
        for (auto const & str : strings)
        {
            holder[index] = const_cast<char *>(str.data());
            ++index;
        }

        argv = holder.get();
    }

    ArgsHelper createArgs(std::initializer_list<std::string_view> strings) noexcept
    {
        return ArgsHelper(std::move(strings));
    }

}  // namespace enumgen::tests