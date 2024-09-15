#ifndef GIO_DBUS_CPP_DETAILS_DBUS_TYPE_SERIALIZER_HPP
#define GIO_DBUS_CPP_DETAILS_DBUS_TYPE_SERIALIZER_HPP

#include "dbus-type-traits.hpp"

#include "../object-path.hpp"
#include "../signature.hpp"
#include "../unix-fd.hpp"

#include <gio/gio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace Gio::DBus::Details {

template<typename T>
struct DBusSerializer
{};

template<>
struct DBusSerializer<bool>
{
    static GVariant *serialize(bool b) noexcept
    {
        return g_variant_new_boolean(b);
    }
};

template<>
struct DBusSerializer<uint8_t>
{
    static GVariant *serialize(uint8_t u8) noexcept
    {
        return g_variant_new_byte(u8);
    }
};

template<>
struct DBusSerializer<int16_t>
{
    static GVariant *serialize(int16_t i16) noexcept
    {
        return g_variant_new_int16(i16);
    }
};

template<>
struct DBusSerializer<uint16_t>
{
    static GVariant *serialize(uint16_t u16) noexcept
    {
        return g_variant_new_uint16(u16);
    }
};

template<>
struct DBusSerializer<int32_t>
{
    static GVariant *serialize(int32_t i32) noexcept
    {
        return g_variant_new_int32(i32);
    }
};

template<>
struct DBusSerializer<uint32_t>
{
    static GVariant *serialize(uint32_t u32) noexcept
    {
        return g_variant_new_uint32(u32);
    }
};

template<>
struct DBusSerializer<int64_t>
{
    static GVariant *serialize(int64_t i64) noexcept
    {
        return g_variant_new_int64(i64);
    }
};

template<>
struct DBusSerializer<uint64_t>
{
    static GVariant *serialize(uint64_t u64) noexcept
    {
        return g_variant_new_uint64(u64);
    }
};

template<>
struct DBusSerializer<double>
{
    static GVariant *serialize(double d) noexcept
    {
        return g_variant_new_double(d);
    }
};

template<>
struct DBusSerializer<std::string>
{
    static GVariant *serialize(const std::string &string) noexcept
    {
        return g_variant_new_string(string.c_str());
    }
};

template<typename T, typename Allocator>
struct DBusSerializer<std::vector<T, Allocator>>
{
    using Vector = std::vector<T, Allocator>;

    static GVariant *serialize(const Vector &vector) noexcept
    {
        GVariantBuilder builder;
        g_variant_builder_init(&builder, dbus_type_to_variant_type_v<Vector>);

        for (const T &value: vector) {
            g_variant_builder_add_value(&builder, DBusSerializer<T>::serialize(value));
        }

        return g_variant_builder_end(&builder);
    }
};

template<typename K, typename V, typename Hash, typename Pred, typename Allocator>
struct DBusSerializer<std::unordered_map<K, V, Hash, Pred, Allocator>>
{
    using Map = std::unordered_map<K, V, Hash, Pred, Allocator>;

    static GVariant *serialize(const Map &map) noexcept
    {
        GVariantBuilder builder;
        g_variant_builder_init(&builder, dbus_type_to_variant_type_v<Map>);

        for (const auto &[k, v]: map) {
            g_variant_builder_add_value(&builder,
                                        g_variant_new_dict_entry(DBusSerializer<K>::serialize(k),
                                                                 DBusSerializer<V>::serialize(v)));
        }

        return g_variant_builder_end(&builder);
    }
};

template<typename... T>
struct DBusSerializer<std::tuple<T...>>
{
    using Tuple = std::tuple<T...>;

    static GVariant *serialize(const Tuple &tuple) noexcept
    {
        return implementation(tuple, std::index_sequence_for<T...>());
    }

private:
    template<size_t... I>
    static GVariant *implementation(const Tuple &tuple, std::index_sequence<I...>) noexcept
    {
        GVariantBuilder builder;
        g_variant_builder_init(&builder, dbus_type_to_variant_type_v<Tuple>);

        (g_variant_builder_add_value(&builder,
                                     DBusSerializer<std::tuple_element_t<I, Tuple>>::serialize(
                                         std::get<I>(tuple))),
         ...);

        return g_variant_builder_end(&builder);
    }
};

template<>
struct DBusSerializer<ObjectPath>
{
    static GVariant *serialize(const ObjectPath &object_path) noexcept
    {
        return g_variant_new_object_path(object_path.as_string().c_str());
    }
};

template<>
struct DBusSerializer<Signature>
{
    static GVariant *serialize(const Signature &signature) noexcept
    {
        return g_variant_new_signature(signature.as_string().c_str());
    }
};

template<>
struct DBusSerializer<UnixFD>
{
    static GVariant *serialize(const UnixFD &unix_fd) noexcept
    {
        return g_variant_new_handle(unix_fd.as_number());
    }
};

} /* namespace Gio::DBus::Details */

#endif /* GIO_DBUS_CPP_DETAILS_DBUS_TYPE_SERIALIZER_HPP */
