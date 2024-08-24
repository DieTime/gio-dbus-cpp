#ifndef GIO_DBUS_CPP_OBJECT_PATH_HPP
#define GIO_DBUS_CPP_OBJECT_PATH_HPP

#include <memory>
#include <string>

namespace Gio::DBus {

class ObjectPathImpl;
class ObjectPath
{
public:
    ObjectPath(std::string object_path);
    ~ObjectPath();

    const std::string &as_string() const noexcept;

private:
    std::unique_ptr<ObjectPathImpl> m_pimpl;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_OBJECT_PATH_HPP */
