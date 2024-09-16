#ifndef GIO_DBUS_CPP_CONNECTION_HPP
#define GIO_DBUS_CPP_CONNECTION_HPP

#include "common.hpp"
#include "connection-type.hpp"
#include "gio-types.hpp"

#include "details/pimpl.hpp"

#include <functional>
#include <string>

namespace Gio::DBus {

class ConnectionImpl;
class GIO_DBUS_CPP_EXPORT_CLASS(Connection)
{
    GIO_DBUS_CPP_DECLARE_PIMPL_PARTS(Connection, ConnectionImpl)

public:
    Connection(ConnectionType connection_type);
    Connection(const std::string &address);

    void acquire_name(const std::string &name,
                      std::function<void(const std::string &)> on_name_acquired = nullptr,
                      std::function<void(const std::string &)> on_name_lost = nullptr) noexcept;

    const std::string &unique_name() const noexcept;
    const std::string &name() const noexcept;

private:
    friend class ProxyImpl;
    GDBusConnection *as_gio_connection() const noexcept;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_CONNECTION_HPP */
