#ifndef GIO_DBUS_CPP_DETAILS_DBUS_TYPE_TRAITS_HPP
#define GIO_DBUS_CPP_DETAILS_DBUS_TYPE_TRAITS_HPP

#include <gio/gio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace Gio::DBus::Details {

template<typename T>
struct DBusType: public std::false_type
{};

template<>
struct DBusType<bool>: public std::true_type
{
    static inline const std::string name = "b";
    static inline const std::string class_name = "bool";
};

template<>
struct DBusType<uint8_t>: public std::true_type
{
    static inline const std::string name = "y";
    static inline const std::string class_name = "uint8_t";
};

template<>
struct DBusType<int16_t>: public std::true_type
{
    static inline const std::string name = "n";
    static inline const std::string class_name = "int16_t";
};

template<>
struct DBusType<uint16_t>: public std::true_type
{
    static inline const std::string name = "q";
    static inline const std::string class_name = "uint16_t";
};

template<>
struct DBusType<int32_t>: public std::true_type
{
    static inline const std::string name = "i";
    static inline const std::string class_name = "int32_t";
};

template<>
struct DBusType<uint32_t>: public std::true_type
{
    static inline const std::string name = "u";
    static inline const std::string class_name = "uint32_t";
};

template<>

struct DBusType<int64_t>: public std::true_type
{
    static inline const std::string name = "x";
    static inline const std::string class_name = "int64_t";
};

template<>
struct DBusType<uint64_t>: public std::true_type
{
    static inline const std::string name = "t";
    static inline const std::string class_name = "uint64_t";
};

template<>
struct DBusType<double>: public std::true_type
{
    static inline const std::string name = "d";
    static inline const std::string class_name = "double";
};

template<>
struct DBusType<std::string>: public std::true_type
{
    static inline const std::string name = "s";
    static inline const std::string class_name = "std::string";
};

template<typename T, typename Allocator>
struct DBusType<std::vector<T, Allocator>>: public std::true_type
{
    /* clang-format off */
    static inline const std::string name = "a" + DBusType<T>::name;
    static inline const std::string class_name = "std::vector<" + DBusType<T>::class_name + ">";
    /* clang-format on */
};

template<typename K, typename V, typename Hash, typename Pred, typename Allocator>
struct DBusType<std::unordered_map<K, V, Hash, Pred, Allocator>>: public std::true_type
{
    /* clang-format off */
    static inline const std::string name = "a{" + DBusType<K>::name + DBusType<V>::name + "}";
    static inline const std::string class_name = "std::unordered_map<" + DBusType<K>::class_name + ", " + DBusType<V>::class_name + ">";
    /* clang-format on */
};

template<typename T, typename... R>
struct DBusType<std::tuple<T, R...>>: public std::true_type
{
    /* clang-format off */
    static inline const std::string name = "(" + (DBusType<T>::name + ... + DBusType<R>::name) + ")";
    static inline const std::string class_name = "std::tuple<" + (DBusType<T>::class_name + ... + (", " + DBusType<R>::class_name)) + ">";
    /* clang-format on */
};

template<typename T>
inline constexpr bool is_dbus_type_v = DBusType<T>::value;

template<typename T>
inline const GVariantType *dbus_type_to_variant_type_v = reinterpret_cast<const GVariantType *>(
    DBusType<T>::name.data());

} /* namespace Gio::DBus::Details */

#endif /* GIO_DBUS_CPP_DETAILS_DBUS_TYPE_TRAITS_HPP */
