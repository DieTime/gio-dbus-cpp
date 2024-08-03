#include "error.hpp"
#include "common.hpp"

namespace Gio::DBus {

class ErrorImpl
{
public:
    ErrorImpl(std::string name, std::string message);

    std::string_view name() const noexcept;
    std::string_view message() const noexcept;

private:
    std::string m_name;
    std::string m_message;
};

ErrorImpl::ErrorImpl(std::string name, std::string message)
    : m_name(std::move(name))
    , m_message(std::move(message))
{}

std::string_view ErrorImpl::name() const noexcept
{
    return m_name;
}

std::string_view ErrorImpl::message() const noexcept
{
    return m_message;
}

Error::Error(std::string name, std::string message) noexcept
    : m_pimpl(new(std::nothrow) ErrorImpl(std::move(name), std::move(message)))
{}

Error::~Error() = default;

std::string_view Error::name() const noexcept
{
    if (m_pimpl) {
        return m_pimpl->name();
    }

    return {GIO_DBUS_CPP_ERROR_NAME};
}

std::string_view Error::message() const noexcept
{
    if (m_pimpl) {
        return m_pimpl->message();
    }

    return {"Empty (Failed to receive an error message)"};
}

const char *Error::what() const noexcept
{
    return message().data();
}

} /* namespace Gio::DBus */
