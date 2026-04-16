#ifndef __RTL_SPAN_H
#define __RTL_SPAN_H

#include "detail/stl_iterator.h"
#include "type_traits.h"
#include "memory.h"

#include <array>
#include <assert.h>
#include <cstddef>

namespace rtl
{
    constexpr std::size_t dynamic_extent = static_cast<std::size_t>(-1);

    template <typename T, std::size_t Extent>
    class span;

    namespace __detail
    {
        template <typename T>
        constexpr bool __is_span = false;

        template <typename T, std::size_t Extent>
        constexpr bool __is_span<span<T, Extent>> = true;

        template <typename T>
        constexpr bool __is_std_array = false;

        template <typename T, std::size_t Extent>
        constexpr bool __is_std_array<std::array<T, Extent>> = true;

        template <std::size_t Extent>
        constexpr bool __is_dynamic_offset = Extent == dynamic_extent;

        template <std::size_t Extent>
        class __extent_storage
        {
        public:
            constexpr static std::size_t extent = Extent;

            constexpr __extent_storage(std::size_t n) noexcept
            {
                assert(extent == Extent && "__extent_storage(N): N != span::extent");
            }

            constexpr __extent_storage(std::integral_constant<std::size_t, Extent>) noexcept {}

            template <std::size_t G>
            __extent_storage(std::integral_constant<std::size_t, G>) = delete;

            static constexpr size_t get_extent() noexcept { return Extent; }
        };

        template<>
        class __extent_storage<dynamic_extent>
        {
        public:
            constexpr __extent_storage(std::size_t n) noexcept
                : m_extent{ n }
            {}

            constexpr size_t get_extent() const noexcept { return m_extent; }

        private:
            std::size_t m_extent;
        };

        template <typename T> struct __span_ptr { T* const ptr; };
    } // namespace detail

    template <typename T, std::size_t Extent = dynamic_extent>
    class span
    {
        template <std::size_t Offset, std::size_t Count>
        static constexpr typename std::enable_if<
            !__detail::__is_dynamic_offset<Count> && !__detail::__is_dynamic_offset<Offset>
        >::type
        subspan_extent() noexcept { return Count; }

        template <std::size_t Offset, std::size_t Count>
        static constexpr typename std::enable_if<
            __detail::__is_dynamic_offset<Offset> && !__detail::__is_dynamic_offset<Count>
        >::type
        subspan_extent() noexcept { return Extent - Offset; }

        template <std::size_t Offset, std::size_t Count>
        static constexpr typename std::enable_if<
            !__detail::__is_dynamic_offset<Offset> && !__detail::__is_dynamic_offset<Count>
        >::type
        subspan_extent() noexcept { return dynamic_extent; }

        template <typename U, std::size_t ArrayExtent, typename =
            typename std::enable_if<(__detail::__is_dynamic_offset<Extent> || __detail::__is_dynamic_offset<ArrayExtent>)>::type
        >
        using __is_compatible_array = __detail::__is_array_convertible<T, U>;

        template <typename Ref>
        using __is_compatible_ref = __detail::__is_array_convertible<T, std::remove_reference_t<Ref>>;

        struct __iter_tag;

        template <std::size_t N>
        static constexpr std::integral_constant<std::size_t, N> __v {};

        template <std::size_t N>
        using __make_extent = std::integral_constant<std::size_t, N>;

    public:
        using element_type = T;
        using value_type = std::remove_cv_t<T>;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using iterator = __detail::__normal_iterator<pointer, __iter_tag>;
        using reverse_iterator = std::reverse_iterator<iterator>;

        static constexpr std::size_t extent = Extent;

    // constructors
    public:
        template <typename = typename std::enable_if<__detail::__is_dynamic_offset<Extent> || Extent == 0>::type>
        constexpr span() noexcept
            : m_ptr{ nullptr }
            , m_extent(__make_extent<0>{})
        {}

        template <std::size_t ArrayExtent, typename =
            typename std::enable_if<__detail::__is_dynamic_offset<Extent> || ArrayExtent == Extent>::type>
        constexpr span(type_identity_t<element_type> (&arr)[ArrayExtent]) noexcept
            : m_ptr( arr )
            , m_extent(__make_extent<ArrayExtent>{})
        {}

        template <typename Iter, typename = typename std::enable_if<
            __detail::is_contiguous_iterator<Iter>::value
            >::type>
        constexpr explicit span(Iter first, size_type count) noexcept
            : m_ptr(to_address(first)), m_extent(count)
        {}

        template <typename U, std::size_t ArrayExtent, typename =
            typename std::enable_if_t<__detail::__is_dynamic_offset<Extent> || ArrayExtent == Extent, void>>
        constexpr span(std::array<U, ArrayExtent>& arr) noexcept
            : m_ptr( arr.data() )
            , m_extent(__make_extent<ArrayExtent>{})
        {}

        template <typename U, std::size_t ArrayExtent, typename =
            typename std::enable_if_t<__is_compatible_array<U, ArrayExtent>::value, void>>
        constexpr span(const std::array<U, ArrayExtent>& arr) noexcept
            : m_ptr( arr.data() )
        , m_extent(__make_extent<ArrayExtent>{})
        {}

        constexpr span(const span&) noexcept = default;

