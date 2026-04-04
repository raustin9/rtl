# Retrofitted Template Library
A collection of useful modern (C++17 onward) utilities for use in C++ 14 libraries and applications.
The focus of this is to map as closely to the standard as possible rather rather than trying to "improve" anything.
For example, accessing an error from a valid `std::expected` object is undefined behavior, so we could define it by throwing a `rtl::bad_expected_error_access`, but that is not our goal.