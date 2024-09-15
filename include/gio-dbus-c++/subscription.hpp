#ifndef GIO_DBUS_CPP_SUBSCRIPTION_HPP
#define GIO_DBUS_CPP_SUBSCRIPTION_HPP

#include "common.hpp"

#include <memory>

namespace Gio::DBus {

class SubscriptionImpl;
class GIO_DBUS_CPP_EXPORT Subscription
{
public:
    Subscription();
    ~Subscription();

    Subscription(const Subscription& other);
    Subscription& operator=(const Subscription& other);

    Subscription(Subscription&& other) noexcept;
    Subscription& operator=(Subscription&& other) noexcept;

    uintptr_t proxy_id() const noexcept;
    size_t id() const noexcept;

private:
    friend class ProxyImpl;
    Subscription(uintptr_t proxy_id, size_t id);

    std::unique_ptr<SubscriptionImpl> m_pimpl;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_SUBSCRIPTION_HPP */
