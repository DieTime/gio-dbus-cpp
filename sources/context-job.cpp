#include "context-job.hpp"

namespace Gio {

class ContextJobImpl
{
public:
    ContextJobImpl(uint64_t id);

    uint64_t id() const noexcept;

private:
    uint64_t m_id;
};

ContextJobImpl::ContextJobImpl(uint64_t id)
    : m_id(id)
{}

uint64_t ContextJobImpl::id() const noexcept
{
    return m_id;
}

GIO_DBUS_CPP_IMPLEMENT_PIMPL_PARTS(ContextJob, ContextJobImpl)

ContextJob::ContextJob(uint64_t id)
    : m_pimpl(std::make_unique<ContextJobImpl>(id))
{}

uint64_t ContextJob::id() const noexcept
{
    return m_pimpl->id();
}

} /* namespace Gio */
