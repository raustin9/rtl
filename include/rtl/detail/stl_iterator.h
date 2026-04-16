#ifndef __RTL_DETAIL_STL_ITERATOR_H
#define __RTL_DETAIL_STL_ITERATOR_H
#include <iterator>

namespace rtl
{
    namespace __detail
    {
        template <typename Iterator, typename Container>
        class __normal_iterator
        {
          protected:
            Iterator m_current;

            typedef std::iterator_traits<Iterator> traits_type;

            template <typename Iter>
            using __convertible_from
                = std::enable_if_t<std::is_convertible<Iter, Container>::value>;

            // info
          public:
            using iterator_type     = Iterator;
            using iterator_category = typename traits_type::iterator_category;
            using value_type        = typename traits_type::value_type;
            using difference_type   = typename traits_type::difference_type;
            using reference         = typename traits_type::reference;
            using pointer           = typename traits_type::pointer;

            // constructors
          public:
            __normal_iterator () noexcept : m_current () {}

            explicit constexpr __normal_iterator (const Iterator &i) noexcept : m_current (i) {}

            template <typename Iter, typename = __convertible_from<Iter>>
            constexpr __normal_iterator (const __normal_iterator<Iter, Container> &i) noexcept
                : m_current (i.base ())
            {
            }

            // forward iterator requirements
          public:
            constexpr reference
            operator* () const noexcept
            {
                return *m_current;
            }

            constexpr pointer
            operator->() const noexcept
            {
                return m_current;
            }

            constexpr __normal_iterator &
            operator++ () noexcept
            {
                ++m_current;
                return *this;
            }

            constexpr __normal_iterator &
            operator++ (int) noexcept
            {
                return __normal_iterator (m_current++);
            }

            // bi-directional iterator requirements
          public:
            constexpr __normal_iterator &
            operator-- () noexcept
            {
                --m_current;
                return *this;
            }

            constexpr __normal_iterator &
            operator-- (int) noexcept
            {
                return __normal_iterator (m_current--);
            }

            // random access requirements
          public:
            constexpr reference
            operator[] (difference_type n) const noexcept
            {
                return m_current[n];
            }

            constexpr __normal_iterator &
            operator+= (difference_type n) noexcept
            {
                m_current += n;
                return *this;
            }

            constexpr __normal_iterator
            operator+ (difference_type n) noexcept
            {
                return __normal_iterator (m_current + n);
            }

            constexpr __normal_iterator &
            operator-= (difference_type n) noexcept
            {
                m_current -= n;
                return *this;
            }

            constexpr __normal_iterator
            operator- (difference_type n) noexcept
            {
                return __normal_iterator (m_current - n);
            }

            constexpr const Iterator &
            base () const noexcept
            {
                return m_current;
            }

          private:
            template <typename Iter>
            friend constexpr bool
            operator== (const __normal_iterator &lhs,
                        const __normal_iterator<Iter, Container> &rhs) noexcept
            {
                return lhs.base () == rhs.base ();
            }

            friend constexpr bool
            operator== (const __normal_iterator &lhs, const __normal_iterator &rhs) noexcept
            {
                return lhs.base () == rhs.base ();
            }

            template <typename Iter>
            friend constexpr bool
            operator!= (const __normal_iterator &lhs,
                        const __normal_iterator<Iter, Container> &rhs) noexcept
            {
                return lhs.base () != rhs.base ();
            }

            friend constexpr bool
            operator!= (const __normal_iterator &lhs, const __normal_iterator &rhs) noexcept
            {
                return lhs.base () != rhs.base ();
            }

            // random access
          private:
            template <typename Iter>
            friend constexpr inline bool
            operator< (const __normal_iterator *lhs,
                       const __normal_iterator<Iter, Container> &rhs) noexcept
            {
                return lhs->base () < rhs.base ();
            }

            friend constexpr inline bool
            operator< (const __normal_iterator *lhs, const __normal_iterator &rhs) noexcept
            {
                return lhs->base () < rhs.base ();
            }

            template <typename Iter>
            friend constexpr inline bool
            operator> (const __normal_iterator *lhs,
                       const __normal_iterator<Iter, Container> &rhs) noexcept
            {
                return lhs->base () > rhs.base ();
            }

            friend constexpr inline bool
            operator> (const __normal_iterator *lhs, const __normal_iterator &rhs) noexcept
            {
                return lhs->base () > rhs.base ();
            }

            template <typename Iter>
            friend constexpr inline bool
            operator<= (const __normal_iterator *lhs,
                        const __normal_iterator<Iter, Container> &rhs) noexcept
            {
                return lhs->base () <= rhs.base ();
            }

            friend constexpr inline bool
            operator<= (const __normal_iterator *lhs, const __normal_iterator &rhs) noexcept
            {
                return lhs->base () <= rhs.base ();
            }

            template <typename Iter>
            friend constexpr inline bool
            operator>= (const __normal_iterator *lhs,
                        const __normal_iterator<Iter, Container> &rhs) noexcept
            {
                return lhs->base () >= rhs.base ();
            }

            friend constexpr inline bool
            operator>= (const __normal_iterator *lhs, const __normal_iterator &rhs) noexcept
            {
                return lhs->base () >= rhs.base ();
            }

            template <typename Iter>
            friend constexpr auto
            operator- (const __normal_iterator &lhs,
                       const __normal_iterator<Iter, Container> &rhs) noexcept
                -> decltype (lhs.base () - rhs.base ())
            {
                return lhs.base () - rhs.base ();
            }

            friend constexpr difference_type
            operator- (const __normal_iterator &lhs, const __normal_iterator &rhs) noexcept
            {
                return lhs.base () - rhs.base ();
            }

            friend __normal_iterator
            operator+ (difference_type n, const __normal_iterator &rhs) noexcept
            {
                return __normal_iterator (rhs.base () + n);
            }
        };
    } // namespace __detail
} // namespace rtl

#endif // __RTL_DETAIL_STL_ITERATOR_H
