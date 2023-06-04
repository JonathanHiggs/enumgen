#pragma once

#include <concepts>
#include <type_traits>


namespace enumgen::utils
{

    template <typename MV>
    concept member_value = std::is_member_object_pointer_v<MV>;

    namespace detail
    {

        template <typename R, typename C>
        C member_value_class_deducer(R C::*);

        template <typename R, typename C>
        C member_value_class_deducer(R C::*const);

        template <typename R, typename C>
        R member_value_type_deducer(R C::*);

        template <typename R, typename C>
        R member_value_type_deducer(R C::*const);

    }  // namespace detail

    template <auto MV>
        requires member_value<decltype(MV)>
    using member_value_class_t = decltype(detail::member_value_class_deducer(MV));

    template <auto MV>
        requires member_value<decltype(MV)>
    using member_value_t = decltype(detail::member_value_type_deducer(MV));


    template <typename Class, typename MV>
    concept member_value_for = member_value<MV> && std::same_as<member_value_class_t<std::declval<MV>()>, Class>;


    template <typename MF>
    concept member_function = std::is_member_function_pointer_v<MF>;

    template <typename T, typename MF, typename... Args>
    concept member_function_args = requires(T && t, MF mf, Args &&... args) { (t.*mf)(std::forward<Args>(args)...); };

    namespace detail
    {

        template <typename R, typename C, typename... Args>
        C member_function_class_deducer(R (C::*)(Args...));

        template <typename R, typename C, typename... Args>
        C member_function_class_deducer(R (C::*)(Args...) const);

        template <typename R, typename C, typename... Args>
        R member_function_return_deducer(R (C::*)(Args...));

        template <typename R, typename C, typename... Args>
        R member_function_return_deducer(R (C::*)(Args...) const);

    }  // namespace detail


    template <auto MF>
    using member_function_class_t = decltype(detail::member_function_class_deducer(MF));

    template <auto MF>
    using member_function_return_t = decltype(detail::member_function_return_deducer(MF));

}  // namespace enumgen::utils
