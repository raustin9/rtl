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
    REQUIRE(test.value() == 0);
}

TEST_CASE( "rtl::expected default construction with custom object", "[expected]" )
{
    class MyObject
    {
    public:
        MyObject() : value{ 10 } {}
        int value { 10 };
    };

    STATIC_REQUIRE(rtl::is_default_constructible_v<MyObject>);

    rtl::expected<MyObject, char> test{};
    REQUIRE(test.value().value == 10);
}
TEST_CASE( "rtl::expected conversion construction from rvalue", "[expected]" )
{
    class MyObject
    {
    public:
        MyObject() = default;
        int value = 10;
    };

    rtl::expected<MyObject, char> test = MyObject();
    REQUIRE(test.value().value == 10);
}

TEST_CASE( "rtl::expected construct from rtl::unexpected", "[expected]" )
{
    rtl::unexpected<int> unexpect { 1 };
    rtl::expected<char, int> test = unexpect;

    REQUIRE(unexpect.error() == 1);
}

TEST_CASE( "rtl::expected construct from object-based rtl::unexpected", "[expected]" )
{
    std::vector<int> expected_error = { 1, 2, 3, 4, 5 };
    rtl::unexpected<std::vector<int>> unexpect { rtl::in_place, { 1, 2, 3, 4, 5 }};

    rtl::expected<int, std::vector<int>> test = unexpect;
    REQUIRE(unexpect.error() == expected_error);
}

TEST_CASE( "rtl::expected construct from object-based rtl::unexpected r-value", "[expected]" )
{
    auto return_unexpected = [] {
        std::vector<int> expected_result = { 1, 2, 3, 4, 5 };
        rtl::unexpected<std::vector<int>> unexpect { rtl::in_place, { 1, 2, 3, 4, 5 }};
        return unexpect;
    };

    rtl::expected<int, std::vector<int>> test = return_unexpected();
    // TODO: check error()
}

TEST_CASE( "rtl::expected copy assign", "[expected]" )
{
    rtl::expected<int, std::vector<int>> exp_1 = 1;
    REQUIRE(exp_1.value() == 1);
    rtl::expected<int, std::vector<int>> exp_2 = 2;
    REQUIRE(exp_2.value() == 2);

    exp_2 = exp_1;
    REQUIRE(exp_2.value() == 1);
}

TEST_CASE( "rtl::expected move assign", "[expected]" )
{
    rtl::expected<int, std::vector<int>> exp_1 = 1;
    REQUIRE(exp_1.value() == 1);

    rtl::expected<int, std::vector<int>> exp_2 = 2;
    REQUIRE(exp_2.value() == 2);

    exp_2 = std::move(exp_1);

    REQUIRE(exp_2.value() == 1);
}

TEST_CASE( "rtl::expected from value assign", "[expected]" )
{
    int value = 10;
    rtl::expected<int, std::vector<int>> exp_1 = 1;
    exp_1 = value;

    REQUIRE(exp_1.value() == 10);
}

TEST_CASE( "rtl::expected from unexpected assign", "[expected]" )
{
    std::vector<int> expected_error = { 1, 2, 3, 4, 5 };
    rtl::unexpected<std::vector<int>> unexpect { rtl::in_place, { 1, 2, 3, 4, 5 }};
    rtl::expected<int, std::vector<int>> exp_1;
    exp_1 = unexpect;
    REQUIRE(exp_1.error() == expected_error);
}

TEST_CASE( "rtl::expected from r-value unexpected assign", "[expected]" )
{
    std::vector<int> expected_error = { 1, 2, 3, 4, 5 };
    rtl::unexpected<std::vector<int>> unexpect { rtl::in_place, { 1, 2, 3, 4, 5 }};
    rtl::expected<int, std::vector<int>> exp_1;
    exp_1 = std::move(unexpect);

    REQUIRE(exp_1.error() == expected_error);
}

TEST_CASE( "rtl::bad_expected_access from value()", "[expected]" )
{
    rtl::expected<int, char> exp = rtl::unexpected<char>{ rtl::in_place, 'c' };

    REQUIRE_THROWS_AS(exp.value(), rtl::bad_expected_access<char>);
}


TEST_CASE( "rtl::bad_expected_access exception error value", "[expected]" )
{
    rtl::expected<int, char> exp = rtl::unexpected<char>{ rtl::in_place, 'c' };

    try
    {
        exp.value();
    } catch (rtl::bad_expected_access<char>& e)
    {
        REQUIRE(e.error() == 'c');
    }
}

TEST_CASE( "rtl::bad_expected_access<std::string> exception error value", "[expected]" )
{
    std::string error_value { "something went wrong" };
    rtl::unexpected<std::string> unex( error_value );

    rtl::expected<int, std::string> exp = unex;

    try
    {
        exp.value();
    } catch (rtl::bad_expected_access<std::string>& e)
    {
        REQUIRE(e.error() == error_value);
    }
}

TEST_CASE( "rtl::expected::error() &&", "[expected]" )
{
    std::string expected_error { "something went wrong" };
    rtl::unexpected<std::string> unex( expected_error );
    rtl::expected<int, std::string> exp = unex;

    REQUIRE(std::move(exp).error() == expected_error);
}

TEST_CASE( "rtl::expected::operator *", "[expected]" )
{
    int expected_value = 10;
    rtl::expected<int, std::string> exp = expected_value;

    REQUIRE(*exp == 10);
}

TEST_CASE( "rtl::expected::operator ->", "[expected]" )
{
    class MyObject
    {
    public:
        int value { 10 };
    };
    rtl::expected<MyObject, std::string> exp = MyObject();

    REQUIRE(exp->value == 10);
}

TEST_CASE( "rtl::expected::operator bool(true)", "[expected]" )
{
    class MyObject
    {
    public:
        int value { 10 };
    };
    rtl::expected<MyObject, std::string> exp = MyObject();

    REQUIRE(static_cast<bool>(exp));
}

TEST_CASE( "rtl::expected::operator bool(false)", "[expected]" )
{
    class MyObject
    {
    public:
        int value { 10 };
    };
    rtl::expected<MyObject, std::string> exp = rtl::unexpected<std::string>(std::string("something went wrong"));

    REQUIRE(static_cast<bool>(exp) == false);
}

TEST_CASE( "rtl::expected::has_value(true)", "[expected]" )
{
    class MyObject
    {
    public:
        int value { 10 };
    };
    rtl::expected<MyObject, std::string> exp = MyObject();

    REQUIRE(exp.has_value());
}

TEST_CASE( "rtl::expected::has_value(false)", "[expected]" )
{
    class MyObject
    {
    public:
        int value { 10 };
    };
    rtl::expected<MyObject, std::string> exp = rtl::unexpected<std::string>(std::string("something went wrong"));

    REQUIRE(exp.has_value() == false);
}
