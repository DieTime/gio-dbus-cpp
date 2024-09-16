#ifndef GIO_DBUS_CPP_PROXY_HPP
#define GIO_DBUS_CPP_PROXY_HPP

#include "common.hpp"
#include "error.hpp"
#include "message.hpp"
#include "subscription.hpp"
#include "timeout.hpp"

#include "details/pimpl.hpp"

#include <functional>
#include <string>

namespace Gio::DBus {

class Connection;

class ProxyImpl;
class GIO_DBUS_CPP_EXPORT_CLASS(Proxy)
{
    GIO_DBUS_CPP_DECLARE_PIMPL_PARTS(Proxy, ProxyImpl)

public:
    Proxy(Connection &connection, std::string service, std::string object, std::string interface);

    const std::string &service() const noexcept;
    const std::string &object() const noexcept;
    const std::string &interface() const noexcept;

    Message call(const std::string &method, const Timeout &timeout = Timeout::Default) const;
    Message call(const std::string &method,
                 const Message &arguments,
                 const Timeout &timeout = Timeout::Default) const;

    void call_async(const std::string &method,
                    const std::function<void(const Message &)> &on_success,
                    const std::function<void(const Error &)> &on_error,
                    const Timeout &timeout = Timeout::Default) const;
    void call_async(const std::string &method,
                    const Message &arguments,
                    const std::function<void(const Message &)> &on_success,
                    const std::function<void(const Error &)> &on_error,
                    const Timeout &timeout = Timeout::Default) const;

    Subscription subscribe_to_signal(std::string signal_name,
                                     std::function<void(const Message &)> on_signal_emitted) const;
    void unsubscribe_from_signal(const Subscription &subscription) const;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_PROXY_HPP */
