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
} // namespace rtl

#endif // __RTL_MEMORY_H
