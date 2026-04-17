#ifndef __RTL_EXPECTED_H
#define __RTL_EXPECTED_H

#include <cassert>
#include <exception>
#include <utility>

#include "detail/stl_construct.h"
#include "memory.h"
#include "type_traits.h"
#include "utility.h"

#ifdef CXX_LIVE_MIN_VERSION_23
#include <expected>
#endif // CXX_LIVE_MIN_VERSION_23

namespace rtl
{
    // Use STl if available
#ifdef CXX_LIVE_MIN_VERSION_23
    template <typename E>
    using unexpected = std::unexpected<E>;

    template <typename E>
    using bad_expected_access = std::bad_expected_access<E>;

    template <typename T, typename E>
    using expected = std::expected<T, E>;
#else
    template <typename T, typename E>
    class expected;

    template <typename E>
    class unexpected;

    template <typename T>
    class bad_expected_access;

    template <>
    class bad_expected_access<void> : public std::exception
    {
      protected:
        bad_expected_access() noexcept                            = default;
        bad_expected_access(const bad_expected_access &) noexcept = default;

        auto operator=(const bad_expected_access &) noexcept -> bad_expected_access & = default;
        auto operator=(bad_expected_access &&) noexcept -> bad_expected_access &      = default;

        ~bad_expected_access() = default;

      public:
        auto
        what() const noexcept -> const char * override
        {
            return "bad access to rtl::expected without expected value";
        }
    };

    template <typename E>
    class bad_expected_access final : public bad_expected_access<void>
    {
      public:
        explicit bad_expected_access(const E &e) noexcept : m_error{ std::move(e) } {}

        auto
        error() & noexcept -> E &
        {
            return m_error;
        }

        auto
        error() const & noexcept -> const E &
        {
            return m_error;
        }

        auto
        error() && noexcept -> E
        {
            return std::move(m_error);
        }

        auto
        error() const && noexcept -> const E &&
        {
            return std::move(m_error);
        }

      private:
        E m_error;
    };

    // Tag type for constructing unexpected values in a rtl::expected
    struct unexpect_t
    {
        explicit unexpect_t() = default;
    };

    // Tag for constructing unexpected values
    constexpr unexpect_t unexpect{};

    namespace __expected_detail
    {
        template <typename T>
        constexpr bool __is_expected = false;
        template <typename T, typename E>
        constexpr bool __is_expected<expected<T, E>> = true;

        template <typename E>
        constexpr bool __is_unexpected = false;
        template <typename E>
        constexpr bool __is_unexpected<unexpected<E>> = true;

        // TODO: results for transform

        template <typename E>
        constexpr bool __can_be_unexpected
            = rtl::is_object_v<E> && (!rtl::is_array_v<E>) && (!__is_unexpected<E>)
              && (!rtl::is_const_v<E>) && (!rtl::is_volatile_v<E>);

        struct __in_place_inv
        {
        };
        struct __unexpect_inv
        {
        };
    } // __expected_detail

    template <typename E>
    class unexpected
    {
        static_assert(__expected_detail::__can_be_unexpected<E>, "");

      public:
        unexpected(const unexpected &) = default;
        unexpected(unexpected &&)      = default;

        template <typename Err = E,
                  typename
                  = std::enable_if_t<!rtl::is_same_v<rtl::remove_cvref_t<E>, unexpected>
                                     && !rtl::is_same_v<rtl::remove_cvref_t<E>, rtl::in_place_t>
                                     && rtl::is_constructible_v<E, Err>>>
        explicit unexpected(Err &&e) noexcept(rtl::is_nothrow_constructible_v<E, Err>)
            : m_error(std::forward<Err>(e))
        {
        }

        template <typename... Args,
                  typename = std::enable_if_t<rtl::is_constructible_v<E, Args...>>>
        explicit unexpected(rtl::in_place_t,
                            Args &&...args) noexcept(rtl::is_nothrow_constructible_v<E, Args...>)
            : m_error(std::forward<Args>(args)...)
        {
        }

