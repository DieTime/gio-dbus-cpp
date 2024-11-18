#include "context.hpp"
#include "details/exception.hpp"

#include <gio/gio.h>
#include <glib-unix.h>
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

int on_one_shot_job_started(void *userdata)
{
    std::function<void()> *callback = static_cast<std::function<void()> *>(userdata);
    (*callback)();

    return G_SOURCE_REMOVE;
}

int on_periodic_job_started(void *userdata)
{
    std::function<void()> *callback = static_cast<std::function<void()> *>(userdata);
    (*callback)();

    return G_SOURCE_CONTINUE;
}

void on_job_removed(void *userdata)
{
    std::function<void()> *callback = static_cast<std::function<void()> *>(userdata);
    delete callback;
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

    ContextJob add_one_shot_job(const Timeout &timeout, std::function<void()> callback) const;
    ContextJob add_periodic_job(const Timeout &timeout, std::function<void()> callback) const;

    void remove_job(const ContextJob &job) const;

private:
    std::unique_ptr<GMainContext, decltype(&g_main_context_unref)> m_context;
    std::unique_ptr<GMainLoop, decltype(&g_main_loop_unref)> m_main_loop;
};

ContextImpl::ContextImpl(ContextType type)
    : m_context(context_by_type(type))
    , m_main_loop(g_main_loop_new(m_context.get(), false), &g_main_loop_unref)
{}

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

ContextJob ContextImpl::add_one_shot_job(const Timeout &timeout,
                                         std::function<void()> callback) const
{
    GSource *source = g_timeout_source_new(timeout.milliseconds());
    g_source_set_callback(source,
                          on_one_shot_job_started,
                          new std::function<void()>(std::move(callback)),
                          on_job_removed);

    return {g_source_attach(source, m_context.get())};
}

ContextJob ContextImpl::add_periodic_job(const Timeout &timeout,
                                         std::function<void()> callback) const
{
    GSource *source = g_timeout_source_new(timeout.milliseconds());
    g_source_set_callback(source,
                          on_periodic_job_started,
                          new std::function<void()>(std::move(callback)),
                          on_job_removed);

    return {g_source_attach(source, m_context.get())};
}

void ContextImpl::remove_job(const ContextJob &job) const
{
    GSource *source = g_main_context_find_source_by_id(m_context.get(), job.id());

    if (!source) {
        return;
    }

    g_source_destroy(source);
    g_source_unref(source);
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

ContextJob Context::add_one_shot_job(const Timeout &timeout, std::function<void()> callback) const
{
    return m_pimpl->add_one_shot_job(timeout, std::move(callback));
}

ContextJob Context::add_periodic_job(const Timeout &timeout, std::function<void()> callback) const
{
    return m_pimpl->add_periodic_job(timeout, std::move(callback));
}

void Context::remove_job(const ContextJob &job) const
{
    return m_pimpl->remove_job(job);
}

} /* namespace Gio */
