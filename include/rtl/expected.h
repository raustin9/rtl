#ifndef __RTL_EXPECTED_H
#define __RTL_EXPECTED_H

#include <exception>
#include <utility>

#include "memory.h"
#include "detail/stl_construct.h"
#include "type_traits.h"
#include "utility.h"

namespace rtl
{
    template <typename T, typename E>
    class expected;

    template <typename E>
    class unexpected;

    template <typename T>
    class bad_expected_access;

    template<>
    class bad_expected_access<void> : public std::exception
    {
    protected:
        bad_expected_access() noexcept = default;
        bad_expected_access(const bad_expected_access&) noexcept = default;

        auto operator=(const bad_expected_access&) noexcept -> bad_expected_access& = default;
        auto operator=(bad_expected_access&&) noexcept -> bad_expected_access& = default;

        ~bad_expected_access() = default;

    public:
        auto what() const noexcept -> const char* override
        {
            return "bad access to rtl::expected without expected value";
        }
    };

    template <typename E>
    class bad_expected_access final : public bad_expected_access<void>
    {
    public:
        explicit bad_expected_access(const E& e) noexcept : m_error{ std::move(e) } {}

        auto error() & noexcept -> E&
        {
            return m_error;
        }

        auto error() const & noexcept -> const E&
        {
            return m_error;
        }

        auto error() && noexcept -> E
        {
            return std::move(m_error);
        }

        auto error() const && noexcept -> const E&&
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
        template <typename T>              constexpr bool __is_expected = false;
        template <typename T, typename E>  constexpr bool __is_expected<expected<T, E>> = true;

        template <typename E> constexpr bool __is_unexpected = false;
        template <typename E> constexpr bool __is_unexpected<unexpected<E>> = true;

        // TODO: results for transform

        template <typename E>
        constexpr bool __can_be_unexpected =
            rtl::is_object_v<E>
            && (!rtl::is_array_v<E>)
            && (!__is_unexpected<E>)
            && (!rtl::is_const_v<E>)
            && (!rtl::is_volatile_v<E>)
        ;

        struct __in_place_inv {};
        struct __unexpect_inv {};
    } // __expected_detail

    template <typename E>
    class unexpected
    {
        static_assert(__expected_detail::__can_be_unexpected<E>, "");

    public:
        unexpected(const unexpected&) = default;
        unexpected(unexpected&&) = default;

        template <
            typename Err = E,
            typename = std::enable_if_t<
                !rtl::is_same_v<rtl::remove_cvref_t<E>, unexpected>
                && !rtl::is_same_v<rtl::remove_cvref_t<E>, rtl::in_place_t>
                && rtl::is_constructible_v<E, Err>
            >
        >
        explicit unexpected(Err&& e) noexcept(rtl::is_nothrow_constructible_v<E, Err>)
            : m_error( std::forward<Err>(e) )
        {}

        template <
            typename ... Args,
            typename = std::enable_if_t<
                rtl::is_constructible_v<E, Args...>
            >
        >
        explicit unexpected(rtl::in_place_t, Args&& ... args)
        noexcept(rtl::is_nothrow_constructible_v<E, Args...>)
            : m_error( std::forward<Args>(args)... )
        {}

        template <
            typename U,
            typename ... Args,
            typename = std::enable_if_t<
                rtl::is_constructible_v<E, std::initializer_list<U>&, Args...>
            >
        >
        explicit unexpected(rtl::in_place_t, std::initializer_list<U> il, Args&& ... args)
        noexcept(rtl::is_nothrow_constructible_v<E, std::initializer_list<U>&, Args...>)
            : m_error( il, std::forward<Args>(args)... )
        {}

        auto operator=(const unexpected&) noexcept -> unexpected& = default;
        auto operator=(unexpected&&) noexcept -> unexpected& = default;

        auto error() const & noexcept  -> const E& { return m_error; }
        auto error()       & noexcept  -> E& { return m_error; }
        auto error() const && noexcept -> const E&& { return std::move(m_error); }
        auto error()       && noexcept -> E&& { return std::move(m_error); }

        // TODO: swap

        template <typename U>
        friend auto operator==(const unexpected& lhs, const unexpected<U>& rhs) noexcept -> bool
        {
            return lhs.m_error == rhs.error();
        }

    private:
        E m_error;
    };

