#ifndef GIO_DBUS_CPP_UNIX_FD_HPP
#define GIO_DBUS_CPP_UNIX_FD_HPP

#include <memory>

namespace Gio::DBus {

class UnixFDImpl;
class UnixFD
{
public:
    UnixFD(int unix_fd);
    ~UnixFD();

    int as_int() const noexcept;

private:
    std::unique_ptr<UnixFDImpl> m_pimpl;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_UNIX_FD_HPP */
