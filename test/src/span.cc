#include <catch2/catch_test_macros.hpp>
#include <rtl/span.h>

TEST_CASE ("construct from std::array", "[span]")
{
    std::array<int, 3> arr = { { 1, 2, 3 } };

    rtl::span<int> s = arr;

    REQUIRE (s.size () == arr.size ());
    REQUIRE (arr[0] == 1);
    REQUIRE (arr[1] == 2);
    REQUIRE (arr[2] == 3);
}

TEST_CASE ("construct from std::array in function", "[span]")
{
    constexpr std::size_t expected_size = 5;
    auto foo                            = [] (rtl::span<int> data) -> bool {
        REQUIRE (data.size () == expected_size);

        bool valid    = true;
        std::size_t i = 0;
        for (int num : data)
            {
                if (num != data[i])
                    valid = false;
                REQUIRE (num == data[i]);
                i++;
            }

        return valid;
    };
    std::array<int, expected_size> arr = { { 1, 2, 3, 4, 5 } };

    REQUIRE (foo (arr));
}

TEST_CASE ("construct from std::vector", "[span]")
{
    std::vector<int> v = { { 1, 2, 3, 4, 5 } };
    rtl::span<int> s   = v;

    std::size_t i = 0;
    for (int num : s)
        {
            REQUIRE (num == v[i++]);
        }
}

TEST_CASE ("construct from std::vector as parameter", "[span]")
{
    std::vector<int> v = { { 1, 2, 3, 4, 5 } };

    auto foo = [=] (rtl::span<int> data) {
        REQUIRE (data.size () == v.size ());
        for (std::size_t i = 0; i < data.size (); i++)
            {
                REQUIRE (data[i] == v[i]);
            }
    };

    foo (v);
}

TEST_CASE ("subspan (1)", "[span]")
{
    std::array<int, 10> arr = { { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 } };
    auto foo                = [=] (rtl::span<int> data) -> void {
        auto subspan = data.subspan (0, 5);

        std::size_t i = 0;
        for (int num : subspan)
            {
                REQUIRE (num == subspan[i]);
                REQUIRE (num == data[i]);
                REQUIRE (num == data[i]);

                i++;
            }
    };

    foo (arr);
}