        template <typename U, typename... Args,
                  typename = std::enable_if_t<
                      rtl::is_constructible_v<E, std::initializer_list<U> &, Args...>>>
        explicit unexpected(rtl::in_place_t, std::initializer_list<U> il, Args &&...args) noexcept(
            rtl::is_nothrow_constructible_v<E, std::initializer_list<U> &, Args...>)
            : m_error(il, std::forward<Args>(args)...)
        {
        }

        auto operator=(const unexpected &) noexcept -> unexpected & = default;
        auto operator=(unexpected &&) noexcept -> unexpected &      = default;

        auto
        error() const & noexcept -> const E &
        {
            return m_error;
        }
        auto
        error() & noexcept -> E &
        {
            return m_error;
        }
        auto
        error() const && noexcept -> const E &&
        {
            return std::move(m_error);
        }
        auto
        error() && noexcept -> E &&
        {
            return std::move(m_error);
        }

        // TODO: swap

        template <typename U>
        friend auto
        operator==(const unexpected &lhs, const unexpected<U> &rhs) noexcept -> bool
        {
            return lhs.m_error == rhs.error();
        }

      private:
        E m_error;
    };

#ifdef CXX_LIVE_MIN_VERSION_17
    template <typename E>
    unexpected(E) -> unexpected<E>;
#endif // CXX_LIVE_MIN_VERSION_17

    namespace __expected_detail
    {
        template <typename T, typename U, typename V>
        auto
        __reinit(T *new_val, U *old_val, V &&arg) noexcept
            -> std::enable_if_t<rtl::is_nothrow_constructible_v<T, V>>
        {
            rtl::destroy_at(old_val);
        rtl:
            construct_at(new_val, std::forward<V>(arg));
        }

        template <typename T, typename U, typename V>
        auto
        __reinit(T *new_val, U *old_val, V &&arg) noexcept(false)
            -> std::enable_if_t<!rtl::is_nothrow_constructible_v<T, V>
                                && rtl::is_nothrow_move_constructible_v<T>>
        {
            T tmp(std::forward<V>(arg));
            rtl::destroy_at(old_val);
        rtl:
            construct_at(new_val, std::move(tmp));
        }

        template <typename T, typename U, typename V>
        auto
        __reinit(T *new_val, U *old_val, V &&arg) noexcept(false)
            -> std::enable_if_t<!rtl::is_nothrow_constructible_v<T, V>
                                && !rtl::is_nothrow_move_constructible_v<T>>
        {
            // TODO: use guard class?
        }

        // If T is cv bool, remove_cvref<U> is not a specialization of expected
        template <typename T, typename U>
        constexpr bool __not_constructing_bool_from_expected
            = !rtl::is_same_v<rtl::remove_cvref_t<T>, bool>
              || !__is_expected<rtl::remove_cvref_t<U>>;

        template <typename T, typename U = rtl::remove_cvref_t<T>>
        constexpr bool __trivially_replaceable
            = rtl::is_trivially_constructible_v<U, T> && rtl::is_trivially_assignable_v<U &, T>
              && rtl::is_trivially_destructible_v<U>;

        template <typename T, typename U = rtl::remove_cvref_t<T>>
        constexpr bool __usable_for_assign
            = rtl::is_constructible_v<U, T> && rtl::is_assignable_v<U &, T>;

        template <typename T, typename U = rtl::remove_cvref_t<T>>
        constexpr bool __usable_for_trivial_assign
            = __trivially_replaceable<T, U> && __usable_for_assign<T, U>;

        template <typename T, typename E>
        constexpr bool __can_reassign_type
            = rtl::is_nothrow_move_constructible_v<T> || rtl::is_nothrow_move_assignable_v<E>;

        /// Template specializations for policy-based construction behavior:

