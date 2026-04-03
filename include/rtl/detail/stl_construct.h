#ifndef __RTL_DETAIL_STL_CONSTRUCT_H
#define __RTL_DETAIL_STL_CONSTRUCT_H

#include <type_traits>
namespace rtl::__detail
{
    /* ------------------- begin: construct_at ------------------- */

    // Array specialization for construct_at
    template <
        typename T, 
        typename ... Args,
        std::enable_if_t<std::is_array<T>::value, int> = 0
    >
    auto construct_at(T* location, Args&& ... args) -> T*
    {
        static_assert(sizeof...(Args) == 0 && "construct_at for array types must not use any arguments to initialize the array");

        void* loc = location;
        return ::new(loc) T[1]();
    }
    
    template <
        typename T, 
        typename ... Args,
        std::enable_if_t<!std::is_array<T>::value, int> = 0
    >
    auto construct_at(T* location, Args&& ... args) -> T*
    {
        void* loc = location;
        return ::new(loc) T(std::forward<Args>(args)...);
    }

    /* ------------------- end:   construct_at ------------------- */
} // namespace rtl::__detail

#endif // __RTL_DETAIL_STL_CONSTRUCT_H
