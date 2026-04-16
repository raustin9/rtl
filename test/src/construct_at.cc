#include <catch2/catch_test_macros.hpp>
#include <rtl/memory.h>
#include <rtl/type_traits.h>

TEST_CASE ("construct_at does not use overloaded new", "[construct_at]")
{
    struct ConstructWatcher
    {
        void *
        operator new (size_t) noexcept
        {
            return nullptr;
        }
        void
        operator delete (void *) noexcept
        {
        }
    } watcher{};

    REQUIRE (new ConstructWatcher () == nullptr);
    REQUIRE (rtl::construct_at (&watcher) != nullptr);
}

template <typename T, typename = void>
struct uses_array_overload : std::false_type
{
};

template <typename T>
struct uses_array_overload<T, rtl::void_t<decltype (rtl::construct_at (std::declval<T *> ()))>>
    : std::is_array<T>
{
};

TEST_CASE ("construct_at selects array overload", "[construct_at]")
{
    using ArrayType = int[5];

    alignas (int[5]) unsigned char buf[sizeof (int[5])];
    auto *p = reinterpret_cast<int (*)[5]> (buf);

    auto *result = rtl::construct_at (p);

    STATIC_REQUIRE (rtl::is_array_v<std::remove_pointer_t<decltype (result)>>);

    STATIC_REQUIRE_FALSE (rtl::is_same_v<decltype (result), int *>);
}

TEST_CASE ("construct_at value initializes (object)", "[construct_at]")
{
    constexpr int ExpectedValue = 1;
    struct Value
    {
        int value = ExpectedValue;
    };

    // Initialize and dirty the array
    Value buf[10];
    std::memset (buf, 0xFF, sizeof (buf));

    auto *p = reinterpret_cast<Value (*)[10]> (buf);
    rtl::construct_at (p);

    for (const auto &value : buf)
        {
            REQUIRE (value.value == ExpectedValue);
        }
}

TEST_CASE ("Scalar overload is selected for non-array types", "[construct_at]")
{
    alignas (int) unsigned char buf[sizeof (int)];
    auto *p = reinterpret_cast<int *> (buf);

    auto *result = rtl::construct_at (p, 42);

    STATIC_REQUIRE_FALSE (rtl::is_array_v<std::remove_pointer_t<decltype (result)>>);
    STATIC_REQUIRE (rtl::is_same_v<decltype (result), int *>);

    REQUIRE (*result == 42);
}

TEST_CASE ("Scalar overload cannot be called with array pointer", "[construct_at]")
{
    // This is a compile-time constraint that is verified via type trait
    STATIC_REQUIRE (uses_array_overload<int[5]>::value);
    STATIC_REQUIRE_FALSE (uses_array_overload<int>::value);
}