        // Storage layer
        template <typename T, typename E>
        struct __expected_storage_base
        {
            // NOLINTNEXTLINE -> initialization of members is handled deeper in the inheritance
            // hierarchy
            __expected_storage_base() noexcept {}
            ~__expected_storage_base() noexcept {}
            union
            {
                std::remove_cv_t<T> m_value;
                E m_error;
            };
            bool m_has_value;

            template <typename U>
            void
            _assign_value(U &&value)
            {
                if ( this->m_has_value )
                    construct_at(std::addressof(this->m_value), std::forward<U>(value));
                else
                    {
                        __reinit(std::addressof(this->m_value), std::addressof(this->m_error),
                                 std::forward<U>(value));
                        this->m_has_value = true;
                    }
            }

            template <typename V>
            void
            _assign_error(V &&err)
            {
                if ( this->m_has_value )
                    {
                        __reinit(std::addressof(this->m_error), std::addressof(this->m_value),
                                 std::forward<V>(err));
                        this->m_has_value = false;
                    }
                else
                    construct_at(std::addressof(this->m_error), std::forward<V>(err));
            }
        };

        // Destructor Layer
        template <typename T, typename E,
                  bool = rtl::is_trivially_destructible_v<T> && rtl::is_trivially_destructible_v<E>>
        struct __expected_destructible_base : __expected_storage_base<T, E>
        {
            ~__expected_destructible_base() = default;
        };

        template <typename T, typename E>
        struct __expected_destructible_base<T, E, false> : __expected_storage_base<T, E>
        {
            ~__expected_destructible_base()
            {
                if ( this->m_has_value )
                    rtl::destroy_at(std::addressof(this->m_value));
                else
                    rtl::destroy_at(std::addressof(this->m_error));
            }
        };

        // Default constructor layer
        template <typename T, typename E, bool = is_default_constructible_v<T>>
        struct __expected_default_base : __expected_destructible_base<T, E>
        {
            ~__expected_default_base() = default;
            __expected_default_base() noexcept(is_nothrow_default_constructible_v<T>)
            {
                rtl::construct_at(std::addressof(this->m_value));
                this->m_has_value = true;
            }
        };

        template <typename T, typename E>
        struct __expected_default_base<T, E, false> : __expected_destructible_base<T, E>
        {
            ~__expected_default_base() = default;
            __expected_default_base()  = default; // Defaults this to __expected_storage_base
                                                  // default constructor which is a no-op
        };

        // Copy layer: default
        template <typename T, typename E,
                  bool = rtl::is_trivially_copy_constructible_v<T>
                         && rtl::is_trivially_copy_constructible_v<E>>
        struct __expected_copy_base : __expected_default_base<T, E>
        {
            __expected_copy_base()                             = default;
            __expected_copy_base(const __expected_copy_base &) = default;
            ~__expected_copy_base()                            = default;
        };

        // Copy layer: non-trivial
        template <typename T, typename E>
        struct __expected_copy_base<T, E, false> : __expected_default_base<T, E>
        {
            __expected_copy_base()                        = default;
            __expected_copy_base(__expected_copy_base &&) = default;
            ~__expected_copy_base()                       = default;

            __expected_copy_base(const __expected_copy_base &other) noexcept(
                rtl::is_nothrow_copy_constructible_v<T> && rtl::is_nothrow_copy_constructible_v<E>)
            {
                this->m_has_value = other.m_has_value;
                if ( this->m_has_value )
                    rtl::construct_at(std::addressof(this->m_value), other.m_value);
                else
                    rtl::construct_at(std::addressof(this->m_error), other.m_error);
            }
        };

        // Move layer: default
        template <typename T, typename E,
                  bool = rtl::is_trivially_move_constructible_v<T>
                         && rtl::is_trivially_move_constructible_v<E>>
        struct __expected_move_base : __expected_copy_base<T, E>
        {
            __expected_move_base()                             = default;
            __expected_move_base(const __expected_move_base &) = default;
            __expected_move_base(__expected_move_base &&)      = default;
            ~__expected_move_base()                            = default;
        };

