#ifndef __RTL_UTILITY_H
#define __RTL_UTILITY_H

#include "detail/prelude.h"
#include <type_traits>
#include <utility>

namespace rtl
{
#ifdef CXX_LIVE_MIN_VERSION_17
    template <typename T>
    auto
    as_const(T &t) noexcept -> std::add_const_t<T &>
    {
        return std::as_const(t);
    }
#else
    // https://en.cppreference.com/w/cpp/utility/as_const.html
    template <typename T>
    auto
    as_const(T &t) noexcept -> typename std::add_const<T>::type
    {
        return t;
    }

    template <typename T>
    auto as_const(const T &&) noexcept -> typename std::add_const<T>::type = delete;
#endif // CXX_LIVE_MIN_VERSION_17

#ifdef CXX_LIVE_MIN_VERSION_17
    using in_place_t = std::in_place_t;
    constexpr in_place_t in_place{};
#else
    // https://en.cppreference.com/w/cpp/utility/in_place.html
    struct in_place_t
    {
        explicit in_place_t() = default;
    };

    constexpr in_place_t in_place{};
#endif // CXX_LIVE_MIN_VERSION_17
} // namespace rtl

#endif // __RTL_UTILITY_H
