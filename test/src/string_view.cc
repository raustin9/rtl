#include <catch2/catch_test_macros.hpp>
#include <rtl/string_view.h>
#include <sstream>
#include <string>

TEST_CASE ("default construction", "[string_view]") { rtl::basic_string_view<char> bsv; }

TEST_CASE ("implicit std::string conversion", "[string_view]")
{
    std::string str{ "hello, world" };
    rtl::basic_string_view<char> sv = str;

    REQUIRE (sv.length () == str.length ());
    REQUIRE (sv.data () == str.data ());

    REQUIRE (str == sv);
    REQUIRE (sv == str);
}

TEST_CASE ("simple compare", "[string_view]")
{
    std::string str{ "hello, world" };
    rtl::basic_string_view<char> sv = str;

    std::string str2{ "different" };
    rtl::basic_string_view<char> sv2 = str2;

    REQUIRE (sv == str);
    REQUIRE (str == sv);

    REQUIRE (sv != str2);
    REQUIRE (str2 != sv);

    REQUIRE (sv != sv2);
}

TEST_CASE ("copy construct", "[string_view]")
{
    std::string str{ "hello, world" };
    rtl::string_view sv = str;

    REQUIRE (sv == str);
    REQUIRE (str == sv);

    rtl::string_view sv2 = sv;

    REQUIRE (sv == str);
    REQUIRE (str == sv);
    REQUIRE (sv2 == str);
    REQUIRE (str == sv2);
}

TEST_CASE ("copy assign", "[string_view]")
{
    std::string str{ "hello, world" };
    rtl::string_view sv = str;

    REQUIRE (sv == str);
    REQUIRE (str == sv);

    rtl::string_view sv2;
    sv2 = sv;

    REQUIRE (sv == str);
    REQUIRE (str == sv);
    REQUIRE (sv2 == str);
    REQUIRE (str == sv2);
}

TEST_CASE ("ordering", "[string_view]")
{
    rtl::string_view first{ "abc" };
    rtl::string_view second{ "bcd" };

    REQUIRE (first != second);
    REQUIRE (first <= second);
    REQUIRE (first < second);
    REQUIRE (second >= first);
    REQUIRE (second > first);
}

TEST_CASE ("ordering with std::string (1)", "[string_view]")
{
    rtl::string_view first{ "abc" };
    std::string second{ "bcd" };

    REQUIRE (first != second);
    REQUIRE (first <= second);
    REQUIRE (first < second);
    REQUIRE (second >= first);
    REQUIRE (second > first);
}

TEST_CASE ("ordering with std::string (2)", "[string_view]")
{
    std::string first{ "abc" };
    std::string_view second{ "bcd" };

    REQUIRE (first != second);
    REQUIRE (first <= second);
    REQUIRE (first < second);
    REQUIRE (second >= first);
    REQUIRE (second > first);
}

TEST_CASE ("contains (1)", "[string_view]")
{
    rtl::string_view sv = "hello, world";

    REQUIRE (sv.contains ("hello, world"));
    REQUIRE (sv.contains ("hello"));
    REQUIRE (sv.contains ("world"));
    REQUIRE (sv.contains (", world"));
    REQUIRE (sv.contains ("lo, wo"));

    REQUIRE_FALSE (sv.contains ("DOES NOT CONTAIN"));
    REQUIRE_FALSE (sv.contains ("HELLO"));
}
TEST_CASE ("contains (2)", "[string_view]")
{
    rtl::string_view sv = "hello, world";

    REQUIRE (sv.contains ("hello, world"));
    REQUIRE (sv.contains ("hello"));
    REQUIRE (sv.contains ("world"));
    REQUIRE (sv.contains (", world"));
    REQUIRE (sv.contains ("lo, wo"));

    REQUIRE (sv.contains ("h"));
    REQUIRE (sv.contains ('h'));
    REQUIRE (sv.contains ('e'));
    REQUIRE (sv.contains ('l'));
    REQUIRE (sv.contains ('l'));
    REQUIRE (sv.contains ('o'));

    REQUIRE_FALSE (sv.contains ("DOES NOT CONTAIN"));
    REQUIRE_FALSE (sv.contains ("HELLO"));

    REQUIRE_FALSE (sv.contains ('x'));
}

