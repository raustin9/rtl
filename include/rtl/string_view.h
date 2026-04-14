#ifndef __RTL_STRING_VIEW
#define __RTL_STRING_VIEW

#include <cstdio>
#include <iterator>
#include <stdexcept>

#include "type_traits.h"

namespace rtl
{
    namespace __detail
    {
        constexpr size_t
        __sv_check(size_t size, size_t pos, const char* s)
        {
            if (pos > size)
            {
                char msg[300] {};
                snprintf(msg, sizeof(msg), "%s: pos (which is %zu) > size (which is %zu)", s, pos, size);
                throw std::out_of_range(msg);
            }

            return pos;
        }

        constexpr size_t
        __sv_limit(size_t size, size_t pos, size_t off) noexcept
        {
            const bool testoff = off < size - pos;
            return testoff ? off : size - pos;
        }
    } // namespace __detail

    template <typename CharT, typename Traits = std::char_traits<CharT>>
    class basic_string_view
    {
        static_assert(!is_array_v<CharT>, "");
        static_assert(is_trivially_copyable_v<CharT>, "");
        static_assert(is_trivially_copyable_v<CharT>
            && is_trivially_default_constructible_v<CharT>
            && is_standard_layout_v<CharT>, "");
        static_assert(is_same_v<CharT, typename Traits::char_type>, "");

    public:
        using traits_type = Traits;
        using value_type = CharT;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using const_iterator = const value_type*;
        using iterator = const_iterator;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using reverse_iterator = const_reverse_iterator;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        static constexpr size_type npos = static_cast<size_type>(-1);

    // Constructors
    public:
        constexpr basic_string_view() noexcept
            : m_str{ nullptr }
            , m_len{ 0 }
        {}

        constexpr basic_string_view(const basic_string_view&) noexcept = default;

    private:
        value_type* m_str;
        size_type   m_len;
    };
} // namespace rtl

#endif // __RTL_STRING_VIEW
