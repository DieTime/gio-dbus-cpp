#ifndef GIO_DBUS_CPP_DETAILS_LAZY_PIMPL_HPP
#define GIO_DBUS_CPP_DETAILS_LAZY_PIMPL_HPP

#include <functional>
#include <memory>

#include "compile-time-string.hpp"
#include "exception.hpp"

namespace Gio::DBus::Details {

template<typename T, CompileTimeString TName>
class LazyPimpl
{
public:
    LazyPimpl(std::function<std::unique_ptr<T>()> initializer) noexcept
        : m_initializer(std::move(initializer))
        , m_pointer(nullptr)
    {}

    T *operator->() const
    {
        if (!m_pointer) {
            try {
                m_pointer = m_initializer();
            }
            catch (const std::bad_alloc &error) {
                GIO_DBUS_CPP_THROW_ERROR(std::string("Failed to allocate the ") + TName.data()
                                         + " class")
            }
        }

        return m_pointer.get();
    }

private:
    std::function<std::unique_ptr<T>()> m_initializer;
    mutable std::unique_ptr<T> m_pointer;
};

} /* namespace Gio::DBus::Details */

#define GIO_DBUS_CPP_DECLARE_LAZY_PIMPL_OF_TYPE(T) Details::LazyPimpl<T, #T> m_pimpl

#endif /* GIO_DBUS_CPP_DETAILS_LAZY_PIMPL_HPP */
