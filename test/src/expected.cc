#include <iostream>
#include <rtl/expected.h>
#include <rtl/utility.h>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "bad_expected_access exception", "[expected]" )
{
    auto throw_except = [] {
        throw rtl::bad_expected_access<int>(1);
    };

    REQUIRE_THROWS_AS(throw_except(), rtl::bad_expected_access<int>);
}

TEST_CASE( "rtl::unexpected value construction", "[expected]" )
{
    rtl::unexpected<int> unexpect { 1 };
    REQUIRE(unexpect.error() == 1);
}

TEST_CASE( "rtl::unexpected error()", "[expected]" )
{
    constexpr int Expected = 1;
    auto func = [=]() -> rtl::unexpected<int> {
        return rtl::unexpected<int>(Expected);
    };

    REQUIRE(func().error() == Expected);
}

TEST_CASE( "rtl::unexpected && error()", "[expected]" )
{
    constexpr int Expected = 1;
    auto func = [=]() -> rtl::unexpected<int> {
        return rtl::unexpected<int>(Expected);
    };

    REQUIRE(std::move(func()).error() == Expected);
}

TEST_CASE( "rtl::unexpected comparison", "[expected]" )
{
    constexpr int Expected = 1;
    auto func = [=]() -> rtl::unexpected<int> {
        return rtl::unexpected<int>(Expected);
    };

    auto unexpect = rtl::unexpected<int>(Expected);

    REQUIRE(func() == unexpect);
}

TEST_CASE( "rtl::unexpected in_place with args", "[expected]" )
{
 const std::string expected_result( 3, 'x' );
 rtl::unexpected<std::string> unexpect { rtl::in_place, 3, 'x' };
 REQUIRE(unexpect.error() == expected_result);
}

TEST_CASE( "rtl::unexpected in_place with initializer list", "[expected]" )
{
    std::vector<int> expected_result = { 1, 2, 3, 4, 5 };
    rtl::unexpected<std::vector<int>> unexpect { rtl::in_place, { 1, 2, 3, 4, 5 }};

    REQUIRE(unexpect.error() == expected_result);
}

TEST_CASE( "rtl::unexpected in_place with initializer list + extra args", "[expected]" )
{
    std::vector<int> expected_result = { 1, 2, 3, 4, 5 };
    rtl::unexpected<std::vector<int>> unexpect { rtl::in_place, { 1, 2, 3, 4, 5 }, std::allocator<int>{} };

    REQUIRE(unexpect.error() == expected_result);
}

TEST_CASE( "rtl::expected default construction int", "[expected]" )
{
    rtl::expected<int, char> test;
}

TEST_CASE( "rtl::expected default construction with custom object", "[expected]" )
{
    class MyObject
    {
    public:
        MyObject() = default;
    };

    rtl::expected<MyObject, char> test;
}
TEST_CASE( "rtl::expected conversion construction from rvalue", "[expected]" )
{
    class MyObject
    {
    public:
        MyObject() = default;
    };

    rtl::expected<MyObject, char> test = MyObject();
}

TEST_CASE( "rtl::expected construct from rtl::unexpected", "[expected]" )
{
    rtl::unexpected<int> unexpect { 1 };
    rtl::expected<char, int> test = unexpect;

    REQUIRE(unexpect.error() == 1);
}

TEST_CASE( "rtl::expected construct from object-based rtl::unexpected", "[expected]" )
{
    std::vector<int> expected_result = { 1, 2, 3, 4, 5 };
    rtl::unexpected<std::vector<int>> unexpect { rtl::in_place, { 1, 2, 3, 4, 5 }};

    rtl::expected<int, std::vector<int>> test = unexpect;
}

TEST_CASE( "rtl::expected construct from object-based rtl::unexpected r-value", "[expected]" )
{
    auto return_unexpected = [] {
        std::vector<int> expected_result = { 1, 2, 3, 4, 5 };
        rtl::unexpected<std::vector<int>> unexpect { rtl::in_place, { 1, 2, 3, 4, 5 }};
        return unexpect;
    };

    rtl::expected<int, std::vector<int>> test = return_unexpected();
}

TEST_CASE( "rtl::expected copy assign", "[expected]" )
{
    rtl::expected<int, std::vector<int>> exp_1 = 1;
    rtl::expected<int, std::vector<int>> exp_2 = 2;

    exp_2 = exp_1;
}

TEST_CASE( "rtl::expected move assign", "[expected]" )
{
    rtl::expected<int, std::vector<int>> exp_1 = 1;
    rtl::expected<int, std::vector<int>> exp_2 = 2;

    exp_2 = std::move(exp_1);
}
