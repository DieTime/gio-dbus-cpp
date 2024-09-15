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

    ~ConnectionImpl();

    void acquire_name(const std::string &name,
                      std::function<void(const std::string &)> on_name_acquired,
                      std::function<void(const std::string &)> on_name_lost);

    const std::string &unique_name() const noexcept;
    const std::string &name() const noexcept;

    GDBusConnection *as_gio_connection() const;

private:
    void setup_unique_name_with_connection(GDBusConnection *connection);

    static void on_connection_name_acquired(GDBusConnection *, const char *name, void *user_data);
    static void on_connection_name_lost(GDBusConnection *, const char *name, void *user_data);

    std::string m_unique_name;
    std::string m_name;
    unsigned int m_name_acquire_id;
    std::function<void(const std::string &)> m_on_name_acquired;
    std::function<void(const std::string &)> m_on_name_lost;
    std::unique_ptr<GDBusConnection, decltype(&g_object_unref)> m_connection;
};

ConnectionImpl::ConnectionImpl(ConnectionType connection_type)
    : m_name_acquire_id(0)
    , m_connection(nullptr, &g_object_unref)
{
    GError *_error = nullptr;
    GDBusConnection *connection = g_bus_get_sync(connection_type == ConnectionType::System
                                                     ? G_BUS_TYPE_SYSTEM
                                                     : G_BUS_TYPE_SESSION,
                                                 nullptr,
                                                 &_error);

    std::unique_ptr<GError, decltype(&g_error_free)> error(_error, &g_error_free);

    if (error) {
        GIO_DBUS_CPP_THROW_ERROR(std::string("Failed to create ")
                                 + connection_type_to_string(connection_type) + " dbus connection ("
                                 + error->message + ")");
    }

    setup_unique_name_with_connection(connection);
}

ConnectionImpl::ConnectionImpl(const std::string &address)
    : m_name_acquire_id(0)
    , m_connection(nullptr, &g_object_unref)
{
    GError *_error = nullptr;
    GDBusConnection *connection = g_dbus_connection_new_for_address_sync(address.data(),
                                                                         G_DBUS_CONNECTION_FLAGS_NONE,
                                                                         nullptr,
                                                                         nullptr,
                                                                         &_error);

    std::unique_ptr<GError, decltype(&g_error_free)> error(_error, &g_error_free);

    if (error) {
        GIO_DBUS_CPP_THROW_ERROR(std::string("Failed to create dbus connection for ") + address
                                 + " address " + "(" + error->message + ")");
    }

    setup_unique_name_with_connection(connection);
}

ConnectionImpl::~ConnectionImpl()
{
    if (m_name_acquire_id) {
        g_bus_unown_name(m_name_acquire_id);

        if (!m_name.empty() && m_on_name_lost) {
            m_on_name_lost(m_name);
        }
    }
}

void ConnectionImpl::acquire_name(const std::string &name,
                                  std::function<void(const std::string &)> on_name_acquired,
                                  std::function<void(const std::string &)> on_name_lost)
{
    m_on_name_acquired = std::move(on_name_acquired);
    m_on_name_lost = std::move(on_name_lost);
    m_name_acquire_id = g_bus_own_name_on_connection(m_connection.get(),
                                                     name.c_str(),
                                                     G_BUS_NAME_OWNER_FLAGS_NONE,
                                                     on_connection_name_acquired,
                                                     on_connection_name_lost,
                                                     this,
                                                     nullptr);
}

void ConnectionImpl::setup_unique_name_with_connection(GDBusConnection *connection)
{
    const char *unique_name = g_dbus_connection_get_unique_name(connection);

    if (!unique_name) {
        GIO_DBUS_CPP_THROW_ERROR("Failed to get unique name of the created dbus connection");
    }

    m_unique_name = unique_name;
    m_connection.reset(connection);
}

void ConnectionImpl::on_connection_name_acquired(GDBusConnection *,
                                                 const char *name,
                                                 void *user_data)
{
    ConnectionImpl *connection_impl = reinterpret_cast<ConnectionImpl *>(user_data);
    connection_impl->m_name = name;

    if (connection_impl->m_on_name_acquired) {
        connection_impl->m_on_name_acquired(name);
    }
}

void ConnectionImpl::on_connection_name_lost(GDBusConnection *, const char *name, void *user_data)
{
    ConnectionImpl *connection_impl = reinterpret_cast<ConnectionImpl *>(user_data);
    connection_impl->m_name = "";

    if (connection_impl->m_on_name_lost) {
        connection_impl->m_on_name_lost(name);
    }
}

const std::string &ConnectionImpl::unique_name() const noexcept
{
    return m_unique_name;
}

const std::string &ConnectionImpl::name() const noexcept
{
    if (m_name.empty()) {
        return m_unique_name;
    }

    return m_name;
}

GDBusConnection *ConnectionImpl::as_gio_connection() const
{
    return m_connection.get();
}

Connection::Connection(ConnectionType connection_type) noexcept
    : m_pimpl([connection_type] { return std::make_unique<ConnectionImpl>(connection_type); })
{}

Connection::Connection(std::string address) noexcept
    : m_pimpl([address = std::move(address)]() mutable {
        return std::make_unique<ConnectionImpl>(address);
    })
{}

Connection::~Connection() = default;

void Connection::acquire_name(const std::string &name,
                              std::function<void(const std::string &)> on_name_acquired,
                              std::function<void(const std::string &)> on_name_lost)
{
    m_pimpl->acquire_name(name, std::move(on_name_acquired), std::move(on_name_lost));
}

const std::string &Connection::unique_name() const
{
    return m_pimpl->unique_name();
}

const std::string &Connection::name() const
{
    return m_pimpl->name();
}

GDBusConnection *Connection::as_gio_connection() const
{
    return m_pimpl->as_gio_connection();
}

} /* namespace Gio::DBus */
