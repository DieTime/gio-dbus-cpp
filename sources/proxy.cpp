#include "proxy.hpp"
#include "connection.hpp"

#include <gio/gio.h>
#include <iostream>
#include <list>
#include <optional>

namespace {

struct SubscriptionContext
{
    uint64_t subscription_id;
    std::string signal_name;
    std::function<void(const Gio::DBus::Message &)> on_signal_emitted;
};

} /* namespace */

namespace Gio::DBus {

class ProxyImpl
{
public:
    ProxyImpl(Connection &connection,
              std::string service,
              std::string object,
              std::string interface);
    ~ProxyImpl();

    const std::string &service() const noexcept;
    const std::string &object() const noexcept;
    const std::string &interface() const noexcept;

    Message call(const std::string &method, const Timeout &timeout) const;
    Message call(const std::string &method, const Message &arguments, const Timeout &timeout) const;

    void call_async(const std::string &method,
                    const std::function<void(const Message &)> &on_success,
                    const std::function<void(const Error &)> &on_error,
                    const Timeout &timeout) const;
    void call_async(const std::string &method,
                    const Message &arguments,
                    const std::function<void(const Message &)> &on_success,
                    const std::function<void(const Error &)> &on_error,
                    const Timeout &timeout) const;

    Subscription subscribe_to_signal(std::string signal_name,
                                     std::function<void(const Message &)> on_signal_emitted) const;
    void unsubscribe_from_signal(const Subscription &subscription) const;

private:
    static void on_async_call_ready(GObject *, GAsyncResult *, void *);
    static void on_any_signal(GDBusProxy *, const char *, const char *, GVariant *, void *);

    std::string m_service;
    std::string m_object;
    std::string m_interface;
    std::vector<gulong> m_gio_signal_connections;
    mutable uint64_t m_signal_subscriptions_count;
    mutable std::list<SubscriptionContext> m_signal_subscriptions;
    std::unique_ptr<GDBusProxy, decltype(&g_object_unref)> m_proxy;
};

struct AsyncCallContext
{
    const Gio::DBus::ProxyImpl &proxy_impl;
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

    m_gio_signal_connections.push_back(
        g_signal_connect(_proxy, "g-signal", G_CALLBACK(on_any_signal), this));

    m_proxy.reset(_proxy);
}

ProxyImpl::~ProxyImpl()
{
    for (const auto &gio_signal_connection: m_gio_signal_connections) {
        g_signal_handler_disconnect(m_proxy.get(), gio_signal_connection);
    }
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

Message ProxyImpl::call(const std::string &method, const Timeout &timeout) const
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

Message ProxyImpl::call(const std::string &method,
                        const Message &arguments,
                        const Timeout &timeout) const
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
                           const Timeout &timeout) const
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
                           const Timeout &timeout) const
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

Subscription ProxyImpl::subscribe_to_signal(
    std::string signal_name, std::function<void(const Message &)> on_signal_emitted) const
{
    m_signal_subscriptions.push_back({
        m_signal_subscriptions_count,
        std::move(signal_name),
        std::move(on_signal_emitted),
    });

    return {reinterpret_cast<uint64_t>(this), m_signal_subscriptions_count++};
}

void ProxyImpl::unsubscribe_from_signal(const Subscription &subscription) const
{
    if (subscription.proxy_id() != reinterpret_cast<uint64_t>(this)) {
        return;
    }

    m_signal_subscriptions.remove_if([&subscription](const SubscriptionContext &context) {
        return context.subscription_id == subscription.id();
    });
}

void ProxyImpl::on_async_call_ready(GObject *object, GAsyncResult *result, void *user_data)
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

void ProxyImpl::on_any_signal(
    GDBusProxy *, const char *, const char *signal_name, GVariant *parameters, void *user_data)
{
    ProxyImpl *proxy_impl = reinterpret_cast<ProxyImpl *>(user_data);

    for (const auto &subscription_context: proxy_impl->m_signal_subscriptions) {
        if (subscription_context.signal_name == signal_name) {
            subscription_context.on_signal_emitted(parameters);
        }
    }
}

GIO_DBUS_CPP_IMPLEMENT_PIMPL_PARTS(Proxy, ProxyImpl)

Proxy::Proxy(Connection &connection,
             std::string service,
             std::string object_path,
             std::string interface)
    : m_pimpl(std::make_unique<ProxyImpl>(connection,
                                          std::move(service),
                                          std::move(object_path),
                                          std::move(interface)))
{}

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

Message Proxy::call(const std::string &method, const Timeout &timeout) const
{
    return m_pimpl->call(method, timeout);
}

Message Proxy::call(const std::string &method,
                    const Message &arguments,
                    const Timeout &timeout) const
{
    return m_pimpl->call(method, arguments, timeout);
}

void Proxy::call_async(const std::string &method,
                       const std::function<void(const Message &)> &on_success,
                       const std::function<void(const Error &)> &on_error,
                       const Timeout &timeout) const
{
    m_pimpl->call_async(method, on_success, on_error, timeout);
}

void Proxy::call_async(const std::string &method,
                       const Message &arguments,
                       const std::function<void(const Message &)> &on_success,
                       const std::function<void(const Error &)> &on_error,
                       const Timeout &timeout) const
{
    m_pimpl->call_async(method, arguments, on_success, on_error, timeout);
}

Subscription Proxy::subscribe_to_signal(std::string signal_name,
                                        std::function<void(const Message &)> on_signal_emitted) const
{
    return m_pimpl->subscribe_to_signal(std::move(signal_name), std::move(on_signal_emitted));
}

void Proxy::unsubscribe_from_signal(const Subscription &subscription) const
{
    m_pimpl->unsubscribe_from_signal(subscription);
}

} /* namespace Gio::DBus */
