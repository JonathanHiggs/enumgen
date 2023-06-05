#pragma once

#include <concepts>
#include <type_traits>


namespace enumgen::utils
{

    /// <summary>
    /// Matches given a pointer to member value
    /// </summary>
    template <auto Ptr>
    concept member_value = std::is_member_object_pointer_v<decltype(Ptr)>;

    /// <summary>
    /// Matches given a pointer to member type
    /// </summary>
    template <typename TPtr>
    concept member_value_type = std::is_member_object_pointer_v<TPtr>;

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

    /// <summary>
    /// Extracts the value type from a pointer to member value
    /// </summary>
    template <auto Ptr>
        requires member_value<Ptr>
    using member_value_t = decltype(detail::member_value_type_deducer(Ptr));

    /// <summary>
    /// Extracts the value type from a pointer to member type
    /// </summary>
    template <typename TPtr>
        requires member_value_type<TPtr>
    using member_value_type_t = decltype(detail::member_value_type_deducer(std::declval<TPtr>()));

    /// <summary>
    /// Extracts the class type from a pointer to member value
    /// </summary>
    template <auto Ptr>
        requires member_value<Ptr>
    using member_value_class_t = decltype(detail::member_value_class_deducer(Ptr));

    /// <summary>
    /// Extracts the class type from a pointer to member type
    /// </summary>
    template <typename TPtr>
        requires member_value_type<TPtr>
    using member_value_type_class_t = decltype(detail::member_value_class_deducer(std::declval<TPtr>()));

    /// <summary>
    /// Matches when the pointer value is a pointer to member value of Class
    /// </summary>
    template <typename Class, auto Ptr>
    concept member_value_for = member_value<Ptr> && std::same_as<member_value_class_t<Ptr>, Class>;

    /// <summary>
    /// Matches when the pointer type is a pointer to member value of Class
    /// </summary>
    template <typename Class, typename TPtr>
    concept member_value_type_for = member_value_type<TPtr> && std::same_as<member_value_type_class_t<TPtr>, Class>;


    /// <summary>
    /// Matches given a pointer to member function value
    /// </summary>
    template <auto Ptr>
    concept member_function = std::is_member_function_pointer_v<decltype(Ptr)>;

    /// <summary>
    /// Matches given a pointer to member function type
    /// </summary>
    template <typename TPtr>
    concept member_function_type = std::is_member_function_pointer_v<TPtr>;

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

    /// <summary>
    /// Extracts the return type from a pointer to member function value
    /// </summary>
    template <auto Ptr>
        requires member_function<Ptr>
    using member_function_return_t = decltype(detail::member_function_return_deducer(Ptr));

    /// <summary>
    /// Extracts the return type from a pointer to member function type
    /// </summary>
    template <typename TPtr>
        requires member_function_type<TPtr>
    using member_function_type_return_t = decltype(detail::member_function_return_deducer(std::declval<TPtr>()));

    /// <summary>
    /// Extracts the class type from a pointer to member function value
    /// </summary>
    template <auto Ptr>
        requires member_function<Ptr>
    using member_function_class_t = decltype(detail::member_function_class_deducer(Ptr));

    /// <summary>
    /// Extracts the class type from a pointer to member function type
    /// </summary>
    template <typename TPtr>
        requires member_function_type<TPtr>
    using member_function_type_class_t = decltype(detail::member_function_class_deducer(std::declval<TPtr>()));

    /// <summary>
    /// Matches when the pointer value is a pointer to member function of Class
    /// </summary>
    template <typename Class, auto Ptr>
    concept member_function_for = member_function<Ptr> && std::same_as<member_function_class_t<Ptr>, Class>;

    /// <summary>
    /// Matches when the pointer type is a pointer to member function of Class
    /// </summary>
    template <typename Class, typename TPtr>
    concept member_function_type_for
        = member_function_type<TPtr> && std::same_as<member_function_type_class_t<TPtr>, Class>;

    // clang-format off
    /// <summary>
    /// Matches when the pointer value is a pointer to member function that takes the given arguments
    /// </summary>
    template <auto Ptr, typename... Args>
    concept member_function_args =
        member_function<Ptr>
        && requires(member_function_class_t<Ptr> && t, decltype(Ptr) ptr, Args &&... args) {
            { (t.*ptr)(std::forward<Args>(args)...) } -> std::same_as<member_function_return_t<Ptr>>;
        };

    /// <summary>
    /// Matches when the pointer type is a pointer to member function that takes the given arguments
    /// </summary>
    template <typename TPtr, typename... Args>
    concept member_function_type_args =
        member_function_type<TPtr>
        && requires(member_function_type_class_t<TPtr> && t, TPtr ptr, Args &&... args) {
            { (t.*ptr)(std::forward<Args>(args)...) } -> std::same_as<member_function_type_return_t<TPtr>>;
        };
    // clang-format on

}  // namespace enumgen::utils
