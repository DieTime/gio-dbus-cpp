#ifndef GIO_DBUS_C_PLUS_PLUG_DETAILS_DBUS_TYPE_SERIALIZER_HPP
#define GIO_DBUS_C_PLUS_PLUG_DETAILS_DBUS_TYPE_SERIALIZER_HPP

#include "dbus-type-traits.hpp"

#include <gio/gio.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace Gio::DBus::Details {

template<typename T>
struct DBusTypeSerializer
{};

template<>
struct DBusTypeSerializer<bool>
{
    static GVariant *serialize(bool b) noexcept
    {
        return g_variant_new_boolean(b);
    }
};

template<>
struct DBusTypeSerializer<uint8_t>
{
    static GVariant *serialize(uint8_t u8) noexcept
    {
        return g_variant_new_byte(u8);
    }
};

template<>
struct DBusTypeSerializer<int16_t>
{
    static GVariant *serialize(int16_t i16) noexcept
    {
        return g_variant_new_int16(i16);
    }
};

template<>
struct DBusTypeSerializer<uint16_t>
{
    static GVariant *serialize(uint16_t u16) noexcept
    {
        return g_variant_new_uint16(u16);
    }
};

template<>
struct DBusTypeSerializer<int32_t>
{
    static GVariant *serialize(int32_t i32) noexcept
    {
        return g_variant_new_int32(i32);
    }
};

template<>
struct DBusTypeSerializer<uint32_t>
{
    static GVariant *serialize(uint32_t u32) noexcept
    {
        return g_variant_new_uint32(u32);
    }
};

template<>
struct DBusTypeSerializer<int64_t>
{
    static GVariant *serialize(int64_t i64) noexcept
    {
        return g_variant_new_int64(i64);
    }
};

template<>
struct DBusTypeSerializer<uint64_t>
{
    static GVariant *serialize(uint64_t u64) noexcept
    {
        return g_variant_new_uint64(u64);
    }
};

template<>
struct DBusTypeSerializer<double>
{
    static GVariant *serialize(double d) noexcept
    {
        return g_variant_new_double(d);
    }
};

template<>
struct DBusTypeSerializer<std::string>
{
    static GVariant *serialize(const std::string &string) noexcept
    {
        return g_variant_new_string(string.c_str());
    }
};

template<typename T, typename Allocator>
struct DBusTypeSerializer<std::vector<T, Allocator>>
{
    using Vector = std::vector<T, Allocator>;

    static GVariant *serialize(const Vector &vector) noexcept
    {
        GVariantBuilder builder;
        g_variant_builder_init(&builder, dbus_type_to_variant_type_v<Vector>);

        for (const T &value: vector) {
            g_variant_builder_add_value(&builder, DBusTypeSerializer<T>::serialize(value));
        }

        return g_variant_builder_end(&builder);
    }
};

template<typename K, typename V, typename Hash, typename Pred, typename Allocator>
struct DBusTypeSerializer<std::unordered_map<K, V, Hash, Pred, Allocator>>
{
    using Map = std::unordered_map<K, V, Hash, Pred, Allocator>;

    static GVariant *serialize(const Map &map) noexcept
    {
        GVariantBuilder builder;
        g_variant_builder_init(&builder, dbus_type_to_variant_type_v<Map>);

        for (const auto &[k, v]: map) {
            g_variant_builder_add_value(&builder,
                                        g_variant_new_dict_entry(DBusTypeSerializer<K>::serialize(k),
                                                                 DBusTypeSerializer<V>::serialize(
                                                                     v)));
        }

        return g_variant_builder_end(&builder);
    }
};

template<typename... T>
struct DBusTypeSerializer<std::tuple<T...>>
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
                                     DBusTypeSerializer<std::tuple_element_t<I, Tuple>>::serialize(
                                         std::get<I>(tuple))),
         ...);

        return g_variant_builder_end(&builder);
    }
};

} /* namespace Gio::DBus::Details */

#endif /* GIO_DBUS_C_PLUS_PLUG_DETAILS_DBUS_TYPE_SERIALIZER_HPP */