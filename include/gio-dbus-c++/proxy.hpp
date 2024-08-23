#ifndef GIO_DBUS_CPP_PROXY_HPP
#define GIO_DBUS_CPP_PROXY_HPP

#include "common.hpp"
#include "error.hpp"
#include "message.hpp"
#include "timeout.hpp"

#include <functional>
#include <memory>
#include <string>

namespace Gio::DBus {

class Connection;

class ProxyImpl;
class GIO_DBUS_CPP_EXPORT Proxy
{
public:
    Proxy(Connection &connection, std::string service, std::string object, std::string interface);
    virtual ~Proxy();

    const std::string &service() const noexcept;
    const std::string &object() const noexcept;
    const std::string &interface() const noexcept;

    Message call(const std::string &method, const Timeout &timeout = Timeout::Default);
    Message call(const std::string &method,
                 const Message &arguments,
                 const Timeout &timeout = Timeout::Default);

    void call_async(const std::string &method,
                    const std::function<void(const Message &)> &on_success,
                    const std::function<void(const Error &)> &on_error,
                    const Timeout &timeout = Timeout::Default);
    void call_async(const std::string &method,
                    const Message &arguments,
                    const std::function<void(const Message &)> &on_success,
                    const std::function<void(const Error &)> &on_error,
                    const Timeout &timeout = Timeout::Default);

private:
    std::unique_ptr<ProxyImpl> m_pimpl;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_PROXY_HPP */
