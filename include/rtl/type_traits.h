#ifndef __RTL_TYPE_TRAITS_H
#define __RTL_TYPE_TRAITS_H
#include <iterator>
#include <type_traits>

namespace rtl
{
    // C++ 17 std::void_t
    // https://en.cppreference.com/w/cpp/types/void_t.html
    template <typename...>
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
    template <typename T, typename... Args>
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
    template <typename T, typename... Args>
    constexpr bool is_nothrow_constructible_v = std::is_nothrow_constructible<T, Args...>::value;

    // C++ 17 std::is_move_constructible_v
    // https://en.cppreference.com/w/cpp/types/is_move_constructible.html
    template <typename T>
    constexpr bool is_nothrow_move_constructible_v = std::is_nothrow_move_constructible<T>::value;

    template <typename T>
    constexpr bool is_nothrow_move_assignable_v = std::is_nothrow_move_assignable<T>::value;

    template <typename T>
    constexpr bool is_move_constructible_v = std::is_move_constructible<T>::value;

    template <typename T, typename... Args>
    constexpr bool is_trivially_constructible_v
        = std::is_trivially_constructible<T, Args...>::value;

    template <typename T>
    constexpr bool is_trivially_move_constructible_v
        = std::is_trivially_move_constructible<T>::value;

    template <typename T, typename Arg>
    constexpr bool is_trivially_assignable_v = std::is_trivially_assignable<T, Arg>::value;

    template <typename T>
    constexpr bool is_trivially_default_constructible_v
        = std::is_trivially_default_constructible<T>::value;

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
    constexpr bool is_nothrow_default_constructible_v
        = std::is_nothrow_default_constructible<T>::value;

    template <typename T>
    constexpr bool is_copy_constructible_v = std::is_copy_constructible<T>::value;

    template <typename T>
    constexpr bool is_trivially_copy_constructible_v
        = std::is_trivially_copy_constructible<T>::value;

    template <typename T>
    constexpr bool is_nothrow_copy_constructible_v = std::is_nothrow_copy_constructible<T>::value;

    template <typename T>
    constexpr bool is_trivially_copyable_v = std::is_trivially_copyable<T>::value;

    template <bool B>
    using bool_constant = std::integral_constant<bool, B>;

    template <typename T>
    constexpr bool is_bounded_array_v = false;

    template <typename T, std::size_t Size>
    constexpr bool is_bounded_array_v<T[Size]> = true;

    template <typename T>
    constexpr bool is_unbounded_array_v = false;

    template <typename T>
    constexpr bool is_unbounded_array_v<T[]> = true;

    template <typename T>
    struct is_bounded_array : public bool_constant<is_bounded_array_v<T>>
    {
    };

    template <typename T>
    struct is_unbounded_array : public bool_constant<is_unbounded_array_v<T>>
    {
    };

    template <typename T>
    constexpr bool is_standard_layout_v = std::is_standard_layout<T>::value;

    namespace __detail
    {
        template <typename From, typename To,
                  bool = std::is_void<From>::value || std::is_function<To>::value || is_array_v<To>>
        class __is_nothrow_convertible_helper : std::is_void<To>
        {
        };

        template <typename From, typename To>
        class __is_nothrow_convertible_helper<From, To, false>
        {
            template <typename To1>
            static void __test_aux(To1) noexcept;

            template <typename From1, typename To1>
            static bool_constant<noexcept(__test_aux<To1>(std::declval<From1>()))> __test(int);

            template <typename, typename>
            static std::false_type __test(...);

          public:
            using type = decltype(__test<From, To>(0));
        };

        template <typename To, typename From>
        using __is_array_convertible = std::is_convertible<From (*)[], To (*)[]>;

        // contiguous iterator
        template <typename T, typename = void>
        struct is_contiguous_iterator : std::false_type
        {
        };

        // pointers are contiguous iterators
        template <typename T>
        struct is_contiguous_iterator<T *> : std::true_type
        {
        };

        // any iterator whose -> operator returns a pointer
        template <typename T>
        struct is_contiguous_iterator<T, typename std::enable_if<std::is_pointer<
                                             decltype(std::declval<T>().operator->)>::value>::type>
            : std::true_type
        {
        };

        template <typename T>
        using iter_refrence_t = typename std::iterator_traits<T>::reference;
    }

    template <typename From, typename To>
    struct is_nothrow_convertible : public __detail::__is_nothrow_convertible_helper<From, To>
    {
    };

    template <typename From, typename To>
    constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<From, To>::value;

    template <typename From, typename To>
    constexpr bool is_convertible_v = std::is_convertible<From, To>::value;

    template <typename T>
    struct type_identity
    {
        using type = T;
    };

    template <typename T>
    using type_identity_t = typename type_identity<T>::type;
} // namespace rtl

#endif // __RTL_TYPE_TRAITS_H
