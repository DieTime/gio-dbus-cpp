#include "proxy.hpp"
#include "connection.hpp"

#include "details/exception.hpp"

#include <gio/gio.h>
#include <optional>

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

    Message call(const std::string &method, const Timeout &timeout);
    Message call(const std::string &method, const Message &arguments, const Timeout &timeout);

    void call_async(const std::string &method,
                    const std::function<void(const Message &)> &on_success,
                    const std::function<void(const Error &)> &on_error,
                    const Timeout &timeout);
    void call_async(const std::string &method,
                    const Message &arguments,
                    const std::function<void(const Message &)> &on_success,
                    const std::function<void(const Error &)> &on_error,
                    const Timeout &timeout);

private:
    static void on_async_call_ready(GObject *object, GAsyncResult *result, gpointer user_data);

    std::string m_service;
    std::string m_object;
    std::string m_interface;
    std::unique_ptr<GDBusProxy, decltype(&g_object_unref)> m_proxy;
};

struct AsyncCallContext
{
    Gio::DBus::ProxyImpl &proxy_impl;
    std::string method;
    std::function<void(const Gio::DBus::Message &)> on_success;
    std::function<void(const Gio::DBus::Error &)> on_error;
};

ProxyImpl::ProxyImpl(Connection &connection,
                     std::string service,
                     std::string object,
                     std::string interface)
    : m_service(std::move(service))
    , m_object(std::move(object))
    , m_interface(std::move(interface))
    , m_proxy(nullptr, &g_object_unref)
{
    GError *_error = nullptr;
    GDBusProxy *_proxy = g_dbus_proxy_new_sync(connection.as_gio_connection(),
                                               G_DBUS_PROXY_FLAGS_NONE,
                                               nullptr,
                                               m_service.data(),
                                               m_object.c_str(),
                                               m_interface.data(),
                                               nullptr,
                                               &_error);

    std::unique_ptr<GError, decltype(&g_error_free)> error(_error, &g_error_free);

    if (error) {
        GIO_DBUS_CPP_THROW_ERROR(std::string("Failed to create proxy for ") + m_service
                                 + " service on " + m_object + " object path on " + m_interface
                                 + " interface (" + error->message + ")");
    }

    m_proxy.reset(_proxy);
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

Message ProxyImpl::call(const std::string &method, const Timeout &timeout)
{
    GError *_error = nullptr;
    GVariant *_variant = g_dbus_proxy_call_sync(m_proxy.get(),
                                                method.data(),
                                                nullptr,
                                                G_DBUS_CALL_FLAGS_NONE,
                                                timeout.milliseconds(),
                                                nullptr,
                                                &_error);

    std::unique_ptr<GError, decltype(&g_error_free)> error(_error, &g_error_free);
    std::unique_ptr<GVariant, decltype(&g_variant_unref)> variant(_variant, &g_variant_unref);

    if (error) {
        GIO_DBUS_CPP_THROW_ERROR(std::string("Failed to call ") + interface() + "." + method
                                 + "() method using proxy for " + service() + " service on "
                                 + object() + " object path (" + error->message + ")");
    }

    return {variant.release()};
}

Message ProxyImpl::call(const std::string &method, const Message &arguments, const Timeout &timeout)
{
    GError *_error = nullptr;
    GVariant *_variant = g_dbus_proxy_call_sync(m_proxy.get(),
                                                method.data(),
                                                arguments.as_gio_variant(),
                                                G_DBUS_CALL_FLAGS_NONE,
                                                timeout.milliseconds(),
                                                nullptr,
                                                &_error);

    std::unique_ptr<GError, decltype(&g_error_free)> error(_error, &g_error_free);
    std::unique_ptr<GVariant, decltype(&g_variant_unref)> variant(_variant, &g_variant_unref);

    if (error) {
        GIO_DBUS_CPP_THROW_ERROR(std::string("Failed to call ") + interface() + "." + method
                                 + "() method using proxy for " + service() + " service on "
                                 + object() + " object path (" + error->message + ")");
    }

    return {variant.release()};
}

void ProxyImpl::call_async(const std::string &method,
                           const std::function<void(const Message &)> &on_success,
                           const std::function<void(const Error &)> &on_error,
                           const Timeout &timeout)
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
                          method,
                          on_success,
                          on_error,
                      });
}

void ProxyImpl::call_async(const std::string &method,
                           const Message &arguments,
                           const std::function<void(const Message &)> &on_success,
                           const std::function<void(const Error &)> &on_error,
                           const Timeout &timeout)
{
    g_dbus_proxy_call(m_proxy.get(),
                      method.data(),
                      arguments.as_gio_variant(),
                      G_DBUS_CALL_FLAGS_NONE,
                      timeout.milliseconds(),
                      nullptr,
                      on_async_call_ready,
                      new AsyncCallContext{
                          *this,
                          method,
                          on_success,
                          on_error,
                      });
}

void ProxyImpl::on_async_call_ready(GObject *object, GAsyncResult *result, gpointer user_data)
{
    GError *_error = nullptr;
    GDBusProxy *_proxy = reinterpret_cast<decltype(_proxy)>(object);
    GVariant *_variant = g_dbus_proxy_call_finish(_proxy, result, &_error);

    std::unique_ptr<GError, decltype(&g_error_free)> error(_error, &g_error_free);
    std::unique_ptr<GVariant, decltype(&g_variant_unref)> variant(_variant, &g_variant_unref);
    std::unique_ptr<AsyncCallContext> context(reinterpret_cast<AsyncCallContext *>(user_data));

    if (error) {
        context->on_error(Gio::DBus::Error(GIO_DBUS_CPP_ERROR_NAME,
                                           std::string("Failed to call ")
                                               + context->proxy_impl.interface() + "."
                                               + context->method + "() method using proxy for "
                                               + context->proxy_impl.service() + " service on "
                                               + context->proxy_impl.object() + " object path ("
                                               + error->message + ")"));
    } else {
        std::optional<Message> message;

        try {
            message = Message(variant.release());
        }
        catch (const Error &error) {
            context->on_error(error);
            return;
        }

        context->on_success(*message);
    }
}

Proxy::Proxy(Connection &connection,
             std::string service,
             std::string object_path,
             std::string interface) noexcept
    : m_pimpl([&connection,
               service = std::move(service),
               object_path = std::move(object_path),
               interface = std::move(interface)]() mutable {
        return std::make_unique<ProxyImpl>(connection,
                                           std::move(service),
                                           std::move(object_path),
                                           std::move(interface));
    })
{}

Proxy::~Proxy() = default;

const std::string &Proxy::service() const
{
    return m_pimpl->service();
}

const std::string &Proxy::object() const
{
    return m_pimpl->object();
}

const std::string &Proxy::interface() const
{
    return m_pimpl->interface();
}

Message Proxy::call(const std::string &method, const Timeout &timeout)
{
    return m_pimpl->call(method, timeout);
}

Message Proxy::call(const std::string &method, const Message &arguments, const Timeout &timeout)
{
    return m_pimpl->call(method, arguments, timeout);
}

void Proxy::call_async(const std::string &method,
                       const std::function<void(const Message &)> &on_success,
                       const std::function<void(const Error &)> &on_error,
                       const Timeout &timeout)
{
    m_pimpl->call_async(method, on_success, on_error, timeout);
}

void Proxy::call_async(const std::string &method,
                       const Message &arguments,
                       const std::function<void(const Message &)> &on_success,
                       const std::function<void(const Error &)> &on_error,
                       const Timeout &timeout)
{
    m_pimpl->call_async(method, arguments, on_success, on_error, timeout);
}

} /* namespace Gio::DBus */
