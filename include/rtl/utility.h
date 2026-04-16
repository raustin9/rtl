#ifndef __RTL_UTILITY_H
#define __RTL_UTILITY_H

#include <iostream>
#include <type_traits>

namespace rtl
{
    /* ------------------- begin: as_const ------------------- */
    template <typename T>
    auto
    as_const (T &t) noexcept -> typename std::add_const<T>::type
    {
        return t;
    }

    template <typename T>
    auto as_const (const T &&) noexcept -> typename std::add_const<T>::type = delete;
    /* ------------------- end:   as_const ------------------- */

    // C++ 17 std::in_place_t
    // https://en.cppreference.com/w/cpp/utility/in_place.html
    struct in_place_t
    {
        explicit in_place_t () = default;
    };

    constexpr in_place_t in_place{};
} // namespace rtl

#endif // __RTL_UTILITY_H
