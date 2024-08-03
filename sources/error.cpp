#include "error.hpp"
#include "common.hpp"

namespace Gio::DBus {

class ErrorImpl
{
public:
    ErrorImpl(std::string name, std::string message);

    const std::string &name() const noexcept;
    const std::string &message() const noexcept;

private:
    std::string m_name;
    std::string m_message;
};

ErrorImpl::ErrorImpl(std::string name, std::string message)
    : m_name(std::move(name))
    , m_message(std::move(message))
{}

const std::string &ErrorImpl::name() const noexcept
{
    return m_name;
}

const std::string &ErrorImpl::message() const noexcept
{
    return m_message;
}

Error::Error(std::string name, std::string message) noexcept
    : m_pimpl(new(std::nothrow) ErrorImpl(std::move(name), std::move(message)))
{}

Error::~Error() = default;

const std::string &Error::name() const noexcept
{
    if (m_pimpl) {
        return m_pimpl->name();
    }

    static std::string name = GIO_DBUS_CPP_ERROR_NAME;
    return name;
}

const std::string &Error::message() const noexcept
{
    if (m_pimpl) {
        return m_pimpl->message();
    }

    static std::string message = "Empty (Failed to receive an error message)";
    return message;
}

const char *Error::what() const noexcept
{
    return message().data();
}

} /* namespace Gio::DBus */
