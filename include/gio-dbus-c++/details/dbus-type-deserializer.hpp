#ifndef GIO_DBUS_C_PLUS_PLUG_DETAILS_DBUS_TYPE_DESERIALIZER_HPP
#define GIO_DBUS_C_PLUS_PLUG_DETAILS_DBUS_TYPE_DESERIALIZER_HPP

#include <gio/gio.h>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace Gio::DBus::Details {

template<typename T>
struct DBusTypeDeserializer
{};

template<>
struct DBusTypeDeserializer<bool>
{
    static bool deserialize(GVariant *message) noexcept
    {
        return g_variant_get_boolean(message);
    }
};

template<>
struct DBusTypeDeserializer<uint8_t>
{
    static uint8_t deserialize(GVariant *message) noexcept
    {
        return g_variant_get_byte(message);
    }
};

template<>
struct DBusTypeDeserializer<int16_t>
{
    static int16_t deserialize(GVariant *message) noexcept
    {
        return g_variant_get_int16(message);
    }
};

template<>
struct DBusTypeDeserializer<uint16_t>
{
    static uint16_t deserialize(GVariant *message) noexcept
    {
        return g_variant_get_uint16(message);
    }
};

template<>
struct DBusTypeDeserializer<int32_t>
{
    static int32_t deserialize(GVariant *message) noexcept
    {
        return g_variant_get_int32(message);
    }
};

template<>
struct DBusTypeDeserializer<uint32_t>
{
    static uint32_t deserialize(GVariant *message) noexcept
    {
        return g_variant_get_uint32(message);
    }
};

template<>
struct DBusTypeDeserializer<int64_t>
{
    static int64_t deserialize(GVariant *message) noexcept
    {
        return g_variant_get_int64(message);
    }
};

template<>
struct DBusTypeDeserializer<uint64_t>
{
    static uint64_t deserialize(GVariant *message) noexcept
    {
        return g_variant_get_uint64(message);
    }
};

template<>
struct DBusTypeDeserializer<double>
{
    static double deserialize(GVariant *message) noexcept
    {
        return g_variant_get_double(message);
    }
};

template<>
struct DBusTypeDeserializer<std::string>
{
    static std::string deserialize(GVariant *message)
    {
        return {g_variant_get_string(message, nullptr)};
    }
};

template<typename T, typename Allocator>
struct DBusTypeDeserializer<std::vector<T, Allocator>>
{
    using Vector = std::vector<T, Allocator>;

    static Vector deserialize(GVariant *message)
    {
        Vector vector;
        vector.reserve(g_variant_n_children(message));

        GVariantIter iterator;
        g_variant_iter_init(&iterator, message);

        GVariant *entry = nullptr;

        while ((entry = g_variant_iter_next_value(&iterator))) {
            vector.emplace_back(DBusTypeDeserializer<T>::deserialize(entry));
            g_variant_unref(entry);
        }

        return vector;
    }
};

template<typename K, typename V, typename Hash, typename Pred, typename Allocator>
struct DBusTypeDeserializer<std::unordered_map<K, V, Hash, Pred, Allocator>>
{
    using Map = std::unordered_map<K, V, Hash, Pred, Allocator>;

    static Map deserialize(GVariant *message)
    {
        Map map;
        map.reserve(g_variant_n_children(message));

        GVariantIter iterator;
        g_variant_iter_init(&iterator, message);

        GVariant *entry = nullptr;

        while ((entry = g_variant_iter_next_value(&iterator))) {
            GVariant *key = g_variant_get_child_value(entry, 0);
            GVariant *value = g_variant_get_child_value(entry, 1);

            map.insert({
                DBusTypeDeserializer<K>::deserialize(key),
                DBusTypeDeserializer<V>::deserialize(value),
            });

            g_variant_unref(entry);
            g_variant_unref(key);
            g_variant_unref(value);
        }

        return map;
    }
};

template<typename... T>
struct DBusTypeDeserializer<std::tuple<T...>>
{
    using Tuple = std::tuple<T...>;

    static Tuple deserialize(GVariant *message)
    {
        return implementation(message, std::index_sequence_for<T...>());
    }

private:
    template<size_t... I>
    static Tuple implementation(GVariant *message, std::index_sequence<I...>)
    {
        using GVariantUniquePtr = std::unique_ptr<GVariant, decltype(&g_variant_unref)>;

        return {DBusTypeDeserializer<T>::deserialize(
            GVariantUniquePtr(g_variant_get_child_value(message, I), &g_variant_unref).get())...};
    }
};

} /* namespace Gio::DBus::Details */

#endif /* GIO_DBUS_C_PLUS_PLUG_DETAILS_DBUS_TYPE_DESERIALIZER_HPP */
