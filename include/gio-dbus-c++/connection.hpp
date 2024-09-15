#ifndef GIO_DBUS_CPP_CONNECTION_HPP
#define GIO_DBUS_CPP_CONNECTION_HPP

#include "common.hpp"
#include "connection-type.hpp"
#include "gio-types.hpp"

#include <functional>
#include <memory>
#include <string>

namespace Gio::DBus {

class ConnectionImpl;
class GIO_DBUS_CPP_EXPORT Connection
{
public:
    Connection(ConnectionType connection_type);
    Connection(const std::string &address);

    virtual ~Connection();

    void acquire_name(const std::string &name,
                      std::function<void(const std::string &)> on_name_acquired = nullptr,
                      std::function<void(const std::string &)> on_name_lost = nullptr);

    const std::string &unique_name() const noexcept;
    const std::string &name() const noexcept;

private:
    friend class ProxyImpl;
    GDBusConnection *as_gio_connection() const noexcept;

    std::unique_ptr<ConnectionImpl> m_pimpl;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_CONNECTION_HPP */
