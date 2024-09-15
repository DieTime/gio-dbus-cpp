#ifndef GIO_DBUS_CPP_CONNECTION_HPP
#define GIO_DBUS_CPP_CONNECTION_HPP

#include "common.hpp"
#include "connection-type.hpp"
#include "gio-types.hpp"

#include "details/lazy-pimpl.hpp"

#include <functional>
#include <memory>
#include <string>

namespace Gio::DBus {

class ConnectionImpl;
class GIO_DBUS_CPP_EXPORT Connection
{
public:
    Connection(ConnectionType connection_type) noexcept;
    Connection(std::string address) noexcept;

    virtual ~Connection();

    void acquire_name(const std::string &name,
                      std::function<void(const std::string &)> on_name_acquired = nullptr,
                      std::function<void(const std::string &)> on_name_lost = nullptr);

    const std::string &unique_name() const;
    const std::string &name() const;

private:
    friend class ProxyImpl;
    GDBusConnection *as_gio_connection() const;

    Details::LazyPimpl<ConnectionImpl> m_pimpl;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_CONNECTION_HPP */
