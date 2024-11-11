#include "context.hpp"
#include "details/exception.hpp"

#include <gio/gio.h>
#include <thread>

namespace {

std::unique_ptr<GMainContext, decltype(&g_main_context_unref)> context_by_type(Gio::ContextType type)
{
    if (type == Gio::ContextType::Global) {
        return {g_main_context_default(), [](GMainContext *) {}};
    }

    if (type == Gio::ContextType::ThreadDefault) {
        GMainContext *context = g_main_context_get_thread_default();

        if (!context) {
            GIO_DBUS_CPP_THROW_ERROR("Attempt to get thread default Gio::Context however thread "
                                     "has no default context set");
        }

        return {context, [](GMainContext *) {}};
    }

    if (type == Gio::ContextType::New) {
        return {g_main_context_new(), &g_main_context_unref};
    }

    // -V::547 (PVS: Expression is always true)
    if (type == Gio::ContextType::NewAsThreadDefault) {
        GMainContext *context = g_main_context_new();
        g_main_context_push_thread_default(context);

        const auto cleanup = [](GMainContext *context) {
            g_main_context_pop_thread_default(context);
            g_main_context_unref(context);
        };

        return {context, cleanup};
    }

    GIO_DBUS_CPP_THROW_ERROR("Attempt to create a Gio::Context with an unknown Gio::ContextType");
}

} /* namespace */

namespace Gio {

class ContextImpl
{
public:
    ContextImpl(ContextType type);

    void start() const;
    void stop() const;

    void push_thread_default() const;
    void pop_thread_default() const;

private:
    std::unique_ptr<GMainContext, decltype(&g_main_context_unref)> m_context;
    std::unique_ptr<GMainLoop, decltype(&g_main_loop_unref)> m_main_loop;
};

ContextImpl::ContextImpl(ContextType type)
    : m_context(context_by_type(type))
    , m_main_loop(g_main_loop_new(m_context.get(), false), &g_main_loop_unref)
{
    if (type == ContextType::NewAsThreadDefault) {
        push_thread_default();
    }
}

void ContextImpl::start() const
{
    g_main_loop_run(m_main_loop.get());
}

void ContextImpl::stop() const
{
    g_main_loop_quit(m_main_loop.get());
}

void ContextImpl::push_thread_default() const
{
    g_main_context_push_thread_default(m_context.get());
}

void ContextImpl::pop_thread_default() const
{
    g_main_context_pop_thread_default(m_context.get());
}

GIO_DBUS_CPP_IMPLEMENT_PIMPL_PARTS(Context, ContextImpl)

Context::Context(ContextType type)
    : m_pimpl(std::make_unique<ContextImpl>(type))
{}

void Context::start() const
{
    m_pimpl->start();
}

void Context::stop() const
{
    m_pimpl->stop();
}

void Context::push_thread_default() const
{
    m_pimpl->push_thread_default();
}

void Context::pop_thread_default() const
{
    m_pimpl->pop_thread_default();
}

} /* namespace Gio */
