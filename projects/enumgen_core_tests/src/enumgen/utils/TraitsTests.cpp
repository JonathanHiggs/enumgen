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
        static_assert(member_value<&Foo::value>);
        static_assert(!member_value<&Foo::function>);

        EXPECT_TRUE((member_value<&Foo::value>));
        EXPECT_FALSE((member_value<&Foo::function>));
    }

    TEST(TraitsTests, MemberValueType_IsCorrect)
    {
        static_assert(member_value_type<decltype(&Foo::value)>);
        static_assert(!member_value_type<decltype(&Foo::function)>);

        EXPECT_TRUE((member_value_type<decltype(&Foo::value)>));
        EXPECT_FALSE((member_value_type<decltype(&Foo::function)>));
    }

    TEST(TraitsTests, MemberValueTypeT_IsCorrect)
    {
        static_assert(std::same_as<member_value_t<&Foo::value>, int>);
        EXPECT_TRUE((std::same_as<member_value_t<&Foo::value>, int>));
    }

    TEST(TraitsTests, MemberValueTypeType_IsCorrect)
    {
        static_assert(std::same_as<member_value_type_t<decltype(&Foo::value)>, int>);
        EXPECT_TRUE((std::same_as<member_value_type_t<decltype(&Foo::value)>, int>));
    }

    TEST(TraitsTests, MemberValueClass_IsCorrect)
    {
        static_assert(std::same_as<member_value_class_t<&Foo::value>, Foo>);
        EXPECT_TRUE((std::same_as<member_value_class_t<&Foo::value>, Foo>));
    }

    TEST(TraitsTests, MemberValueTypeClass_IsCorrect)
    {
        static_assert(std::same_as<member_value_type_class_t<decltype(&Foo::value)>, Foo>);
        EXPECT_TRUE((std::same_as<member_value_type_class_t<decltype(&Foo::value)>, Foo>));
    }

    TEST(TraitsTests, MemberValueFor_IsCorrect)
    {
        static_assert(member_value_for<Foo, &Foo::value>);
        EXPECT_TRUE((member_value_for<Foo, &Foo::value>));
    }

    TEST(TraitsTests, MemberValueTypeFor_IsCorrect)
    {
        static_assert(member_value_type_for<Foo, decltype(&Foo::value)>);
        EXPECT_TRUE((member_value_type_for<Foo, decltype(&Foo::value)>));
    }

    TEST(TraitsTests, MemberFunction_IsCorrect)
    {
        static_assert(member_function<&Foo::function>);
        static_assert(!member_function<&Foo::value>);

        EXPECT_TRUE((member_function<&Foo::function>));
        EXPECT_FALSE((member_function<&Foo::value>));
    }

    TEST(TraitsTests, MemberFunctionType_IsCorrect)
    {
        static_assert(member_function_type<decltype(&Foo::function)>);
        static_assert(!member_function_type<decltype(&Foo::value)>);

        EXPECT_TRUE((member_function_type<decltype(&Foo::function)>));
        EXPECT_FALSE((member_function_type<decltype(&Foo::value)>));
    }

    TEST(TraitsTests, MemberFunctionReturn_IsCorrect)
    {
        static_assert(std::same_as<member_function_return_t<&Foo::function>, int>);
        EXPECT_TRUE((std::same_as<member_function_return_t<&Foo::function>, int>));
    }

    TEST(TraitsTests, MemberFunctionTypeReturn_IsCorrect)
    {
        static_assert(std::same_as<member_function_type_return_t<decltype(&Foo::function)>, int>);
        EXPECT_TRUE((std::same_as<member_function_type_return_t<decltype(&Foo::function)>, int>));
    }

    TEST(TraitsTests, MemberFunctionClass_IsCorrect)
    {
        static_assert(std::same_as<member_function_class_t<&Foo::function>, Foo>);
        EXPECT_TRUE((std::same_as<member_function_class_t<&Foo::function>, Foo>));
    }

    TEST(TraitsTests, MemberFunctionTypeClass_IsCorrect)
    {
        static_assert(std::same_as<member_function_type_class_t<decltype(&Foo::function)>, Foo>);
        EXPECT_TRUE((std::same_as<member_function_type_class_t<decltype(&Foo::function)>, Foo>));
    }

    TEST(TraitsTests, MemberFunctionFor_IsCorrect)
    {
        static_assert(member_function_for<Foo, &Foo::function>);
        EXPECT_TRUE((member_function_for<Foo, &Foo::function>));
    }

    TEST(TraitsTests, MemberFunctionTypeFor_IsCorrect)
    {
        static_assert(member_function_type_for<Foo, decltype(&Foo::function)>);
        EXPECT_TRUE((member_function_type_for<Foo, decltype(&Foo::function)>));
    }

    TEST(TraitsTests, MemberFunctionArgs_IsCorrect)
    {
        static_assert(member_function_args<&Foo::function, int>);
        EXPECT_TRUE((member_function_args<&Foo::function, int>));
    }

    TEST(TraitsTests, MemberFunctionTypeArgs_IsCorrect)
    {
        static_assert(member_function_type_args<decltype(&Foo::function), int>);
        EXPECT_TRUE((member_function_type_args<decltype(&Foo::function), int>));
    }

}  // namespace enumgen::utils