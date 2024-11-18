#ifndef GIO_DBUS_CPP_CONTEXT_HPP
#define GIO_DBUS_CPP_CONTEXT_HPP

#include "common.hpp"
#include "context-job.hpp"
#include "context-type.hpp"
#include "timeout.hpp"

#include "details/pimpl.hpp"

#include <functional>

namespace Gio {

class ContextImpl;
class GIO_DBUS_CPP_EXPORT_CLASS(Context)
{
    GIO_DBUS_CPP_DECLARE_PIMPL_PARTS(Context, ContextImpl)

public:
    Context(ContextType type);

    void start() const;
    void stop() const;

    void push_thread_default() const;
    void pop_thread_default() const;

    ContextJob add_one_shot_job(const Timeout &timeout, std::function<void()> callback) const;
    ContextJob add_periodic_job(const Timeout &timeout, std::function<void()> callback) const;

    void remove_job(const ContextJob &job) const;
};

} /* namespace Gio */

#endif /* GIO_DBUS_CPP_CONTEXT_HPP */
