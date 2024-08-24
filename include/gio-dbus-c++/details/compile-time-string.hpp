#ifndef GIO_DBUS_CPP_DETAILS_COMPILE_TIME_STRING_HPP
#define GIO_DBUS_CPP_DETAILS_COMPILE_TIME_STRING_HPP

#include <algorithm>
#include <cstddef>

namespace Gio::DBus::Details {

template<size_t Size>
class CompileTimeString
{
public:
    constexpr CompileTimeString(const char (&data)[Size]) noexcept
    {
        std::copy(data, data + Size, m_data);
    }

    constexpr const char *data() const noexcept
    {
        return m_data;
    }

    template<size_t OtherSize>
    constexpr auto operator+(CompileTimeString<OtherSize> other) const noexcept
    {
        char data[Size + OtherSize - 1] = {0};

        std::copy(m_data, m_data + Size - 1, data);
        std::copy(other.data(), other.data() + OtherSize, data + Size - 1);

        return CompileTimeString<Size + OtherSize - 1>(data);
    }

    char m_data[Size] = {0};
};

template<CompileTimeString S>
constexpr auto operator""_cts() noexcept
{
    return S;
}

} /* namespace Gio::DBus::Details */

#endif /* GIO_DBUS_CPP_DETAILS_COMPILE_TIME_STRING_HPP */