        // Move layer: non-trivial
        template <typename T, typename E>
        struct __expected_move_base<T, E, false> : __expected_copy_base<T, E>
        {
            __expected_move_base()                             = default;
            __expected_move_base(const __expected_move_base &) = default;
            ~__expected_move_base()                            = default;

            __expected_move_base(__expected_move_base &&other) noexcept(
                rtl::is_nothrow_move_constructible_v<T> && rtl::is_nothrow_move_constructible_v<E>)
            {
                this->m_has_value = other.m_has_value;
                if ( this->m_has_value )
                    rtl::construct_at(std::addressof(this->m_value), std::move(other.m_value));
                else
                    rtl::construct_at(std::addressof(this->m_error), std::move(other.m_error));
            }
        };

        template <typename T, typename E,
                  bool = rtl::is_trivially_copy_assignable_v<T>
                         && rtl::is_trivially_copy_assignable_v<E>>
        struct __expected_copy_assign_base : __expected_move_base<T, E>
        {
            __expected_copy_assign_base()                                    = default;
            ~__expected_copy_assign_base()                                   = default;
            __expected_copy_assign_base(__expected_copy_assign_base &&)      = default;
            __expected_copy_assign_base(const __expected_copy_assign_base &) = default;

            __expected_copy_assign_base &operator=(const __expected_copy_assign_base &) = default;
        };

        template <typename T, typename E>
        struct __expected_copy_assign_base<T, E, false> : __expected_move_base<T, E>
        {
            __expected_copy_assign_base()                                    = default;
            ~__expected_copy_assign_base()                                   = default;
            __expected_copy_assign_base(__expected_copy_assign_base &&)      = default;
            __expected_copy_assign_base(const __expected_copy_assign_base &) = default;

            __expected_copy_assign_base &
            operator=(const __expected_copy_assign_base &other)
            {
                if ( other.m_has_value )
                    this->_assign_value(other.m_value);
                else
                    this->_assign_error(other.m_error);

                return *this;
            }
        };

        template <typename T, typename E,
                  bool = rtl::is_trivially_move_assignable_v<T>
                         && rtl::is_trivially_move_assignable_v<E>>
        struct __expected_move_assign_base : __expected_copy_assign_base<T, E>
        {
            __expected_move_assign_base()                                               = default;
            ~__expected_move_assign_base()                                              = default;
            __expected_move_assign_base(__expected_move_assign_base &&)                 = default;
            __expected_move_assign_base(const __expected_move_assign_base &)            = default;
            __expected_move_assign_base &operator=(const __expected_move_assign_base &) = default;
            __expected_move_assign_base &operator=(__expected_move_assign_base &&)      = default;
        };

        template <typename T, typename E>
        struct __expected_move_assign_base<T, E, false> : __expected_copy_assign_base<T, E>
        {
            __expected_move_assign_base()                                               = default;
            ~__expected_move_assign_base()                                              = default;
            __expected_move_assign_base(__expected_move_assign_base &&)                 = default;
            __expected_move_assign_base(const __expected_move_assign_base &)            = default;
            __expected_move_assign_base &operator=(const __expected_move_assign_base &) = default;

            __expected_move_assign_base &
            operator=(__expected_move_assign_base &&other)
            {
                if ( other.m_has_value )
                    this->_assign_value(std::move(other.m_value));
                else
                    this->_assign_value(std::move(other.m_error));

                return *this;
            }
        };

        // Copy delete layer
        template <typename T, typename E,
                  bool = rtl::is_copy_constructible_v<T> && rtl::is_copy_constructible_v<E>>
        struct __expected_copy_delete_base : __expected_copy_assign_base<T, E>
        {
            __expected_copy_delete_base()                                    = default;
            __expected_copy_delete_base(__expected_copy_delete_base &&)      = default;
            __expected_copy_delete_base(const __expected_copy_delete_base &) = delete;
            ~__expected_copy_delete_base()                                   = default;

