#ifndef GIO_DBUS_CPP_PROXY_HPP
#define GIO_DBUS_CPP_PROXY_HPP

#include "common.hpp"
#include "error.hpp"
#include "message.hpp"
#include "timeout.hpp"

#include "details/lazy-pimpl.hpp"

#include <functional>
#include <memory>
#include <string>

namespace Gio::DBus {

class Connection;

class ProxyImpl;
class GIO_DBUS_CPP_EXPORT Proxy
{
public:
    Proxy(Connection &connection,
          std::string service,
          std::string object,
          std::string interface) noexcept;

    virtual ~Proxy();

    const std::string &service() const;
    const std::string &object() const;
    const std::string &interface() const;

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
    GIO_DBUS_CPP_DECLARE_LAZY_PIMPL_OF_TYPE(Gio::DBus::ProxyImpl);
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_PROXY_HPP */
