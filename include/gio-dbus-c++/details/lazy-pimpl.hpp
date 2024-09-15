#ifndef GIO_DBUS_CPP_DETAILS_LAZY_PIMPL_HPP
#define GIO_DBUS_CPP_DETAILS_LAZY_PIMPL_HPP

#include <functional>
#include <memory>

namespace Gio::DBus::Details {

template<typename T>
class LazyPimpl
{
public:
    LazyPimpl(std::function<std::unique_ptr<T>()> initializer)
        : m_initializer(std::move(initializer))
        , m_pimpl(nullptr)
    {}

    T *operator->() const
    {
        if (!m_pimpl) {
            m_pimpl = m_initializer();
        }

        return m_pimpl.get();
    }

private:
    std::function<std::unique_ptr<T>()> m_initializer;
    mutable std::unique_ptr<T> m_pimpl;
};

} /* namespace Gio::DBus::Details */

#endif /* GIO_DBUS_CPP_DETAILS_LAZY_PIMPL_HPP */