TEST_CASE ("find (1)", "[string_view]")
{
    rtl::string_view sv = "hello, world";

    rtl::string_view::size_type pos = sv.find ("hello");
    REQUIRE (pos == 0);

    pos = sv.find ("world");
    REQUIRE (pos == 7);

    pos = sv.find ("w");
    REQUIRE (pos == 7);

    pos = sv.find ("e");
    REQUIRE (pos == 1);

    pos = sv.find ('w');
    REQUIRE (pos == 7);

    pos = sv.find ('e');
    REQUIRE (pos == 1);

    pos = sv.find ('x');
    REQUIRE (pos == rtl::string_view::npos);
}

TEST_CASE ("find (2)", "[string_view]")
{
    rtl::string_view sv    = "hello, world";
    rtl::string_view inner = "lo";

    REQUIRE (sv.contains (inner));

    rtl::string_view::size_type pos = sv.find (inner);
    REQUIRE (pos == 3);
}

TEST_CASE ("rfind (1)", "[string_view]")
{
    rtl::string_view sv = "hello, world";

    rtl::string_view::size_type pos = sv.rfind ("hello");
    REQUIRE (pos == 0);

    pos = sv.rfind ("world");
    REQUIRE (pos == 7);

    pos = sv.rfind ("ello,");
    REQUIRE (pos == 1);

    pos = sv.rfind ("w");
    REQUIRE (pos == 7);

    pos = sv.rfind ("e");
    REQUIRE (pos == 1);

    pos = sv.rfind ('w');
    REQUIRE (pos == 7);

    pos = sv.rfind ('e');
    REQUIRE (pos == 1);

    pos = sv.rfind ('l');
    REQUIRE (pos == 10);

    pos = sv.rfind ('x');
    REQUIRE (pos == rtl::string_view::npos);
}

TEST_CASE ("rfind (2)", "[string_view]")
{
    rtl::string_view sv    = "hello, world";
    rtl::string_view inner = "hello";

    REQUIRE (sv.contains (inner));

    rtl::string_view::size_type pos = sv.rfind (inner);
    REQUIRE (pos == 0);

    inner = "world";
    pos   = sv.rfind (inner);
    REQUIRE (pos == 7);

    sv  = "hello, world world";
    pos = sv.rfind (inner);
    REQUIRE (pos == 13);
    REQUIRE (inner[0] == sv[pos]);
}

TEST_CASE ("find_first_of + find_last_of", "[string_view]")
{
    rtl::string_view sv      = "abcd abcd abcd";
    rtl::string_view pattern = "abcd";

    REQUIRE (sv.contains (pattern));
    REQUIRE (sv.find (pattern) < sv.rfind (pattern));

    REQUIRE (sv.find_first_of (pattern) == 0);

    REQUIRE (sv.find_last_of (pattern) == 13);

    pattern = " abc";
    REQUIRE (sv.find_last_of (pattern) == 12);
}

TEST_CASE ("starts_with", "[string_view]")
{
    rtl::string_view sv    = "abcd abcd abcd";
    const char *pattern    = "abcd";
    rtl::string_view inner = pattern;

    REQUIRE (sv.starts_with (pattern));
    REQUIRE (sv.starts_with (inner));
    REQUIRE (sv.ends_with (pattern));
    REQUIRE (sv.ends_with (inner));

    REQUIRE_FALSE (sv.starts_with ("aklfjdsf"));
    REQUIRE_FALSE (sv.ends_with ("aklfjdsf"));

    REQUIRE (sv.starts_with ('a'));
    REQUIRE (sv.ends_with ('d'));
    REQUIRE_FALSE (sv.starts_with ('d'));
    REQUIRE_FALSE (sv.ends_with ('a'));
}

TEST_CASE ("stream (1)", "[string_view]")
{
    const char *pattern = "hello, world";
    rtl::string_view sv = pattern;

    std::ostringstream oss;
    oss << sv;

    REQUIRE (oss.str () == pattern);
    REQUIRE (oss.str () == sv);
}
