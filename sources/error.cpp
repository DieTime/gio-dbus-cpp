#include "error.hpp"

namespace Gio::DBus {

Error::Error(std::string name, std::string message) noexcept
    : m_name(std::move(name))
    , m_message(std::move(message))
{}

const std::string &Error::name() const noexcept
{
    return m_name;
}

const std::string &Error::message() const noexcept
{
    return m_message;
}

const char *Error::what() const noexcept
{
    return m_message.data();
}

} /* namespace Gio::DBus */
