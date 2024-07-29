#include "error.hpp"

namespace Gio::DBus {

class ErrorImpl
{
public:
    ErrorImpl(std::string name, std::string message) noexcept;

    const std::string &name() const noexcept;
    const std::string &message() const noexcept;

private:
    std::string m_name;
    std::string m_message;
};

ErrorImpl::ErrorImpl(std::string name, std::string message) noexcept
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

Error::Error(std::string name, std::string message)
    : std::runtime_error(name + ": " + message)
    , m_pimpl(std::make_unique<ErrorImpl>(std::move(name), std::move(message)))
{}

Error::~Error() = default;

const std::string &Error::name() const noexcept
{
    return m_pimpl->name();
}

const std::string &Error::message() const noexcept
{
    return m_pimpl->message();
}

} /* namespace Gio::DBus */
