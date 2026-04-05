#ifndef __RTL_TYPE_TRAITS_H
#define __RTL_TYPE_TRAITS_H
#include <type_traits>

namespace rtl
{
    // C++ 17 std::void_t
    // https://en.cppreference.com/w/cpp/types/void_t.html
    template <typename ...>
    using void_t = void;

    // C++ 17 std::is_array_v
    // https://en.cppreference.com/w/cpp/types/is_array.html
    template <typename T>
    constexpr bool is_array_v = std::is_array<T>::value;

    // C++ 17 std::is_object_v
    // https://en.cppreference.com/w/cpp/types/is_object.html
    template <typename T>
    constexpr bool is_object_v = std::is_object<T>::value;

    // C++ 17 std::is_same_v
    // https://en.cppreference.com/w/cpp/types/is_same.html
    template <typename T, typename U>
    constexpr bool is_same_v = std::is_same<T, U>::value;

    // C++ 17 std::is_const_v
    // https://en.cppreference.com/w/cpp/types/is_const.html
    template <typename T>
    constexpr bool is_const_v = std::is_const<T>::value;

    // C++ 17 std::is_volatile_v
    // https://en.cppreference.com/w/cpp/types/is_volatile.html
    template <typename T>
    constexpr bool is_volatile_v = std::is_volatile<T>::value;

    template <typename T>
    struct remove_cvref
    {
        using type = std::remove_cv_t<std::remove_reference_t<T>>;
    };

    // C++ 20 std::remove_cvref_t
    // https://en.cppreference.com/w/cpp/types/remove_cvref.html
    template <typename T>
    using remove_cvref_t = typename remove_cvref<T>::type;

    // C++ 17 std::is_constructible_v
    // https://en.cppreference.com/w/cpp/types/is_constructible.html
    template <typename T, typename ... Args>
    constexpr bool is_constructible_v = std::is_constructible<T, Args...>::value;

    template <typename T>
    constexpr bool is_copy_assignable_v = std::is_copy_assignable<T>::value;

    template <typename T>
    constexpr bool is_trivially_copy_assignable_v = std::is_trivially_copy_assignable<T>::value;

    template <typename T>
    constexpr bool is_move_assignable_v = std::is_move_assignable<T>::value;

    template <typename T>
    constexpr bool is_trivially_move_assignable_v = std::is_trivially_move_assignable<T>::value;

    // C++ 17 std::is_constructible_v
    // https://en.cppreference.com/w/cpp/types/is_constructible.html
    template <typename T, typename ... Args>
    constexpr bool is_nothrow_constructible_v = std::is_nothrow_constructible<T, Args...>::value;

    // C++ 17 std::is_move_constructible_v
    // https://en.cppreference.com/w/cpp/types/is_move_constructible.html
    template <typename T>
    constexpr bool is_nothrow_move_constructible_v = std::is_nothrow_move_constructible<T>::value;

    template <typename T>
    constexpr bool is_nothrow_move_assignable_v = std::is_nothrow_move_assignable<T>::value;

    template <typename T>
    constexpr bool is_move_constructible_v = std::is_move_constructible<T>::value;

    template <typename T, typename ... Args>
    constexpr bool is_trivially_constructible_v = std::is_trivially_constructible<T, Args...>::value;

    template <typename T>
    constexpr bool is_trivially_move_constructible_v = std::is_trivially_move_constructible<T>::value;

    template <typename T, typename Arg>
    constexpr bool is_trivially_assignable_v = std::is_trivially_assignable<T, Arg>::value;

    template <typename T>
    constexpr bool is_trivially_destructible_v = std::is_trivially_destructible<T>::value;

    template <typename T, typename Arg>
    constexpr bool is_assignable_v = std::is_assignable<T, Arg>::value;

    template <typename T>
    constexpr bool is_reference_v = std::is_reference<T>::value;

    template <typename T>
    constexpr bool is_function_v = std::is_function<T>::value;

    template <typename T>
    constexpr bool is_default_constructible_v = std::is_default_constructible<T>::value;

    template <typename T>
    constexpr bool is_nothrow_default_constructible_v = std::is_nothrow_default_constructible<T>::value;

    template <typename T>
    constexpr bool is_copy_constructible_v = std::is_copy_constructible<T>::value;

    template <typename T>
    constexpr bool is_trivially_copy_constructible_v = std::is_trivially_copy_constructible<T>::value;

    template <typename T>
    constexpr bool is_nothrow_copy_constructible_v = std::is_nothrow_copy_constructible<T>::value;

    template <bool B>
    using bool_constant = std::integral_constant<bool, B>;

    template <typename T>
    constexpr bool is_bounded_array_v = false;

    template <typename T, size_t Size>
    constexpr bool is_bounded_array_v<T[Size]> = true;

    template <typename T>
    constexpr bool is_unbounded_array_v = false;

    template <typename T>
    constexpr bool is_unbounded_array_v<T[]> = true;

    template <typename T>
    struct is_bounded_array : public bool_constant<is_bounded_array_v<T>> {};

    template <typename T>
    struct is_unbounded_array : public bool_constant<is_unbounded_array_v<T>> {};
} // namespace rtl

#endif // __RTL_TYPE_TRAITS_H
