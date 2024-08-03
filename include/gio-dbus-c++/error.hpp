#ifndef GIO_DBUS_CPP_ERROR_HPP
#define GIO_DBUS_CPP_ERROR_HPP

#include "common.hpp"

#include <memory>
#include <stdexcept>
#include <string_view>

namespace Gio::DBus {

class ErrorImpl;
class GIO_DBUS_CPP_EXPORT Error: public std::exception
{
public:
    Error(std::string name, std::string message) noexcept;
    ~Error() override;

    std::string_view name() const noexcept;
    std::string_view message() const noexcept;

    const char *what() const noexcept override;

private:
    std::unique_ptr<ErrorImpl> m_pimpl;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_ERROR_HPP */
