#include <iostream>
#include <rtl/utility.h>
#include <catch2/catch_test_macros.hpp>

class ConstTracker
{
public:
    static constexpr int ConstValue = 1;
    static constexpr int NonConstValue = 2;

    int get() const { return ConstValue; }
    int get() { return NonConstValue; }

    static int test_get(ConstTracker& tracker) { return tracker.get(); }
    static int test_const_get(const ConstTracker& tracker) { return rtl::as_const(tracker).get(); }
};

TEST_CASE( "Const method called using rtl::as_const", "[as_const]") {
    ConstTracker tracker {};

    REQUIRE( ConstTracker::test_get(tracker) == ConstTracker::NonConstValue );
    REQUIRE( ConstTracker::test_const_get(tracker) == ConstTracker::ConstValue );
}