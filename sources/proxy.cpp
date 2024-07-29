#include "proxy.hpp"
#include "connection.hpp"

#include "details/exception.hpp"

#include <gio/gio.h>

namespace Gio::DBus {

class ProxyImpl
{
public:
    ProxyImpl(Connection &connection,
              std::string service,
              std::string object,
              std::string interface);

    const std::string &service() const noexcept;
    const std::string &object() const noexcept;
    const std::string &interface() const noexcept;

    Message call(std::string_view method, Timeout timeout);
    void call_async(std::string_view method,
                    std::function<void(const Message &)> on_success,
                    std::function<void(const Error &)> on_error,
                    Timeout timeout);

private:
    std::string m_service;
    std::string m_object;
    std::string m_interface;
    std::unique_ptr<GDBusProxy, decltype(&g_object_unref)> m_proxy;
};

} /* namespace Gio::DBus */

namespace {

struct AsyncCallContext
{
    Gio::DBus::ProxyImpl &proxy_impl;
    std::string method;
    std::function<void(const Gio::DBus::Message &)> on_success;
    std::function<void(const Gio::DBus::Error &)> on_error;
};

void on_async_call_ready(GObject *object, GAsyncResult *result, gpointer user_data)
{
    AsyncCallContext *context = reinterpret_cast<decltype(context)>(user_data);

    GError *error = nullptr;
    GDBusProxy *proxy = reinterpret_cast<decltype(proxy)>(object);

    Gio::DBus::Message message = g_dbus_proxy_call_finish(proxy, result, &error);

    if (error) {
        context->on_error(Gio::DBus::Error(GIO_DBUS_CPP_ERROR_NAME,
                                           std::string("Failed to call ")
                                               + context->proxy_impl.interface() + "."
                                               + context->method + "() method using proxy for "
                                               + context->proxy_impl.service() + " service on "
                                               + context->proxy_impl.object() + " object path ("
                                               + error->message + ")"));
        g_error_free(error);
    } else {
        context->on_success(message);
    }

    delete context;
}

} /* namespace */

namespace Gio::DBus {

ProxyImpl::ProxyImpl(Connection &connection,
                     std::string service,
                     std::string object,
                     std::string interface)
    : m_service(std::move(service))
    , m_object(std::move(object))
    , m_interface(std::move(interface))
    , m_proxy(nullptr, &g_object_unref)
{
    GError *error = nullptr;
    GDBusProxy *proxy = g_dbus_proxy_new_sync(connection.as_gio_connection(),
                                              G_DBUS_PROXY_FLAGS_NONE,
                                              nullptr,
                                              m_service.data(),
                                              m_object.c_str(),
                                              m_interface.data(),
                                              nullptr,
                                              &error);
    if (error) {
        std::string error_message = error->message;
        g_error_free(error);

        THROW_GIO_DBUS_CPP_ERROR(std::string("Failed to create proxy for ") + m_service
                                 + " service on " + m_object + " object path on " + m_interface
                                 + " interface (" + error_message + ")");
    }

    m_proxy.reset(proxy);
}

const std::string &ProxyImpl::service() const noexcept
{
    return m_service;
}

const std::string &ProxyImpl::object() const noexcept
{
    return m_object;
}

const std::string &ProxyImpl::interface() const noexcept
{
    return m_interface;
}

Message ProxyImpl::call(std::string_view method, Timeout timeout)
{
    GError *error = nullptr;
    Message message = g_dbus_proxy_call_sync(m_proxy.get(),
                                             method.data(),
                                             nullptr,
                                             G_DBUS_CALL_FLAGS_NONE,
                                             timeout.milliseconds(),
                                             nullptr,
                                             &error);

    if (error) {
        std::string error_message = error->message;
        g_error_free(error);

        THROW_GIO_DBUS_CPP_ERROR(
            std::string("Failed to call ") + g_dbus_proxy_get_interface_name(m_proxy.get()) + "."
            + method.data() + "() method using proxy for " + g_dbus_proxy_get_name(m_proxy.get())
            + " service on " + g_dbus_proxy_get_object_path(m_proxy.get()) + " object path ("
            + error_message + ")");
    }

    return message;
}

void ProxyImpl::call_async(std::string_view method,
                           std::function<void(const Message &)> on_success,
                           std::function<void(const Error &)> on_error,
                           Timeout timeout)
{
    g_dbus_proxy_call(m_proxy.get(),
                      method.data(),
                      nullptr,
                      G_DBUS_CALL_FLAGS_NONE,
                      timeout.milliseconds(),
                      nullptr,
                      on_async_call_ready,
                      new AsyncCallContext{
                          *this,
                          std::string(method),
                          std::move(on_success),
                          std::move(on_error),
                      });
}

Proxy::Proxy(Connection &connection,
             std::string service,
             std::string object_path,
             std::string interface)
    : m_pimpl(std::make_unique<ProxyImpl>(connection,
                                          std::move(service),
                                          std::move(object_path),
                                          std::move(interface)))
{}

Proxy::~Proxy() = default;

const std::string &Proxy::service() const noexcept
{
    return m_pimpl->service();
}

const std::string &Proxy::object() const noexcept
{
    return m_pimpl->object();
}

const std::string &Proxy::interface() const noexcept
{
    return m_pimpl->interface();
}

Message Proxy::call(std::string_view method, Timeout timeout)
{
    return m_pimpl->call(method, timeout);
}

void Proxy::call_async(std::string_view method,
                       std::function<void(const Message &)> on_success,
                       std::function<void(const Error &)> on_error,
                       Timeout timeout)
{
    m_pimpl->call_async(method, std::move(on_success), std::move(on_error), timeout);
}

} /* namespace Gio::DBus */
