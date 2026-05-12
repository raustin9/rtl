#include <catch2/catch_test_macros.hpp>
#include <rtl/ext/view.h>

#include <memory>

template <typename T>
void test_view(rtl::ext::view<T> view, T expected)
{
    REQUIRE(view == expected);
}

TEST_CASE("view", "[view]")
{
    int i = 10;
    std::string s = "test";
    auto unique = std::make_unique<int>(i);
    test_view<int>(i, i);
    test_view<std::string>(s, s);
    test_view<int>(unique, i);
}
