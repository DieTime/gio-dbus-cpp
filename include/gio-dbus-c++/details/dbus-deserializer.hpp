#ifndef GIO_DBUS_CPP_DETAILS_DBUS_TYPE_DESERIALIZER_HPP
#define GIO_DBUS_CPP_DETAILS_DBUS_TYPE_DESERIALIZER_HPP

#include "../object-path.hpp"
#include "../signature.hpp"
#include "../unix-fd.hpp"

#include <gio/gio.h>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace Gio::DBus::Details {

template<typename T>
struct DBusDeserializer
{};

template<>
struct DBusDeserializer<bool>
{
    static bool deserialize(GVariant *message) noexcept
    {
        return g_variant_get_boolean(message);
    }
};

template<>
struct DBusDeserializer<uint8_t>
{
    static uint8_t deserialize(GVariant *message) noexcept
    {
        return g_variant_get_byte(message);
    }
};

template<>
struct DBusDeserializer<int16_t>
{
    static int16_t deserialize(GVariant *message) noexcept
    {
        return g_variant_get_int16(message);
    }
};

template<>
struct DBusDeserializer<uint16_t>
{
    static uint16_t deserialize(GVariant *message) noexcept
    {
        return g_variant_get_uint16(message);
    }
};

template<>
struct DBusDeserializer<int32_t>
{
    static int32_t deserialize(GVariant *message) noexcept
    {
        return g_variant_get_int32(message);
    }
};

template<>
struct DBusDeserializer<uint32_t>
{
    static uint32_t deserialize(GVariant *message) noexcept
    {
        return g_variant_get_uint32(message);
    }
};

template<>
struct DBusDeserializer<int64_t>
{
    static int64_t deserialize(GVariant *message) noexcept
    {
        return g_variant_get_int64(message);
    }
};

template<>
struct DBusDeserializer<uint64_t>
{
    static uint64_t deserialize(GVariant *message) noexcept
    {
        return g_variant_get_uint64(message);
    }
};

template<>
struct DBusDeserializer<double>
{
    static double deserialize(GVariant *message) noexcept
    {
        return g_variant_get_double(message);
    }
};

template<>
struct DBusDeserializer<std::string>
{
    static std::string deserialize(GVariant *message)
    {
        return {g_variant_get_string(message, nullptr)};
    }
};

template<typename T, typename Allocator>
struct DBusDeserializer<std::vector<T, Allocator>>
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
            vector.emplace_back(DBusDeserializer<T>::deserialize(entry));
            g_variant_unref(entry);
        }

        return vector;
    }
};

template<typename K, typename V, typename Hash, typename Pred, typename Allocator>
struct DBusDeserializer<std::unordered_map<K, V, Hash, Pred, Allocator>>
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
                DBusDeserializer<K>::deserialize(key),
                DBusDeserializer<V>::deserialize(value),
            });

            g_variant_unref(entry);
            g_variant_unref(key);
            g_variant_unref(value);
        }

        return map;
    }
};

template<typename... T>
struct DBusDeserializer<std::tuple<T...>>
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

        return {DBusDeserializer<T>::deserialize(
            GVariantUniquePtr(g_variant_get_child_value(message, I), &g_variant_unref).get())...};
    }
};

template<>
struct DBusDeserializer<ObjectPath>
{
    static ObjectPath deserialize(GVariant *message) noexcept
    {
        return {g_variant_get_string(message, nullptr)};
    }
};

template<>
struct DBusDeserializer<Signature>
{
    static Signature deserialize(GVariant *message) noexcept
    {
        return {g_variant_get_string(message, nullptr)};
    }
};

template<>
struct DBusDeserializer<UnixFD>
{
    static UnixFD deserialize(GVariant *message) noexcept
    {
        return {g_variant_get_handle(message)};
    }
};

} /* namespace Gio::DBus::Details */

#endif /* GIO_DBUS_CPP_DETAILS_DBUS_TYPE_DESERIALIZER_HPP */
