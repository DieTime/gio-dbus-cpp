#include "connection.hpp"
#include "error.hpp"

#include "details/exception.hpp"

#include <gio/gio.h>

namespace {

const char *connection_type_to_string(Gio::DBus::ConnectionType connection_type) noexcept
{
    switch (connection_type) {
    case Gio::DBus::ConnectionType::System:
        return "system";
    case Gio::DBus::ConnectionType::Session:
        return "session";
    default:
        return "unknown";
    }
}

} /* namespace */

namespace Gio::DBus {

class ConnectionImpl
{
public:
    ConnectionImpl(ConnectionType connection_type);
    ConnectionImpl(const std::string &address);

    const std::string &unique_name() const noexcept;

    GDBusConnection *as_gio_connection() const;

private:
    void setup_unique_name_with_connection(GDBusConnection *connection);

    std::string m_unique_name;
    std::unique_ptr<GDBusConnection, decltype(&g_object_unref)> m_connection;
};

ConnectionImpl::ConnectionImpl(ConnectionType connection_type)
    : m_connection(nullptr, &g_object_unref)
{
    GError *_error = nullptr;
    GDBusConnection *connection = g_bus_get_sync(connection_type == ConnectionType::System
                                                     ? G_BUS_TYPE_SYSTEM
                                                     : G_BUS_TYPE_SESSION,
                                                 nullptr,
                                                 &_error);

    std::unique_ptr<GError, decltype(&g_error_free)> error(_error, &g_error_free);

    if (error) {
        THROW_GIO_DBUS_CPP_ERROR(std::string("Failed to create ")
                                 + connection_type_to_string(connection_type) + " dbus connection ("
                                 + error->message + ")");
    }

    setup_unique_name_with_connection(connection);
}

ConnectionImpl::ConnectionImpl(const std::string &address)
    : m_connection(nullptr, &g_object_unref)
{
    GError *_error = nullptr;
    GDBusConnection *connection = g_dbus_connection_new_for_address_sync(address.data(),
                                                                         G_DBUS_CONNECTION_FLAGS_NONE,
                                                                         nullptr,
                                                                         nullptr,
                                                                         &_error);

    std::unique_ptr<GError, decltype(&g_error_free)> error(_error, &g_error_free);

    if (error) {
        THROW_GIO_DBUS_CPP_ERROR(std::string("Failed to create dbus connection for ") + address
                                 + " address " + "(" + error->message + ")");
    }

    setup_unique_name_with_connection(connection);
}

void ConnectionImpl::setup_unique_name_with_connection(GDBusConnection *connection)
{
    const char *unique_name = g_dbus_connection_get_unique_name(connection);

    if (!unique_name) {
        THROW_GIO_DBUS_CPP_ERROR("Failed to get unique name of the created dbus connection");
    }

    m_unique_name = unique_name;
    m_connection.reset(connection);
}

const std::string &ConnectionImpl::unique_name() const noexcept
{
    return m_unique_name;
}

GDBusConnection *ConnectionImpl::as_gio_connection() const
{
    return m_connection.get();
}

Connection::Connection(ConnectionType connection_type)
    : m_pimpl(std::make_unique<ConnectionImpl>(connection_type))
{}

Connection::Connection(const std::string &address)
    : m_pimpl(std::make_unique<ConnectionImpl>(address))
{}

Connection::~Connection() = default;

const std::string &Connection::unique_name() const noexcept
{
    return m_pimpl->unique_name();
}

GDBusConnection *Connection::as_gio_connection() const noexcept
{
    return m_pimpl->as_gio_connection();
}

} /* namespace Gio::DBus */
