#ifndef GIO_DBUS_CPP_TYPE_TRAITS_HPP
#define GIO_DBUS_CPP_TYPE_TRAITS_HPP

#include <tuple>

namespace Gio::DBus::Details {

template<typename T>
struct is_tuple_type_impl: std::false_type
{};

template<typename... Ts>
struct is_tuple_type_impl<std::tuple<Ts...>>: std::true_type
{};

template<typename T>
struct is_tuple_type: is_tuple_type_impl<std::decay_t<T>>
{};

template<typename T>
inline constexpr bool is_tuple_type_v = is_tuple_type<T>::value;

} /* namespace Gio::DBus::Details */

#endif /* GIO_DBUS_CPP_TYPE_TRAITS_HPP */
