#include <rtl/string_view.h>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "default construction", "[string_view]" )
{
    rtl::basic_string_view<char> bsv;
}