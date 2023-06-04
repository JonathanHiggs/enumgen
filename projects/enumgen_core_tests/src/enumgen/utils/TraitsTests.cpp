#include <enumgen/utils/Traits.hpp>

#include <gtest/gtest.h>


namespace enumgen::utils
{
    namespace
    {

        struct Foo
        {
            int value;

            int function(int a) const
            {
                return value + a;
            }
        };

    }  // namespace


    TEST(TraitsTests, MemberValue_IsCorrect)
    {
        static_assert(member_value<decltype(&Foo::value)>);
        static_assert(!member_value<decltype(&Foo::function)>);

        EXPECT_TRUE((member_value<decltype(&Foo::value)>));
        EXPECT_FALSE((member_value<decltype(&Foo::function)>));
    }

    TEST(TraitsTests, MemberValueClass_IsCorrect)
    {
        static_assert(std::same_as<member_value_class_t<&Foo::value>, Foo>);
        EXPECT_TRUE((std::same_as<member_value_class_t<&Foo::value>, Foo>));
    }

    TEST(TraitsTests, MemberValueType_IsCorrect)
    {
        static_assert(std::same_as<member_value_t<&Foo::value>, int>);
        EXPECT_TRUE((std::same_as<member_value_t<&Foo::value>, int>));
    }

    TEST(TraitsTests, MemberFunction_IsCorrect)
    {
        static_assert(member_function<decltype(&Foo::function)>);
        static_assert(!member_function<decltype(&Foo::value)>);

        EXPECT_TRUE((member_function<decltype(&Foo::function)>));
        EXPECT_FALSE((member_function<decltype(&Foo::value)>));
    }

    TEST(TraitsTests, MemberFunctionClass_IsCorrect)
    {
        static_assert(std::same_as<member_function_class_t<&Foo::function>, Foo>);
        EXPECT_TRUE((std::same_as<member_function_class_t<&Foo::function>, Foo>));
    }

    TEST(TraitsTests, MemberFunctionReturn_IsCorrect)
    {
        static_assert(std::same_as<member_function_return_t<&Foo::function>, int>);
        EXPECT_TRUE((std::same_as<member_function_return_t<&Foo::function>, int>));
    }

}  // namespace enumgen::utils