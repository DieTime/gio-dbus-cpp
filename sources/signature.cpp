#include "signature.hpp"

#include <gio/gio.h>

namespace Gio::DBus {

class SignatureImpl
{
public:
    SignatureImpl(std::string signature);

    const std::string &as_string() const noexcept;

private:
    std::string m_signature;
};

SignatureImpl::SignatureImpl(std::string signature)
    : m_signature(std::move(signature))
{
    if (!g_variant_is_signature(m_signature.c_str())) {
        GIO_DBUS_CPP_THROW_ERROR(
            "Attempt to create Gio::DBus::Signature using not valid signature \"" + m_signature
            + "\"")
    }
}

const std::string &SignatureImpl::as_string() const noexcept
{
    return m_signature;
}

GIO_DBUS_CPP_IMPLEMENT_PIMPL_PARTS(Signature, SignatureImpl)

Signature::Signature(std::string signature)
    : m_pimpl(std::make_unique<SignatureImpl>(std::move(signature)))
{}

const std::string &Signature::as_string() const noexcept
{
    return m_pimpl->as_string();
}

} /* namespace Gio::DBus */
