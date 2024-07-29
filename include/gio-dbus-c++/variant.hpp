#ifndef GIO_DBUS_CPP_VARIANT_HPP
#define GIO_DBUS_CPP_VARIANT_HPP

#include "details/dbus-type-deserializer.hpp"
#include "details/dbus-type-serializer.hpp"
#include "details/dbus-type-traits.hpp"
#include "details/exception.hpp"

#include <gio/gio.h>
#include <memory>

namespace Gio::DBus {

class Variant
{
public:
    template<typename T>
    Variant(const T &value);    /* NOLINT(google-explicit-constructor) */
    Variant(GVariant *variant); /* NOLINT(google-explicit-constructor) */

    std::string_view type() const noexcept;

    template<typename T>
    bool is() const;

    template<typename T>
    T as() const;

private:
    template<typename T>
    friend class Details::DBusTypeSerializer;

    GVariant *as_gio_variant() const noexcept;

private:
    std::unique_ptr<GVariant, decltype(&g_variant_unref)> m_variant;
};

namespace Details {

template<>
struct DBusType<Gio::DBus::Variant>: public std::true_type
{
    static inline const std::string name = "v";
    static inline const std::string class_name = "Gio::DBus::Variant";
};

template<>
struct DBusTypeSerializer<Gio::DBus::Variant>
{
    static GVariant *serialize(const Gio::DBus::Variant &variant) noexcept
    {
        return g_variant_new_variant(variant.as_gio_variant());
    }
};

template<>
struct DBusTypeDeserializer<Gio::DBus::Variant>
{
    static Gio::DBus::Variant deserialize(GVariant *message)
    {
        return g_variant_get_variant(message);
    }
};

} /* namespace Details */

template<typename T>
Variant::Variant(const T &value)
{
    static_assert(Details::is_dbus_type_v<T>(),
                  "Attempt to construct Gio::DBus::Variant from value of type T using "
                  "Gio::DBus::Variant::Variant<T>(const T &), but T is not a dbus type");

    try {
        m_variant = std::make_unique<GVariant, &g_variant_unref>(
            Details::DBusTypeSerializer<T>::serialize(value), &g_variant_unref);
    }
    catch (const std::exception &err) {
        THROW_GIO_DBUS_CPP_ERROR(
            std::string("Failed to construct Gio::DBus::Variant from value of type T (aka ")
            + Details::DBusType<T>::class_name.data() + " aka " + Details::DBusType<T>::name.data()
            + ") using Gio::DBus::Message::Message<T>(const T &)" + " (" + err.what() + ")");
    }
}

Variant::Variant(GVariant *variant) /* NOLINT(google-explicit-constructor) */
    : m_variant(variant, &g_variant_unref)
{}

std::string_view Variant::type() const noexcept
{
    const char *type = g_variant_get_type_string(m_variant.get());

    if (!type) {
        return {"unknown"};
    }

    return {type};
}

template<typename T>
bool Variant::is() const
{
    static_assert(Details::is_dbus_type_v<T>,
                  "Attempt to check whether Gio::DBus::Variant stores a value of type T using "
                  "Gio::DBus::Variant::is<T>(), but T is not a dbus type");

    return g_variant_is_of_type(const_cast<GVariant *>(m_variant.get()),
                                Details::dbus_type_to_variant_type_v<T>);
}

template<typename T>
T Variant::as() const
{
    static_assert(Details::is_dbus_type_v<T>,
                  "Attempt to read a value of type T using Gio::DBus::Variant::as<T>(), "
                  "but T is not a dbus type");

    if (!is<T>()) {
        THROW_GIO_DBUS_CPP_ERROR(std::string("Attempt to read a value of type T (aka ")
                                 + Details::DBusType<T>::class_name.data() + " aka "
                                 + Details::DBusType<T>::name.data()
                                 + ") using Gio::DBus::Variant::as<T>(), "
                                   "but Gio::DBus::Variant contains value of type "
                                 + type().data());
    }

    try {
        return Details::DBusTypeDeserializer<T>::deserialize(
            const_cast<GVariant *>(m_variant.get()));
    }
    catch (const std::exception &err) {
        THROW_GIO_DBUS_CPP_ERROR(std::string("Failed to read a value of type T (aka ")
                                 + Details::DBusType<T>::class_name.data() + " aka "
                                 + Details::DBusType<T>::name.data()
                                 + ") using Gio::DBus::Variant::as<T>() (" + err.what() + ")");
    }
}

GVariant *Variant::as_gio_variant() const noexcept
{
    return m_variant.get();
}

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_VARIANT_HPP */
