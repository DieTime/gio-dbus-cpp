#ifndef GIO_DBUS_CPP_CONTEXT_JOB_HPP
#define GIO_DBUS_CPP_CONTEXT_JOB_HPP

#include "common.hpp"

#include "details/pimpl.hpp"

namespace Gio {

class ContextJobImpl;
class GIO_DBUS_CPP_EXPORT_CLASS(ContextJob)
{
    GIO_DBUS_CPP_DECLARE_PIMPL_PARTS(ContextJob, ContextJobImpl)

public:
    uint64_t id() const noexcept;

private:
    friend class ContextImpl;
    ContextJob(uint64_t id);
};

} /* namespace Gio */

#endif /* GIO_DBUS_CPP_CONTEXT_JOB_HPP */
