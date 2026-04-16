#ifndef __RTL_MEMORY_H
#define __RTL_MEMORY_H

#include "detail/stl_construct.h"
#include <utility>

namespace rtl
{
    /* ------------------- begin: construct_at ------------------- */
    // Alias for __detail::construct_at
    template <typename ... Args>
    auto construct_at(Args&& ... args) -> decltype(__memory_detail::construct_at(std::forward<Args>(args)...))
    {
        return __memory_detail::construct_at(std::forward<Args>(args)...);
    }
    /* ------------------- end:   construct_at ------------------- */

    /* ------------------- begin: destroy_at ------------------- */
    // Alias for __detail::destroy_at
    template <typename ... Args>
    auto destroy_at(Args&& ... args) -> decltype(__memory_detail::destroy_at(std::forward<Args>(args)...))
    {
        return __memory_detail::destroy_at(std::forward<Args>(args)...);
    }
    /* ------------------- end:   destroy_at ------------------- */

    template <typename T>
    constexpr T* to_address(T* p) noexcept
    {
        static_assert(!std::is_function<T>::value, "");
        return p;
    }

    template <typename T, typename = typename std::enable_if<!std::is_pointer<T>::value>>
    constexpr auto to_address(const T& p) noexcept
    {
        return to_address(p.operator->());
    }
} // namespace rtl

#endif // __RTL_MEMORY_H
