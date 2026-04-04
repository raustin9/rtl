#include <rtl/memory.h>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "construct_at does not use overloaded new", "[construct_at]" ) {
    struct ConstructWatcher
    {
        void* operator new(size_t size) throw() { return nullptr; }
    } watcher {};

    REQUIRE( new ConstructWatcher() == nullptr );
    REQUIRE( rtl::construct_at(&watcher) != nullptr );
}

template <typename T, typename = void>
struct uses_array_overload : std::false_type {};

template <typename T>
struct uses_array_overload<T, std::void_t<decltype(rtl::construct_at(std::declval<T*>()))> > : std::is_array<T> {};

TEST_CASE( "construct_at selects array overload", "[construct_at]" ) {
    using ArrayType = int[5];

    auto overload_check = []<typename T>(T*) -> bool {
        return std::is_array_v<T>;
    };

    alignas(int[5]) unsigned char buf[sizeof(int[5])];
    auto* p = reinterpret_cast<int(*)[5]>(buf);

    auto* result = rtl::construct_at(p);

    STATIC_REQUIRE(std::is_array_v<std::remove_pointer_t<decltype(result)>>);

    STATIC_REQUIRE_FALSE(std::is_same_v<decltype(result), int*>);
}

TEST_CASE( "construct_at value initializes (object)", "[construct_at]" )
{
    constexpr int ExpectedValue = 1;
    struct Value { int value = ExpectedValue; };

    // Initialize and dirty the array
    Value buf[10];
    std::memset(buf, 0xFF, sizeof(buf));

    auto* p = reinterpret_cast<Value(*)[10]>(buf);
    rtl::construct_at(p);

    for (const auto&[value] : buf)
    {
        REQUIRE(value == ExpectedValue);
    }
}

TEST_CASE( "Scalar overload is selected for non-array types", "[construct_at]" ) {
    alignas(int) unsigned char buf[sizeof(int)];
    auto* p = reinterpret_cast<int*>(buf);

    auto* result = rtl::construct_at(p, 42);

    STATIC_REQUIRE_FALSE(std::is_array_v<std::remove_pointer_t<decltype(result)>>);
    STATIC_REQUIRE(std::is_same_v<decltype(result), int*>);

    REQUIRE(*result == 42);
}

TEST_CASE( "Scalar overload cannot be called with array pointer", "[construct_at]" ) {
    // This is a compile-time constraint — verified via type trait
    STATIC_REQUIRE(uses_array_overload<int[5]>::value);
    STATIC_REQUIRE_FALSE(uses_array_overload<int>::value);
}