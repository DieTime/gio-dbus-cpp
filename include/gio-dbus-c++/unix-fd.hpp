#ifndef GIO_DBUS_CPP_UNIX_FD_HPP
#define GIO_DBUS_CPP_UNIX_FD_HPP

#include "common.hpp"

#include "details/pimpl.hpp"

namespace Gio::DBus {

class UnixFDImpl;
class GIO_DBUS_CPP_EXPORT_CLASS(UnixFD)
{
    GIO_DBUS_CPP_DECLARE_PIMPL_PARTS(UnixFD, UnixFDImpl)

public:
    UnixFD(int unix_fd);

    int as_int() const noexcept;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_UNIX_FD_HPP */
