#include "timeout.hpp"

namespace Gio {

int Timeout::milliseconds() const noexcept
{
    return m_milliseconds;
}

} /* namespace Gio */
