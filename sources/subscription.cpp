#include "subscription.hpp"

namespace Gio::DBus {

class SubscriptionImpl
{
public:
    SubscriptionImpl(uintptr_t proxy_id, size_t id);

    uintptr_t proxy_id() const noexcept;
    size_t id() const noexcept;

private:
    uintptr_t m_proxy_id;
    size_t m_id;
};

SubscriptionImpl::SubscriptionImpl(uintptr_t proxy_id, size_t id)
    : m_proxy_id(proxy_id)
    , m_id(id)
{}

uintptr_t SubscriptionImpl::proxy_id() const noexcept
{
    return m_proxy_id;
}

size_t SubscriptionImpl::id() const noexcept
{
    return m_id;
}

GIO_DBUS_CPP_IMPLEMENT_PIMPL_PARTS(Subscription, SubscriptionImpl)

Subscription::Subscription(uintptr_t proxy_id, size_t id)
    : m_pimpl(std::make_unique<SubscriptionImpl>(proxy_id, id))
{}

uintptr_t Subscription::proxy_id() const noexcept
{
    return m_pimpl->proxy_id();
}

size_t Subscription::id() const noexcept
{
    return m_pimpl->id();
}

} /* namespace Gio::DBus */
