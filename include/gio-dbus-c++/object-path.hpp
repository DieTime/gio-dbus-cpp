#ifndef GIO_DBUS_CPP_OBJECT_PATH_HPP
#define GIO_DBUS_CPP_OBJECT_PATH_HPP

#include "common.hpp"

#include "details/pimpl.hpp"

#include <string>

namespace Gio::DBus {

class ObjectPathImpl;
class GIO_DBUS_CPP_EXPORT_CLASS(ObjectPath)
{
    GIO_DBUS_CPP_DECLARE_PIMPL_PARTS(ObjectPath, ObjectPathImpl)

public:
    ObjectPath(std::string object_path);

    const std::string &as_string() const noexcept;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_OBJECT_PATH_HPP */