            __expected_copy_delete_base &operator=(const __expected_copy_delete_base &) = default;
        };

        // Copy delete layer: allow copyable
        template <typename T, typename E>
        struct __expected_copy_delete_base<T, E, true> : __expected_copy_assign_base<T, E>
        {
            __expected_copy_delete_base()                                    = default;
            __expected_copy_delete_base(const __expected_copy_delete_base &) = default;
            ~__expected_copy_delete_base()                                   = default;

            __expected_copy_delete_base &operator=(const __expected_copy_delete_base &) = default;
        };

        // Move delete layer: not-movable
        template <typename T, typename E,
                  bool = rtl::is_move_constructible_v<T> && rtl::is_move_constructible_v<E>>
        struct __expected_move_delete_base : __expected_copy_delete_base<T, E>
        {
            __expected_move_delete_base()                                               = default;
            __expected_move_delete_base(const __expected_move_delete_base &)            = default;
            __expected_move_delete_base(__expected_move_delete_base &&)                 = delete;
            ~__expected_move_delete_base()                                              = default;
            __expected_move_delete_base &operator=(const __expected_move_delete_base &) = default;
        };

        // Move delete layer: is-movable
        template <typename T, typename E>
        struct __expected_move_delete_base<T, E, true> : __expected_copy_delete_base<T, E>
        {
            __expected_move_delete_base()                                               = default;
            __expected_move_delete_base(const __expected_move_delete_base &)            = default;
            __expected_move_delete_base(__expected_move_delete_base &&)                 = default;
            ~__expected_move_delete_base()                                              = default;
            __expected_move_delete_base &operator=(const __expected_move_delete_base &) = default;
        };
    } // namespace __expected_detail

    template <typename T, typename E>
    class expected : __expected_detail::__expected_move_delete_base<T, E>
    {
        // Restore inherited constructors
      public:
        expected()                            = default;
        expected(const expected &)            = default;
        expected(expected &&)                 = default;
        ~expected()                           = default;
        expected &operator=(const expected &) = default;

        using value_type = T;
        using error_type = E;

        // Conversion constructors
      public:
        // Convert from value
        template <typename U = std::remove_cv_t<T>,
                  typename   = std::enable_if_t<!rtl::is_same_v<U, expected>
                                                && !rtl::is_same_v<U, rtl::in_place_t>
                                                && rtl::is_constructible_v<T, U>>>
        expected(U &&value) noexcept(rtl::is_nothrow_constructible_v<T, U>)
        {
            construct_at(std::addressof(this->m_value), std::forward<U>(value));
            // this->m_value = std::forward<U>(value);
            this->m_has_value = true;
        }

        // Convert from unexpected
        template <typename G = E,
                  typename   = std::enable_if_t<rtl::is_constructible_v<E, const G &>
                                                && !rtl::is_same_v<G, expected>
                                                && !rtl::is_same_v<G, rtl::in_place_t>>>
        expected(const unexpected<G> &unex) noexcept(rtl::is_nothrow_constructible_v<E, G>)
        {
            rtl::construct_at(std::addressof(this->m_error), unex.error());
            this->m_has_value = false;
        }

        template <typename G = E,
                  typename
                  = std::enable_if_t<rtl::is_constructible_v<E, G> && !rtl::is_same_v<G, expected>
                                     && !rtl::is_same_v<G, rtl::in_place_t>>>
        expected(unexpected<G> &&unex) noexcept(rtl::is_nothrow_constructible_v<E, G>)
        {
            rtl::construct_at(std::addressof(this->m_error), std::move(unex.error()));
            this->m_has_value = false;
        }

