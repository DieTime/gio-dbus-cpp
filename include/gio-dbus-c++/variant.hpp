#ifndef GIO_DBUS_CPP_VARIANT_HPP
#define GIO_DBUS_CPP_VARIANT_HPP

#include "details/dbus-deserializer.hpp"
#include "details/dbus-serializer.hpp"
#include "details/dbus-type-traits.hpp"
#include "details/exception.hpp"

#include <gio/gio.h>
#include <memory>

namespace Gio::DBus {

class Variant
{
public:
    template<typename T>
    Variant(const T &value); /* NOLINT(google-explicit-constructor) */

    Variant(GVariant *variant) /* NOLINT(google-explicit-constructor) */
        : m_variant(variant, &g_variant_unref)
    {}

    template<typename T>
    bool contains_value_of_type() const noexcept
    {
        using namespace Details;

        static_assert(is_dbus_type_v<T>,
                      "Attempt to check whether Gio::DBus::Variant stores a value of type T using "
                      "Gio::DBus::Variant::is<T>(), but T is not a dbus type");

        return g_variant_is_of_type(as_gio_variant(), dbus_type_to_variant_type_v<T>);
    }

    template<typename T>
    T as() const;

private:
    template<typename T>
    friend struct Details::DBusSerializer;

    const char *dbus_type_signature() const noexcept
    {
        const char *type_signature = g_variant_get_type_string(m_variant.get());

        if (!type_signature) {
            return "?";
        }

        return type_signature;
    }

    GVariant *as_gio_variant() const noexcept
    {
        return m_variant.get();
    }

    std::unique_ptr<GVariant, decltype(&g_variant_unref)> m_variant;
};

namespace Details {

template<>
struct DBusType<Gio::DBus::Variant>: std::true_type
{
    static constexpr auto name = "v"_cts;
    static constexpr auto class_name = "Gio::DBus::Variant"_cts;
};

template<>
struct DBusSerializer<Gio::DBus::Variant>
{
    static GVariant *serialize(const Gio::DBus::Variant &variant) noexcept
    {
        return g_variant_new_variant(variant.as_gio_variant());
    }
};

template<>
struct DBusDeserializer<Gio::DBus::Variant>
{
    static Gio::DBus::Variant deserialize(GVariant *message)
    {
        return g_variant_get_variant(message);
    }
};

} /* namespace Details */

template<typename T>
Variant::Variant(const T &value)
    : m_variant(nullptr, &g_variant_unref)
{
    using namespace Details;

    static_assert(is_dbus_type_v<T>(),
                  "Attempt to construct Gio::DBus::Variant from value of type T using "
                  "Gio::DBus::Variant::Variant<T>(const T &), but T is not a dbus type");

    try {
        m_variant.reset(g_varaiant_ref(DBusSerializer<T>::serialize(value)));
    }
    catch (const std::exception &err) {
        THROW_GIO_DBUS_CPP_ERROR(
            std::string("Failed to construct Gio::DBus::Variant from value of type T (aka ")
            + DBusType<T>::class_name.data() + " aka " + DBusType<T>::name.data()
            + ") using Gio::DBus::Message::Message<T>(const T &)" + " (" + err.what() + ")");
    }
}

template<typename T>
T Variant::as() const
{
    using namespace Details;

    static_assert(is_dbus_type_v<T>,
                  "Attempt to read a value of type T using Gio::DBus::Variant::as<T>(), "
                  "but T is not a dbus type");

    if (!contains_value_of_type<T>()) {
        THROW_GIO_DBUS_CPP_ERROR(std::string("Attempt to read a value of type T (aka ")
                                 + DBusType<T>::class_name.data() + " aka "
                                 + DBusType<T>::name.data()
                                 + ") using Gio::DBus::Variant::as<T>(), "
                                   "but Gio::DBus::Variant contains value of type "
                                 + dbus_type_signature());
    }

    try {
        return DBusDeserializer<T>::deserialize(as_gio_variant());
    }
    catch (const std::exception &err) {
        THROW_GIO_DBUS_CPP_ERROR(std::string("Failed to read a value of type T (aka ")
                                 + DBusType<T>::class_name.data() + " aka "
                                 + DBusType<T>::name.data()
                                 + ") using Gio::DBus::Variant::as<T>() (" + err.what() + ")");
    }
}

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_VARIANT_HPP */
