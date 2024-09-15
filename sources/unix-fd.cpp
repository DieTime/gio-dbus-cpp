#include "unix-fd.hpp"
#include "details/exception.hpp"

#include <gio/gio.h>

namespace Gio::DBus {

class UnixFDImpl
{
public:
    UnixFDImpl(int unix_fd);

    int as_int() const noexcept;

private:
    int m_unix_fd;
};

UnixFDImpl::UnixFDImpl(int unix_fd)
    : m_unix_fd(unix_fd)
{}

int UnixFDImpl::as_int() const noexcept
{
    return m_unix_fd;
}

UnixFD::UnixFD(int unix_fd)
    : m_pimpl(std::make_unique<UnixFDImpl>(unix_fd))
{}

UnixFD::~UnixFD() = default;

int UnixFD::as_int() const noexcept
{
    return m_pimpl->as_int();
}

} /* namespace Gio::DBus */
