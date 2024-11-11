#ifndef GIO_DBUS_CPP_CONTEXT_HPP
#define GIO_DBUS_CPP_CONTEXT_HPP

#include "common.hpp"
#include "context-type.hpp"
#include "timeout.hpp"

#include "details/pimpl.hpp"

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
};

} /* namespace Gio */

#endif /* GIO_DBUS_CPP_CONTEXT_HPP */
