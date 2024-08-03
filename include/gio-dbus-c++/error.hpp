#ifndef GIO_DBUS_CPP_ERROR_HPP
#define GIO_DBUS_CPP_ERROR_HPP

#include "common.hpp"

#include <memory>
#include <stdexcept>

namespace Gio::DBus {

class ErrorImpl;
class GIO_DBUS_CPP_EXPORT Error: public std::exception
{
public:
    Error(std::string name, std::string message) noexcept;
    ~Error() override;

    const std::string &name() const noexcept;
    const std::string &message() const noexcept;

    const char *what() const noexcept override;

private:
    std::unique_ptr<ErrorImpl> m_pimpl;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_ERROR_HPP */
