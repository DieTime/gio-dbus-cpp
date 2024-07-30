#ifndef GIO_DBUS_CPP_ERROR_HPP
#define GIO_DBUS_CPP_ERROR_HPP

#include "common.hpp"

#include <stdexcept>
#include <string>

namespace Gio::DBus {

class GIO_DBUS_CPP_EXPORT Error: public std::exception
{
public:
    Error(std::string name, std::string message) noexcept;

    const std::string &name() const noexcept;
    const std::string &message() const noexcept;

    const char *what() const noexcept override;

private:
    std::string m_name;
    std::string m_message;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_ERROR_HPP */
