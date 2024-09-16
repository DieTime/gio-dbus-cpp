#ifndef GIO_DBUS_CPP_SIGNATURE_HPP
#define GIO_DBUS_CPP_SIGNATURE_HPP

#include "common.hpp"

#include "details/pimpl.hpp"

#include <string>

namespace Gio::DBus {

class SignatureImpl;
class GIO_DBUS_CPP_EXPORT_CLASS(Signature)
{
    GIO_DBUS_CPP_DECLARE_PIMPL_PARTS(Signature, SignatureImpl)

public:
    Signature(std::string signature);

    const std::string &as_string() const noexcept;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_SIGNATURE_HPP */
