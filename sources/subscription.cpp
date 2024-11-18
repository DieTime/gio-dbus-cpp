#include "subscription.hpp"

namespace Gio::DBus {

class SubscriptionImpl
{
public:
    SubscriptionImpl(uint64_t proxy_id, uint64_t id);

    uint64_t proxy_id() const noexcept;
    uint64_t id() const noexcept;

private:
    uint64_t m_proxy_id;
    uint64_t m_id;
};

SubscriptionImpl::SubscriptionImpl(uint64_t proxy_id, uint64_t id)
    : m_proxy_id(proxy_id)
    , m_id(id)
{}

uint64_t SubscriptionImpl::proxy_id() const noexcept
{
    return m_proxy_id;
}

uint64_t SubscriptionImpl::id() const noexcept
{
    return m_id;
}

GIO_DBUS_CPP_IMPLEMENT_PIMPL_PARTS(Subscription, SubscriptionImpl)

Subscription::Subscription(uint64_t proxy_id, uint64_t id)
    : m_pimpl(std::make_unique<SubscriptionImpl>(proxy_id, id))
{}

uint64_t Subscription::proxy_id() const noexcept
{
    return m_pimpl->proxy_id();
}

uint64_t Subscription::id() const noexcept
{
    return m_pimpl->id();
}

} /* namespace Gio::DBus */