    // template <typename E> unexpected(E) -> unexpected<E>;

    namespace __expected_detail
    {
        template <typename T, typename U, typename V>
        auto __reinit(T* new_val, U* old_val, V&& arg) noexcept
            -> std::enable_if_t<rtl::is_nothrow_constructible_v<T, V>>
        {
            rtl::destroy_at(old_val);
            rtl:construct_at(new_val, std::forward<V>(arg));
        }

        template <typename T, typename U, typename V>
        auto __reinit(T* new_val, U* old_val, V&& arg) noexcept(false)
            -> std::enable_if_t<!rtl::is_nothrow_constructible_v<T, V> && rtl::is_nothrow_move_constructible_v<T>>
        {
            T tmp(std::forward<V>(arg));
            rtl::destroy_at(old_val);
            rtl:construct_at(new_val, std::move(tmp));
        }

        template <typename T, typename U, typename V>
        auto __reinit(T* new_val, U* old_val, V&& arg) noexcept(false)
            -> std::enable_if_t<!rtl::is_nothrow_constructible_v<T, V> && !rtl::is_nothrow_move_constructible_v<T>>
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
            = rtl::is_trivially_constructible_v<U, T>
            && rtl::is_trivially_assignable_v<U&, T>
            && rtl::is_trivially_destructible_v<U>;

        template <typename T, typename U = rtl::remove_cvref_t<T>>
        constexpr bool __usable_for_assign
            = rtl::is_constructible_v<U, T> && rtl::is_assignable_v<U&, T>;

        template <typename T, typename U = rtl::remove_cvref_t<T>>
        constexpr bool __usable_for_trivial_assign
            = __trivially_replaceable<T, U> && __usable_for_assign<T, U>;

        template <typename T, typename E>
        constexpr bool __can_reassign_type
            = rtl::is_nothrow_move_constructible_v<T> || rtl::is_nothrow_move_assignable_v<E>;
    } // namespace __expected_detail

    template <typename T, typename E>
    class expected
    {
        static_assert( ! rtl::is_reference_v<T>, "" );
        static_assert( ! rtl::is_function_v<T>, "" );
        static_assert( ! rtl::is_same_v<std::remove_cv_t<T>, rtl::in_place_t>, "" );
        static_assert( ! rtl::is_same_v<std::remove_cv_t<T>, unexpect_t>, "" );
        static_assert( ! __expected_detail::__is_unexpected<std::remove_cv_t<T>>, "" );
        static_assert( __expected_detail::__can_be_unexpected<E>, "" );

        // If T is not cv bool, converts from any cvref<T, expected<U, G>> and
        // is_constructible<unexpected<E>, cv expected<U, G> ref-qual> are false
        template <typename U,
                  typename G,
                  typename Unex = unexpected<E>,
                  typename = std::remove_cv_t<T>>
        static constexpr bool __cons_from_expected
            = std::is_constructible<T, expected<U, G>&>::value
            || std::is_constructible<T, expected<U, G>>::value
            || std::is_constructible<T, const expected<U, G>&>::value
            || std::is_constructible<T, const expected<U, G>>::value
            || std::is_constructible<expected<U, G>&, T>::value
            || std::is_constructible<expected<U, G>, T>::value
            || std::is_constructible<const expected<U, G>&, T>::value
            || std::is_constructible<const expected<U, G>, T>::value
            || std::is_constructible<Unex, expected<U, G>&>::value
            || std::is_constructible<Unex, expected<U, G>>::value
            || std::is_constructible<Unex, const expected<U, G>&>::value
            || std::is_constructible<Unex, const expected<U, G>>::value
        ;

        template <typename U, typename G, typename Unex>
        static constexpr bool __cons_from_expected<U, G, Unex, bool>
            = std::is_constructible<Unex, expected<U, G>&>::value
            || std::is_constructible<Unex, expected<U, G>>::value
            || std::is_constructible<Unex, const expected<U, G>&>::value
            || std::is_constructible<Unex, const expected<U, G>>::value
        ;

        template <typename U, typename G>
        static constexpr bool __explicit_conv = std::is_convertible<U, T>::value || std::is_convertible<G, T>::value;


    public:

    private:
        union
        {
            std::remove_cv_t<T> m_value;
            E                   m_error;
        };
        bool m_has_value;
    };
} // namespace rtl

#endif // __RTL_EXPECTED_H
