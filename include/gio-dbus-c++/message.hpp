#ifndef GIO_DBUS_CPP_MESSAGE_HPP
#define GIO_DBUS_CPP_MESSAGE_HPP

#include "details/dbus-deserializer.hpp"
#include "details/dbus-serializer.hpp"
#include "details/dbus-type-traits.hpp"
#include "details/exception.hpp"
#include "details/type-traits.hpp"

#include <gio/gio.h>
#include <iostream>
#include <memory>

namespace Gio::DBus {

class Message
{
public:
    template<typename T>
    Message(const T &value) /* NOLINT(google-explicit-constructor) */
        : m_variant(nullptr, &g_variant_unref)
    {
        using namespace Details;

        static_assert(is_dbus_type_v<T>,
                      "Attempt to construct Gio::DBus::Message from value of type T using "
                      "Gio::DBus::Message::Message<T>(const T &), but T is not a dbus type");

        try {
            GVariant *variant = nullptr;

            if constexpr (is_tuple_type_v<T>) {
                variant = DBusSerializer<T>::serialize(value);
            } else {
                variant = DBusSerializer<std::tuple<T>>::serialize(std::tuple<T>(value));
            }

            m_variant.reset(g_variant_ref(variant));
        }
        catch (const std::exception &err) {
            THROW_GIO_DBUS_CPP_ERROR(
                std::string("Failed to construct Gio::DBus::Message from value of type T (aka ")
                + DBusType<T>::class_name.data() + " aka " + DBusType<T>::name.data()
                + ") using Gio::DBus::Message::Message<T>(const T &)" + " (" + err.what() + ")");
        }
    }

    template<typename T>
    bool contains_value_of_type() const
    {
        using namespace Details;

        static_assert(is_dbus_type_v<T>,
                      "Attempt to check whether Gio::DBus::Message stores a value of type T using "
                      "Gio::DBus::Message::is<T>(), but T is not a dbus type");

        return g_variant_is_of_type(as_gio_variant(), dbus_type_to_variant_type_v<T>);
    }

    template<typename T>
    T as() const
    {
        using namespace Details;

        static_assert(is_dbus_type_v<T>,
                      "Attempt to read a value of type T using Gio::DBus::Message::as<T>(), "
                      "but T is not a dbus type");

        if constexpr (is_tuple_type_v<T>) {
            if (!contains_value_of_type<T>()) {
                THROW_GIO_DBUS_CPP_ERROR(std::string("Attempt to read a value of type T (aka ")
                                         + DBusType<T>::class_name.data() + " aka "
                                         + DBusType<T>::name.data()
                                         + ") using Gio::DBus::Message::as<T>(), "
                                           "but Gio::DBus::Message contains value of type "
                                         + dbus_type_signature());
            }
        } else {
            if (!contains_value_of_type<std::tuple<T>>()) {
                THROW_GIO_DBUS_CPP_ERROR(
                    std::string("Attempt to read a value of type std::tuple<T> (aka std::tuple<")
                    + DBusType<T>::class_name.data() + "> aka (" + DBusType<T>::name.data()
                    + ")) using Gio::DBus::Message::as<T>(), "
                      "but Gio::DBus::Message contains value of type "
                    + dbus_type_signature());
            }
        }

        try {
            if constexpr (is_tuple_type_v<T>) {
                return DBusDeserializer<T>::deserialize(as_gio_variant());
            } else {
                return std::get<0>(DBusDeserializer<std::tuple<T>>::deserialize(as_gio_variant()));
            }
        }
        catch (const std::exception &err) {
            THROW_GIO_DBUS_CPP_ERROR(std::string("Failed to read a value of type T (aka ")
                                     + DBusType<T>::class_name.data() + " aka "
                                     + DBusType<T>::name.data()
                                     + ") using Gio::DBus::Message::as<T>() (" + err.what() + ")");
        }
    }

    template<typename T>
    operator T() const /* NOLINT(google-explicit-constructor) */
    {
        return as<T>();
    }

private:
    friend class ProxyImpl;

    Message(GVariant *variant) /* NOLINT(google-explicit-constructor) */
        : m_variant(variant, &g_variant_unref)
    {
        if (!g_variant_is_of_type(variant, G_VARIANT_TYPE_TUPLE)) {
            THROW_GIO_DBUS_CPP_ERROR("Attempt to construct Gio::DBus::Message using a GVariant "
                                     "that is not a tuple type");
        }
    }

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

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_MESSAGE_HPP */
