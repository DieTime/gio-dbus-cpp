#ifndef GIO_DBUS_CPP_SIGNATURE_HPP
#define GIO_DBUS_CPP_SIGNATURE_HPP

#include <memory>
#include <string>

namespace Gio::DBus {

class SignatureImpl;
class Signature
{
public:
    Signature(std::string signature);
    ~Signature();

    const std::string &as_string() const noexcept;

private:
    std::unique_ptr<SignatureImpl> m_pimpl;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_SIGNATURE_HPP */
