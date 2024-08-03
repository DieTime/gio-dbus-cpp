#ifndef GIO_DBUS_CPP_DETAILS_DBUS_TYPE_TRAITS_HPP
#define GIO_DBUS_CPP_DETAILS_DBUS_TYPE_TRAITS_HPP

#include "compile-time-string.hpp"

#include <gio/gio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace Gio::DBus::Details {

template<typename T>
struct DBusType: std::false_type
{};

template<>
struct DBusType<bool>: std::true_type
{
    static constexpr auto name = "b"_cts;
    static constexpr auto class_name = "bool"_cts;
};

template<>
struct DBusType<uint8_t>: std::true_type
{
    static constexpr auto name = "y"_cts;
    static constexpr auto class_name = "uint8_t"_cts;
};

template<>
struct DBusType<int16_t>: std::true_type
{
    static constexpr auto name = "n"_cts;
    static constexpr auto class_name = "int16_t"_cts;
};

template<>
struct DBusType<uint16_t>: std::true_type
{
    static constexpr auto name = "q"_cts;
    static constexpr auto class_name = "uint16_t"_cts;
};

template<>
struct DBusType<int32_t>: std::true_type
{
    static constexpr auto name = "i"_cts;
    static constexpr auto class_name = "int32_t"_cts;
};

template<>
struct DBusType<uint32_t>: std::true_type
{
    static constexpr auto name = "u"_cts;
    static constexpr auto class_name = "uint32_t"_cts;
};

template<>

struct DBusType<int64_t>: std::true_type
{
    static constexpr auto name = "x"_cts;
    static constexpr auto class_name = "int64_t"_cts;
};

template<>
struct DBusType<uint64_t>: std::true_type
{
    static constexpr auto name = "t"_cts;
    static constexpr auto class_name = "uint64_t"_cts;
};

template<>
struct DBusType<double>: std::true_type
{
    static constexpr auto name = "d"_cts;
    static constexpr auto class_name = "double"_cts;
};

template<>
struct DBusType<std::string>: std::true_type
{
    static constexpr auto name = "s"_cts;
    static constexpr auto class_name = "std::string"_cts;
};

template<typename T, typename Allocator>
struct DBusType<std::vector<T, Allocator>>: std::true_type
{
    /* clang-format off */
    static constexpr auto name = "a"_cts + DBusType<T>::name;
    static constexpr auto class_name = "std::vector<"_cts + DBusType<T>::class_name + ">"_cts;
    /* clang-format on */
};

template<typename K, typename V, typename Hash, typename Pred, typename Allocator>
struct DBusType<std::unordered_map<K, V, Hash, Pred, Allocator>>: std::true_type
{
    /* clang-format off */
    static constexpr auto name = "a{"_cts + DBusType<K>::name + DBusType<V>::name + "}"_cts;
    static constexpr auto class_name = "std::unordered_map<"_cts + DBusType<K>::class_name + ", "_cts + DBusType<V>::class_name + ">"_cts;
    /* clang-format on */
};

template<typename T, typename... R>
struct DBusType<std::tuple<T, R...>>: std::true_type
{
    /* clang-format off */
    static constexpr auto name = "("_cts + (DBusType<T>::name + ... + DBusType<R>::name) + ")"_cts;
    static constexpr auto class_name = "std::tuple<"_cts + (DBusType<T>::class_name + ... + (", "_cts + DBusType<R>::class_name)) + ">"_cts;
    /* clang-format on */
};

template<typename T>
constexpr bool is_dbus_type_v = DBusType<T>::value;

template<typename T>
const GVariantType *dbus_type_to_variant_type_v = reinterpret_cast<const GVariantType *>(
    DBusType<T>::name.data());

} /* namespace Gio::DBus::Details */

#endif /* GIO_DBUS_CPP_DETAILS_DBUS_TYPE_TRAITS_HPP */
