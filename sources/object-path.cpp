#include "object-path.hpp"
#include "details/exception.hpp"

#include <gio/gio.h>

namespace Gio::DBus {

class ObjectPathImpl
{
public:
    ObjectPathImpl(std::string object_path);

    const std::string &as_string() const noexcept;

private:
    std::string m_object_path;
};

ObjectPathImpl::ObjectPathImpl(std::string object_path)
    : m_object_path(std::move(object_path))
{
    if (!g_variant_is_object_path(m_object_path.c_str())) {
        THROW_GIO_DBUS_CPP_ERROR(
            "Attempt to create Gio::DBus::ObjectPath using not valid object path \"" + m_object_path
            + "\"")
    }
}

const std::string &ObjectPathImpl::as_string() const noexcept
{
    return m_object_path;
}

ObjectPath::ObjectPath(std::string object_path)
    : m_pimpl(std::make_unique<ObjectPathImpl>(std::move(object_path)))
{}

ObjectPath::~ObjectPath() = default;

const std::string &ObjectPath::as_string() const noexcept
{
    return m_pimpl->as_string();
}

} /* namespace Gio::DBus */
