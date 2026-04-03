#ifndef __RTL_UTILITY_H
#define __RTL_UTILITY_H

#include <type_traits>
#include <iostream>

namespace rtl
{
    /* ------------------- begin: as_const ------------------- */
    template <typename T>
    auto as_const(T& t) noexcept -> typename std::add_const<T>::type 
    {
        return t;
    }

    template <typename T>
    auto as_const(const T&&) noexcept -> typename std::add_const<T>::type = delete;
    /* ------------------- end:   as_const ------------------- */
} // namespace rtl

#endif // __RTL_UTILITY_H
