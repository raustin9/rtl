#ifndef __RTL_STRING_VIEW
#define __RTL_STRING_VIEW

#include "detail/prelude.h"
#include "detail/functional_hash.h"
#include "type_traits.h"

#include <cstdio>
#include <cassert>
#include <iostream>
#include <iterator>
#include <stdexcept>

#include "detail/functional_hash.h"

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

        constexpr basic_string_view(const CharT* str) noexcept
            : m_str{ str }
            , m_len{ traits_type::length(str) }
        {}

        constexpr basic_string_view(const CharT* str, size_t len) noexcept
            : m_str{ str }
            , m_len{ len }
        {}

        constexpr auto operator=(const basic_string_view&) noexcept -> basic_string_view& = default;

        // std::string conversion
        // NOTE: This is implicit intentionally
        constexpr basic_string_view(const std::basic_string<CharT>& str) noexcept
            : m_str{ str.data() }
            , m_len{ str.length() }
        {}


    // Iterator
    public:
        constexpr auto begin() const noexcept -> const_iterator { return this->m_str; }
        constexpr auto end() const noexcept -> const_iterator { return this->m_str + this->m_len; }

        constexpr auto cbegin() const noexcept -> const_iterator { return this->m_str; }
        constexpr auto cend() const noexcept -> const_iterator { return this->m_str + this->m_len; }

        constexpr auto rbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator{this->end()}; }
        constexpr auto rend() const noexcept -> const_reverse_iterator { return const_reverse_iterator{this->begin()}; }

        constexpr auto crbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator{this->end()}; }
        constexpr auto crend() const noexcept -> const_reverse_iterator { return const_reverse_iterator{this->begin()}; }

    // API
    public:
        constexpr size_type size() const noexcept { return m_len; }
        constexpr size_type length() const noexcept { return m_len; }
        constexpr size_type max_size() const noexcept
        {
            return (npos - sizeof(size_type) - sizeof(void*))
                / sizeof(value_type)
                / 4;
        }

        constexpr bool empty() const noexcept { return m_len == 0; }

        constexpr const_reference operator[](size_type pos) const noexcept
        {
            assert(pos < this->size() && "index out of range");
            return *(this->m_str + pos);
        }

        constexpr const_reference at(size_type pos) const
        {
            if (pos >= this->size())
            {
                char msg[300] {};
                snprintf(msg, sizeof(msg), "index out of range: %zu", pos);
                throw std::out_of_range(msg);
            }

            return *(this->m_str + pos);
        }

        constexpr const_reference front() const noexcept
        {
            assert(!this->empty());
            return *(this->m_str);
        }

        constexpr const_reference back() const noexcept
        {
            assert(!this->empty());
            return *(this->m_str + this->m_len - 1);
        }

        constexpr const_pointer data() const noexcept { return this->m_str; }

        constexpr void remove_prefix(size_type n) noexcept
        {
            assert(n < this->size() && "index out of range");
            this->m_str += n;
            this->m_len -= n;
        }

        constexpr void remove_suffix(size_type n) noexcept
        {
            assert(n < this->size() && "index out of range");
            this->m_len -= n;
        }

        constexpr void swap(basic_string_view& other) noexcept
        {
            auto tmp = *this;
            *this = other;
            other = tmp;
        }

        size_type copy(CharT* str, size_type n, size_type pos = 0) const
        {
            pos = __detail::__sv_check(this->size(), pos, "basic_string_view::copy");
            const size_type rlen = std::min<size_type>(n, this->m_len - pos);
            traits_type::copy(str, this->data() + pos , rlen);
            return rlen;
        }

        constexpr basic_string_view substr(size_type pos = 0, size_type n = npos) const
        {
            pos = __detail::__sv_check(this->size(), pos, "basic_string_view::substr");
            const size_type rlen = std::min<size_type>(n, this->m_len - pos);
            return basic_string_view{this->data() + pos, rlen};
        }

        constexpr basic_string_view subview(size_type pos = 0, size_type n = npos) const { return this->substr(pos, n); }

    // compare
    public:
        constexpr int compare(basic_string_view str) const noexcept
        {
            const size_type rlen = std::min(this->size(), str.size());
            int ret = traits_type::compare(this->data(), str.data(), rlen);
            if (ret == 0)
            {
                ret = static_compare(this->size(), str.size());
            }

            return ret;
        }

        constexpr int compare(size_type pos1, size_type n1, basic_string_view str) const
        {
            return this->substr(pos1, n1).compare(str);
        }

        constexpr int compare(size_type pos1, size_type n1, basic_string_view str, size_type pos2, size_type n2) const
        {
            return this->substr(pos1, n1).compare(str.substr(pos2, n2));
        }

        constexpr int compare(const CharT* str) const noexcept
        {
            return this->compare(basic_string_view{str});
        }

        constexpr int compare(size_type pos1, size_type n1, const CharT* str) const
        {
            return this->substr(pos1, n1).compare(basic_string_view{str});
        }

        constexpr int compare(size_type pos1, size_type n1, const CharT* str, size_type pos2, size_type n2) const
        {
            return this->substr(pos1, n1).compare(basic_string_view{str}.substr(pos2, n2));
        }

    // starts_with + ends_with
    public:
        constexpr bool starts_with(basic_string_view pre) const noexcept
        {
            return this->size() >= pre.size()
                && traits_type::compare(this->data(), pre.data(), pre.size()) == 0;
        }

        constexpr bool starts_with(CharT pre) const noexcept
        {
            return !this->empty() && traits_type::eq(this->front(), pre);
        }

        constexpr bool starts_with(const CharT* pre) const noexcept
        {
            return this->starts_with(basic_string_view{pre});
        }

        constexpr bool ends_with(basic_string_view end) const noexcept
        {
            const size_type len = this->size();
            const size_type end_len = end.size();
            return len >= end_len && traits_type::compare(this->end() - end_len, end.data(), end_len) == 0;
        }

        constexpr bool ends_with(CharT end) const noexcept
        {
            return !this->empty() && traits_type::eq(this->back(), end);
        }

        constexpr bool ends_with(const CharT* end) const noexcept
        {
            return this->ends_with(basic_string_view{end});
        }

    // contains + find
    public:
        constexpr bool contains(basic_string_view str) const noexcept
        {
            return this->find(str) != npos;
        }

        constexpr bool contains(CharT c) const noexcept
        {
            return this->find(c) != npos;
        }

        constexpr bool contains(const CharT* str) const noexcept
        {
            return this->find(str) != npos;
        }

        constexpr size_type find(basic_string_view str, size_type pos = 0) const noexcept
        {
            return this->find(str.data(), pos, str.size());
        }

        constexpr size_type find(CharT c, size_type pos = 0) const noexcept;
        constexpr size_type find(const CharT* str, size_type pos, size_type n) const noexcept;

        constexpr size_type find(const CharT* str, size_type pos = 0) const noexcept
        {
            return this->find(str, pos, traits_type::length(str));
        }

        constexpr size_type rfind(basic_string_view str, size_type pos = npos) const noexcept
        {
            return this->rfind(str.data(), pos, str.size());
        }

        constexpr size_type rfind(CharT str, size_type pos = npos) const noexcept;
        constexpr size_type rfind(const CharT* str, size_type pos, size_type n) const noexcept;

        constexpr size_type rfind(const CharT* str, size_type pos = npos) const noexcept
        {
            return this->rfind(str, pos, traits_type::length(str));
        }

        // find_first_of
        constexpr size_type find_first_of(basic_string_view str, size_type pos = 0) const noexcept
        {
            return this->find_first_of(str.data(), pos, str.size());
        }

        constexpr size_type find_first_of(CharT c, size_type pos = 0) const noexcept
        {
            return this->find(c, pos);
        }

        constexpr size_type find_first_of(const CharT* str, size_type pos, size_type n) const noexcept;

        constexpr size_type find_first_of(const CharT* str, size_type pos = 0) const noexcept
        {
            return this->find_first_of(str, pos, traits_type::length(str));
        }

        // find_last_of
        constexpr size_type find_last_of(basic_string_view str, size_type pos = npos) const noexcept
        {
            return this->find_last_of(str.data(), pos, str.size());
        }

        constexpr size_type find_last_of(CharT c, size_type pos = npos) const noexcept
        {
            return this->rfind(c, pos);
        }

        constexpr size_type find_last_of(const CharT* str, size_type pos, size_type n) const noexcept;

        constexpr size_type find_last_of(const CharT* str, size_type pos = 0) const noexcept
        {
            return this->find_last_of(str, pos, traits_type::length(str));
        }

        // find_first_not_of
        constexpr size_type find_first_not_of(basic_string_view str, size_type pos = 0) const noexcept
        {
            return this->find_first_not_of(str.data(), pos, str.size());
        }

        constexpr size_type find_first_not_of(CharT c, size_type pos) const noexcept;
        constexpr size_type find_first_not_of(const CharT* str, size_type pos, size_type n) const noexcept;

        constexpr size_type find_first_not_of(const CharT* str, size_type pos = 0) const noexcept
        {
            return this->find_first_not_of(str, pos, traits_type::length(str));
        }

        // find_last_not_of
        constexpr size_type find_last_not_of(basic_string_view str, size_type pos = npos) const noexcept
        {
            return this->find_last_not_of(str.data(), pos, str.size());
        }

        constexpr size_type find_last_not_of(CharT c, size_type pos = npos) const noexcept;
        constexpr size_type find_last_not_of(const CharT* str, size_type pos, size_type n) const noexcept;

        constexpr size_type find_last_not_of(const CharT* str, size_type pos = npos) const noexcept
        {
            return this->find_last_not_of(str, pos, traits_type::length(str));
        }

    // Private methods
    private:
        static constexpr int static_compare(size_type n1, size_type n2) noexcept
        {
            using limits = std::numeric_limits<size_type>;
            const difference_type diff = n1 - n2;
            if (diff > limits::max())
                return limits::max();
            if (diff < limits::min())
                return limits::min();
            return static_cast<int>(diff);
        }

    private:
        const value_type* m_str;
        size_type         m_len;
    };

    template <typename CharT, typename Traits>
    constexpr bool operator==(basic_string_view<CharT, Traits> lhs, type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept
    {
        return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
    }

    template <typename CharT, typename Traits>
    constexpr bool operator==(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) noexcept
    {
        return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
    }

    template <typename CharT, typename Traits>
    constexpr bool operator==(type_identity_t<basic_string_view<CharT, Traits>> lhs, basic_string_view<CharT, Traits> rhs) noexcept
    {
        return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
    }

    template <typename CharT, typename Traits>
    constexpr bool operator!=(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template <typename CharT, typename Traits>
    constexpr bool operator!=(basic_string_view<CharT, Traits> lhs, type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template <typename CharT, typename Traits>
    constexpr bool operator!=(type_identity_t<basic_string_view<CharT, Traits>> lhs, basic_string_view<CharT, Traits> rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template <typename CharT, typename Traits>
    constexpr bool operator<(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    template <typename CharT, typename Traits>
    constexpr bool operator<(basic_string_view<CharT, Traits> lhs, type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    template <typename CharT, typename Traits>
    constexpr bool operator<(type_identity_t<basic_string_view<CharT, Traits>> lhs, basic_string_view<CharT, Traits> rhs) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    template <typename CharT, typename Traits>
    constexpr bool operator>(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    template <typename CharT, typename Traits>
    constexpr bool operator>(basic_string_view<CharT, Traits> lhs, type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    template <typename CharT, typename Traits>
    constexpr bool operator>(type_identity_t<basic_string_view<CharT, Traits>> lhs, basic_string_view<CharT, Traits> rhs) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    template <typename CharT, typename Traits>
    constexpr bool operator<=(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    template <typename CharT, typename Traits>
    constexpr bool operator<=(basic_string_view<CharT, Traits> lhs, type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    template <typename CharT, typename Traits>
    constexpr bool operator<=(type_identity_t<basic_string_view<CharT, Traits>> lhs, basic_string_view<CharT, Traits> rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    template <typename CharT, typename Traits>
    constexpr bool operator>=(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    template <typename CharT, typename Traits>
    constexpr bool operator>=(basic_string_view<CharT, Traits> lhs, type_identity_t<basic_string_view<CharT, Traits>> rhs) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    template <typename CharT, typename Traits>
    constexpr bool operator>=(type_identity_t<basic_string_view<CharT, Traits>> lhs, basic_string_view<CharT, Traits> rhs) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }


    // TODO: validate the expected behavior of this
    template <typename CharT, typename Traits>
    inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, basic_string_view<CharT, Traits> str)
    {
        os << str.data();
        return os;
    }

    using string_view = basic_string_view<char>;
    using wstring_view = basic_string_view<wchar_t>;

#if defined(CXX_LIVE_MIN_VERSION_20)
    using u8string_view = basic_string_view<char8_t>;
#endif // C++ 20
    using u16string_view = basic_string_view<char16_t>;
    using u32string_view = basic_string_view<char32_t>;

    template <typename CharT, typename Traits>
    constexpr typename basic_string_view<CharT, Traits>::size_type
    basic_string_view<CharT, Traits>::find(const CharT* str, size_type pos, size_type n) const noexcept
    {
        if (n == 0)
            return pos <= this->size() ? pos : npos;
        if (pos >= this->size())
            return npos;

        const CharT elem_0 = str[0];
        const CharT* first = this->m_str + pos;
        const CharT* const last = this->m_str + this->size();
        size_type len = this->size() - pos;

        while (len >= n)
        {
            first = traits_type::find(first, len - n + 1, elem_0);
            if (!first)
            {
                return npos;
            }

            if (traits_type::compare(first, str, n) == 0)
                return first - this->m_str;
            len = last - ++first;
        }

        return npos;
    }

    template <typename CharT, typename Traits>
    constexpr typename basic_string_view<CharT, Traits>::size_type
    basic_string_view<CharT, Traits>::find(CharT c, size_type pos) const noexcept
    {
        size_type ret = npos;
        if (pos < this->size())
        {
            const size_type n = this->size() - pos;
            const CharT* p = traits_type::find(this->m_str + pos, n, c);
            if (p)
                ret = p - this->m_str;
        }
        return ret;
    }

    template <typename CharT, typename Traits>
    constexpr typename basic_string_view<CharT, Traits>::size_type
    basic_string_view<CharT, Traits>::rfind(const CharT* str, size_type pos, size_type n) const noexcept
    {
        if (n <= this->size())
        {
            pos = std::min(this->size() - n, pos);
            do
            {
                if (traits_type::compare(this->data() + pos, str, n) == 0)
                    return pos;
            } while (pos-- > 0);
        }

        return npos;
    }

    template <typename CharT, typename Traits>
    constexpr typename basic_string_view<CharT, Traits>::size_type
    basic_string_view<CharT, Traits>::rfind(CharT c, size_type pos) const noexcept
    {
        size_type size = this->size();
        if (size > 0)
        {
            if (--size > pos)
                size = pos;
            for (++size; size-- > 0;)
                if (traits_type::eq(this->m_str[size], c))
                    return size;
        } else
        {
            std::cout << "SIZE = 0\n";
        }
        return npos;
    }

    template <typename CharT, typename Traits>
    constexpr typename basic_string_view<CharT, Traits>::size_type
    basic_string_view<CharT, Traits>::find_first_of(const CharT* str, size_type pos, size_type n) const noexcept
    {
        for (; n && pos < this->size(); ++pos)
        {
            const CharT* p = traits_type::find(this->m_str, n, this->m_str[pos]);
            if (p)
                return pos;
        }

        return npos;
    }

    template <typename CharT, typename Traits>
    constexpr typename basic_string_view<CharT, Traits>::size_type
    basic_string_view<CharT, Traits>::find_last_of(const CharT* str, size_type pos, size_type n) const noexcept
    {
        size_type size = this->size();
        if (size && n)
        {
            if (--size > pos)
                size = pos;
            do
            {
                if (traits_type::find(str, n, this->m_str[size]))
                    return size;
            }
            while (--size != 0);
        }
        return npos;
    }


    template <typename CharT, typename Traits>
    constexpr typename basic_string_view<CharT, Traits>::size_type
    basic_string_view<CharT, Traits>::find_first_not_of(const CharT* str, size_type pos, size_type n) const noexcept
    {
        for (; pos < this->size(); ++pos)
        {
            if (!traits_type::find(str, n, this->m_str[pos]))
                return pos;
        }
        return npos;
    }

    template <typename CharT, typename Traits>
    constexpr typename basic_string_view<CharT, Traits>::size_type
    basic_string_view<CharT, Traits>::find_first_not_of(CharT c, size_type pos) const noexcept
    {
        for (; pos < this->size(); ++pos)
        {
            if (!traits_type::eq(this->m_str[pos], c))
                return pos;
        }
        return npos;
    }

    template <typename CharT, typename Traits>
    constexpr typename basic_string_view<CharT, Traits>::size_type
    basic_string_view<CharT, Traits>::find_last_not_of(const CharT* str, size_type pos, size_type n) const noexcept
    {
        size_type size = this->size();
        if (size)
        {
            if (--size > pos)
                size = pos;
            do
            {
                if (!traits_type::find(str, n, this->m_str[size]))
                    return size;
            } while (size--);
        }

        return npos;
    }

    template <typename CharT, typename Traits>
    constexpr typename basic_string_view<CharT, Traits>::size_type
    basic_string_view<CharT, Traits>::find_last_not_of(CharT c, size_type pos) const noexcept
    {
        size_type size = this->size();
        if (size)
        {
            if (--size > pos)
                size = pos;
            do
            {
                if (!traits_type::eq(this->m_str[size], c))
                    return size;
            } while (size--);
        }

        return npos;
    }
} // namespace rtl

// std::hash
namespace std
{
    template<>
    struct hash<rtl::string_view>
    {
        size_t operator()(const rtl::string_view& str) const noexcept
        {
            return rtl::__detail::__hash_impl::hash(str.data(), str.size());
        }
    };

    template <>
    struct hash<rtl::wstring_view>
    {
        size_t operator()(const rtl::wstring_view& str) const noexcept
        {
            return rtl::__detail::__hash_impl::hash(str.data(), str.length() * sizeof(rtl::wstring_view::traits_type::char_type));
        }
    };

#if defined(CXX_LIVE_MIN_VERSION_20)
    template <>
    struct hash<rtl::u8string_view>
    {
        size_t operator()(const rtl::u8string_view& str) const noexcept
        {
            return rtl::__detail::__hash_impl::hash(str.data(), str.length() * sizeof(rtl::u8string_view::traits_type::char_type));
        }
    };
#endif // C++20

    template <>
    struct hash<rtl::u16string_view>
    {
        size_t operator()(const rtl::u16string_view& str) const noexcept
        {
            return rtl::__detail::__hash_impl::hash(str.data(), str.length() * sizeof(rtl::u16string_view::traits_type::char_type));
        }
    };

    template <>
    struct hash<rtl::u32string_view>
    {
        size_t operator()(const rtl::u32string_view& str) const noexcept
        {
            return rtl::__detail::__hash_impl::hash(str.data(), str.length() * sizeof(rtl::u32string_view::traits_type::char_type));
        }
    };
}

#endif // __RTL_STRING_VIEW