        // Construct with in_place_t
        template <typename... Args, typename = std::enable_if_t<is_constructible_v<T, Args...>>>
        expected(in_place_t, Args &&...args) noexcept(is_nothrow_constructible_v<T, Args...>)
        {
            construct_at(std::addressof(this->m_value), std::forward<Args>(args)...);
            this->m_has_value = true;
        }

        // Construct in_place with initializer list
        template <typename U, typename... Args,
                  typename
                  = std::enable_if_t<is_constructible_v<T, std::initializer_list<U> &, Args...>>>
        expected(in_place_t, std::initializer_list<U> &init_list, Args &&...args) noexcept(
            is_nothrow_constructible_v<T, std::initializer_list<U> &, Args...>)
        {
            construct_at(std::addressof(this->m_value), init_list, std::forward<Args>(args)...);
            this->m_has_value = true;
        }

        // Construct error in place with unexpect_t
        template <typename... Args, typename = std::enable_if_t<is_constructible_v<E, Args...>>>
        expected(unexpect_t, Args &&...args) noexcept(is_nothrow_constructible_v<E, Args...>)
        {
            construct_at(std::addressof(this->m_error), std::forward<Args>(args)...);
            this->m_has_value = false;
        }

        // Construct unexpected value in place with initializer list
        template <typename U, typename... Args,
                  typename
                  = std::enable_if_t<is_constructible_v<E, std::initializer_list<U> &, Args...>>>
        expected(unexpect_t, std::initializer_list<U> &init_list, Args &&...args) noexcept(
            is_nothrow_constructible_v<E, std::initializer_list<U> &, Args...>)
        {
            construct_at(std::addressof(this->m_error), init_list, std::forward<Args>(args)...);
            this->m_has_value = false;
        }

        // Assignment from value
        template <typename U,
                  typename = std::enable_if_t<
                      !is_same_v<remove_cvref_t<U>, expected>
                      && !__expected_detail::__is_unexpected<remove_cvref_t<U>>
                      && is_constructible_v<T, U> && !is_assignable_v<T &, U>
                      && (is_nothrow_constructible_v<T, U> || is_nothrow_move_constructible_v<T>
                          || is_nothrow_move_constructible_v<E>)>>
        expected &
        operator=(U &&value)
        {
            this->_assign_value(std::forward<U>(value));
            return *this;
        }

        template <
            typename G,
            typename = std::enable_if_t<
                is_constructible_v<E, const G &> && is_assignable_v<E &, const G &>
                && (is_nothrow_constructible_v<E, const G &> || is_nothrow_move_constructible_v<T>
                    || is_nothrow_move_constructible_v<E>)>>
        expected &
        operator=(const unexpected<G> &unex)
        {
            this->_assign_error(unex.error());
            return *this;
        }

        template <
            typename G,
            typename = std::enable_if_t<
                is_constructible_v<E, const G &> && is_assignable_v<E &, const G &>
                && (is_nothrow_constructible_v<E, const G &> || is_nothrow_move_constructible_v<T>
                    || is_nothrow_move_constructible_v<E>)>>
        expected &
        operator=(unexpected<G> &&unex)
        {
            this->_assign_error(std::move(unex.error()));
            return *this;
        }

        /* Modifiers */

        template <typename... Args>
        T &
        emplace(Args &&...args) noexcept
        {
            if ( this->m_has_value )
                destroy_at(std::addressof(this->m_value));
            else
                {
                    destroy_at(std::addressof(this->m_error));
                    this->m_has_value = true;
                }

            construct_at(std::addressof(this->m_value), std::forward<Args>(args)...);
            return this->m_value;
        }

        /* Observers */
        const T *
        operator->() const noexcept
        {
            assert(this->m_has_value);
            return std::addressof(this->m_value);
        }

        T *
        operator->() noexcept
        {
            assert(this->m_has_value);
            return std::addressof(this->m_value);
        }

