#include "timeout.hpp"

namespace Gio::DBus {

int Timeout::milliseconds() const noexcept
{
    return m_milliseconds;
}

} /* namespace Gio::DBus */
