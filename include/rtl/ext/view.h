#ifndef __RTL_EXT_VIEW_H
#define __RTL_EXT_VIEW_H
#include "../type_traits.h"

#include <memory>

namespace rtl
{
    namespace ext
    {
        template <typename T>
        class view
        {
        public:
            using value = T;
            using reference = T&;
            using const_reference = const T&;
            using pointer = T*;
            using const_pointer = const T*;

            /// @brief Create a view<T> from a const reference to a T
            /// @note This is not marked explicit as we wish to be able to implicitly cast to this for function parameters
            view(const_reference value) noexcept
                : m_value{ std::addressof(value) }
            {}

            /// @brief Create a view<T> from a const pointer to a T
            /// @note This is not marked explicit as we wish to be able to implicitly cast to this for function parameters
            view(const_pointer value) noexcept
                : m_value{ value }
            {}

            /// @brief Create a view<T> from a pointer to a T
            /// @note This is not marked explicit as we wish to be able to implicitly cast to this for function parameters
            view(pointer value) noexcept
                : m_value{ value }
            {}

            /// @brief Create a view<T> from a unique_ptr<T>
            /// @note This is not marked explicit as we wish to be able to implicitly cast to this for function parameters
            view(const std::unique_ptr<T>& value) noexcept
                : m_value{ value.get() }
            {}

            // Special Members
        public:
            view(const view&) noexcept = default;
            view(view&&) noexcept = default;
            view& operator=(const view&) noexcept = default;
            view& operator=(view&&) noexcept = default;

            // Observers
        public:
            const_reference operator*() const noexcept
            {
                return ptr();
            }

            const_pointer operator->() const noexcept
            {
                return std::addressof(get());
            }

            const_reference get() const noexcept
            {
                return *m_value;
            }

            const_pointer ptr() const noexcept
            {
                return m_value;
            }

            // TODO: consider removing
            bool operator==(const view& rhs) noexcept
            {
                return get() == rhs.get();
            }

            // TODO: consider removing
            bool operator!=(const view& rhs) noexcept
            {
                return get() != rhs.get();
            }

        private:
            const T* m_value;
        };
    } // namespace ext
} // namespace rtl

#endif // __RTL_EXT_VIEW_H