        const T &
        operator*() const & noexcept
        {
            assert(this->m_has_value);
            return this->m_value;
        }

        T &
        operator*() & noexcept
        {
            assert(this->m_has_value);
            return this->m_value;
        }

        const T &&
        operator*() const && noexcept
        {
            assert(this->m_has_value);
            return std::move(this->m_value);
        }

        T &&
        operator*() && noexcept
        {
            assert(this->m_has_value);
            return std::move(this->m_value);
        }

        explicit
        operator bool() const noexcept
        {
            return this->m_has_value;
        }

        bool
        has_value() const noexcept
        {
            return this->m_has_value;
        }

        const T &
        value() const &
        {
            static_assert(is_copy_constructible_v<E>, "");
            if ( this->m_has_value )
                return this->m_value;

            throw bad_expected_access<E>(this->m_error);
        }

        T &
        value() &
        {
            static_assert(is_copy_constructible_v<E>, "");
            if ( this->m_has_value )
                return this->m_value;

            // TODO: use as_const<E>?
            const auto &__unex = this->m_error;
            throw bad_expected_access<E>(__unex);
        }

        const T &&
        value() const &&
        {
            static_assert(is_copy_constructible_v<E>, "");
            static_assert(is_constructible_v<E, const E &&>, "");

            if ( this->m_has_value )
                return std::move(this->m_value);

            throw bad_expected_access<E>(std::move(this->m_error));
        }

        T &&
        value() &&
        {
            static_assert(is_copy_constructible_v<E>, "");
            static_assert(is_constructible_v<E, const E &&>, "");

            if ( this->m_has_value )
                return std::move(this->m_value);

            throw bad_expected_access<E>(std::move(this->m_error));
        }

        const E &
        error() const & noexcept
        {
            assert(!this->m_has_value);
            return this->m_error;
        }

        E &
        error() & noexcept
        {
            assert(!this->m_has_value);
            return this->m_error;
        }

        const E &&
        error() const && noexcept
        {
            assert(!this->m_has_value);
            return std::move(this->m_error);
        }

        E &&
        error() && noexcept
        {
            assert(!this->m_has_value);
            return std::move(this->m_error);
        }

        template <typename U = std::remove_cv_t<T>>
        constexpr std::remove_cv_t<T>
        value_or(U &&u) const & noexcept(is_nothrow_copy_constructible_v<T>
                                         && is_nothrow_convertible_v<U, T>)
        {
            using X = std::remove_cv_t<T>;
            static_assert(is_convertible_v<const T &, X>, "");
            static_assert(is_convertible_v<U, T>, "");

            if ( this->m_has_value )
                return this->m_value;

            return std::forward<U>(u);
        }

        template <typename U = std::remove_cv_t<T>>
        constexpr std::remove_cv_t<T>
        value_or(U &&u) && noexcept(is_nothrow_copy_constructible_v<T>
                                    && is_nothrow_convertible_v<U, T>)
        {
            using X = std::remove_cv_t<T>;
            static_assert(is_convertible_v<T, X>, "");
            static_assert(is_convertible_v<U, X>, "");

            if ( this->m_has_value )
                return std::move(this->m_value);

            return std::forward<U>(u);
        }

        template <typename G = E>
        E
        error_or(G &&err) const &
        {
            static_assert(is_copy_constructible_v<E>, "");
            static_assert(is_convertible_v<G, E>, "");

            if ( this->m_has_value )
                return std::forward<G>(err);
            return this->m_error;
        }

        template <typename G = E>
        E
        error_or(G &&err) &&
        {
            static_assert(is_move_constructible_v<E>, "");
            static_assert(is_convertible_v<G, E>, "");

            if ( this->m_has_value )
                return std::forward<G>(err);
            return std::move(this->m_error);
        }

        // Equality Operators
    };
#endif // CXX_LIVE_MIN_VERSION_23
} // namespace rtl

#endif // __RTL_EXPECTED_H