        template <typename U, std::size_t UExtent, typename =
            typename std::enable_if_t<
                (__detail::__is_dynamic_offset<Extent> || __detail::__is_dynamic_offset<UExtent> || UExtent == Extent)
            && (__detail::__is_array_convertible<T, U>::value)>>
        constexpr explicit span(const span<U, UExtent>& other) noexcept
            : m_ptr( other.data() )
            , m_extent( other.size() )
        {}

        constexpr span& operator=(const span&) noexcept = default;

    // observers
    public:
        constexpr size_type size() const noexcept { return m_extent.get_extent(); }

        constexpr size_type size_bytes() const noexcept { return m_extent.get_extent() * sizeof(element_type); }

        constexpr bool empty() const noexcept { return size() == 0; }

        constexpr reference front() const noexcept
        {
            assert(!empty() && "calling span::front() on empty span");
            return *this->m_ptr;
        }

        constexpr reference back() const noexcept
        {
            assert(!empty() && "calling span::back() on empty span");
            return *(this->m_ptr + (size() - 1));
        }

        constexpr reference operator[](size_type idx) const noexcept
        {
            assert(!empty() && "calling span::operator[]() on empty span");
            return *(this->m_ptr + idx);
        }

        constexpr reference at(size_type idx) const
        {
            if (idx >= size())
            {
                char msg[200] {};
                snprintf(msg, 200, "span::at(%zu) out-of-range for span of size %zu", idx, size());
                throw std::out_of_range(msg);
            }

            return *(this->m_ptr + idx);
        }

        constexpr pointer data() const noexcept { return this->m_ptr; }

    // iterator
    public:
        constexpr iterator begin() const noexcept { return iterator(this->m_ptr); }
        constexpr iterator end() const noexcept { return iterator(this->m_ptr + this->size()); }

        constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(this->end()); }
        constexpr reverse_iterator rend() const noexcept { return reverse_iterator(this->begin()); }

        constexpr iterator cbegin() const noexcept { return this->begin(); }
        constexpr iterator cend() const noexcept { return this->end(); }

        constexpr reverse_iterator crbegin() const noexcept { return this->rbegin(); }
        constexpr reverse_iterator crend() const noexcept { return this->rend(); }

    // subviews
    public:
        template <std::size_t Count>
        constexpr typename
        std::enable_if<__detail::__is_dynamic_offset<Count> ,span<element_type, Count>>::type
        first() const noexcept
        {
            assert(Count <= size());
            using S = span<element_type, Count>;
            return S(SizedPtr{this->m_ptr});
        }

        template <std::size_t Count>
        constexpr typename
        std::enable_if<!__detail::__is_dynamic_offset<Count> ,span<element_type, Count>>::type
        first() const noexcept
        {
            static_assert(Count <= Extent, "");
            using S = span<element_type, Count>;
            return S(SizedPtr{this->m_ptr});
        }

        constexpr span<element_type, dynamic_extent>
        first(size_type count) const noexcept
        {
            assert(count <= size());
            return span<element_type>{this->m_ptr, count};
        }

        template <std::size_t Count>
        constexpr typename
        std::enable_if<__detail::__is_dynamic_offset<Count> ,span<element_type, Count>>::type
        last() const noexcept
        {
            assert(Count <= size());
            using S = span<element_type, Count>;
            return S( SizedPtr{this->data() + (this->size() - Count)} );
        }

        template <std::size_t Count>
        constexpr typename
        std::enable_if<!__detail::__is_dynamic_offset<Count> ,span<element_type, Count>>::type
        last() const noexcept
        {
            static_assert(Count <= Extent, "");
            using S = span<element_type, Count>;
            return S( SizedPtr{this->data() + (this->size() - Count)} );
        }


        constexpr span<element_type, dynamic_extent>
        last(size_type count) const noexcept
        {
            assert(count <= size());
            return span<element_type>{this->data() + (this->size() - count), count};
        }

        template <std::size_t Offset, std::size_t Count = dynamic_extent>
        constexpr auto subspan() const noexcept
            -> typename std::enable_if_t<
                __detail::__is_dynamic_offset<Extent>,
                span<element_type, subspan_extent<Offset, Count>()>
            >
        {
            assert(Offset <= size());

            using S = span<element_type, subspan_extent<Offset, Count>()>;

            assert(Count <= size());
            assert(Count <= (size() - Offset));

            return S(SizedPtr{ this->data() + Offset });
        }

        template <std::size_t Offset, std::size_t Count = dynamic_extent>
        constexpr auto subspan() const noexcept
            -> typename std::enable_if_t<
                !__detail::__is_dynamic_offset<Extent> && __detail::__is_dynamic_offset<Count>,
                span<element_type, subspan_extent<Offset, Count>()>
            >
        {
            static_assert(Offset <= Extent, "");

            using S = span<element_type, subspan_extent<Offset, Count>()>;

            return S(this->data() + Offset, this->size() - Offset);
        }

        constexpr span<element_type, dynamic_extent> subspan(size_type offset, size_type count = dynamic_extent) const noexcept
        {
            assert(offset <= size());
            if (count == dynamic_extent)
                count = this->size() - offset;
            else
            {
                assert(count <= size());
                assert(offset + count <= size());
            }

            return span<element_type>(this->data() + offset, count);
        }

    private:
        using SizedPtr = __detail::__span_ptr<T>;

    private:
        pointer m_ptr;
        __detail::__extent_storage<Extent> m_extent;
    };

} // namespace rtl

#endif // __RTL_SPAN_H
