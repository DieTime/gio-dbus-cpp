#ifndef GIO_DBUS_CPP_SUBSCRIPTION_HPP
#define GIO_DBUS_CPP_SUBSCRIPTION_HPP

#include "common.hpp"

#include "details/pimpl.hpp"

namespace Gio::DBus {

class SubscriptionImpl;
class GIO_DBUS_CPP_EXPORT_CLASS(Subscription)
{
    GIO_DBUS_CPP_DECLARE_PIMPL_PARTS(Subscription, SubscriptionImpl)

public:
    uintptr_t proxy_id() const noexcept;
    size_t id() const noexcept;

private:
    friend class ProxyImpl;
    Subscription(uintptr_t proxy_id, size_t id);
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_SUBSCRIPTION_HPP */
