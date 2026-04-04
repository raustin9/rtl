#ifndef __RTL_EXPECTED_H
#define __RTL_EXPECTED_H

#include <exception>
#include <utility>

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
    inline constexpr unexpect_t unexpect{};

    namespace __expected_detail
    {
        template <typename T>              constexpr bool __is_expected = false;
        template <typename T, typename E>  constexpr bool __is_expected<expected<T, E>> = true;

        template <typename E> constexpr bool __is_unexpected = false;
        template <typename E> constexpr bool __is_unexpected<unexpected<E>> = true;

        // TODO: results for transform

        template <typename E>
        constexpr bool __can_be_unexpected =
            std::is_object_v<E>
            && (!std::is_array_v<E>)
            && (!__is_unexpected<E>)
            && (!std::is_const_v<E>)
            && (!std::is_volatile_v<E>)
        ;

        struct __in_place_inv {};
        struct __unexpect_inv {};
    } // __expected_detail

    template <typename E>
    class unexpected
    {
        static_assert(__expected_detail::__can_be_unexpected<E>);

    public:
        unexpected(const unexpected&) = default;
        unexpected(unexpected&&) = default;

        template <
            typename Err = E,
            typename = std::enable_if_t<
                !std::is_same_v<std::remove_cvref_t<E>, unexpected>
                && !std::is_same_v<std::remove_cvref_t<E>, std::in_place_t>
                && std::is_constructible_v<E, Err>
            >
        >
        explicit unexpected(Err&& e) noexcept(std::is_nothrow_constructible_v<E, Err>)
            : m_error( std::forward<Err>(e) )
        {}

        template <
            typename ... Args,
            typename = std::enable_if_t<
                std::is_constructible_v<E, Args...>
            >
        >
        explicit unexpected(std::in_place_t, Args&& ... args)
        noexcept(std::is_nothrow_constructible_v<E, Args...>)
            : m_error( std::forward<Args>(args)... )
        {}

        template <
            typename U,
            typename ... Args,
            typename = std::enable_if_t<
                std::is_constructible_v<E, std::initializer_list<U>&, Args...>
            >
        >
        explicit unexpected(std::in_place_t, std::initializer_list<U> il, Args&& ... args)
        noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>&, Args...>)
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

    template <typename E> unexpected(E) -> unexpected<E>;
} // namespace rtl

#endif // __RTL_EXPECTED_H
