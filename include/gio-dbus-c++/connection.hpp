#ifndef GIO_DBUS_CPP_CONNECTION_HPP
#define GIO_DBUS_CPP_CONNECTION_HPP

#include "common.hpp"
#include "connection-type.hpp"
#include "gio-types.hpp"

#include <memory>
#include <string>

namespace Gio::DBus {

class ConnectionImpl;
class GIO_DBUS_CPP_EXPORT Connection
{
public:
    explicit Connection(ConnectionType connection_type);
    explicit Connection(const std::string &address);

    virtual ~Connection();

    const std::string &unique_name() const noexcept;

private:
    friend class ProxyImpl;
    GDBusConnection *as_gio_connection() const noexcept;

    std::unique_ptr<ConnectionImpl> m_pimpl;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_CONNECTION_HPP */
