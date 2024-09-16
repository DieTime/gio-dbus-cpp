#ifndef GIO_DBUS_CPP_TIMEOUT_HPP
#define GIO_DBUS_CPP_TIMEOUT_HPP

#include "common.hpp"

#include <chrono>
#include <limits>

namespace Gio::DBus {

class GIO_DBUS_CPP_EXPORT_CLASS(Timeout)
{
public:
    static inline constexpr std::chrono::milliseconds Default{-1};
    static inline constexpr std::chrono::milliseconds Inf{std::numeric_limits<int>::max()};

    template<typename Rep, typename Period>
    Timeout(const std::chrono::duration<Rep, Period> &duration)
        : m_milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count())
    {}

    int milliseconds() const noexcept;

private:
    int m_milliseconds;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_TIMEOUT_HPP */
