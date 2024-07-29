#ifndef GIO_DBUS_CPP_MESSAGE_HPP
#define GIO_DBUS_CPP_MESSAGE_HPP

#include "details/dbus-type-deserializer.hpp"
#include "details/dbus-type-serializer.hpp"
#include "details/dbus-type-traits.hpp"
#include "details/exception.hpp"

#include <gio/gio.h>
#include <memory>

namespace Gio::DBus {

class Message
{
public:
    template<typename T>
    Message(const T &value) /* NOLINT(google-explicit-constructor) */
    {
        static_assert(Details::is_dbus_type_v<T>(),
                      "Attempt to construct Gio::DBus::Message from value of type T using "
                      "Gio::DBus::Message::Message<T>(const T &), but T is not a dbus type");

        try {
            m_variant = std::make_unique<GVariant, &g_variant_unref>(
                Details::DBusTypeSerializer<T>::serialize(value), &g_variant_unref);
        }
        catch (const std::exception &err) {
            THROW_GIO_DBUS_CPP_ERROR(
                "Failed to construct Gio::DBus::Message from value of type T (aka "
                + Details::DBusType<T>::class_name.data() + " aka "
                + Details::DBusType<T>::name.data()
                + ") using Gio::DBus::Message::Message<T>(const T &)" + " (" + err.what() + ")");
        }
    }

    Message(GVariant *variant) /* NOLINT(google-explicit-constructor) */
        : m_variant(variant, &g_variant_unref)
    {}

    std::string_view type() const noexcept
    {
        const char *type = g_variant_get_type_string(m_variant.get());

        if (!type) {
            return {"unknown"};
        }

        return {type};
    }

    template<typename T>
    bool is() const
    {
        static_assert(Details::is_dbus_type_v<T>,
                      "Attempt to check whether Gio::DBus::Message stores a value of type T using "
                      "Gio::DBus::Message::is<T>(), but T is not a dbus type");

        return g_variant_is_of_type(const_cast<GVariant *>(m_variant.get()),
                                    Details::dbus_type_to_variant_type_v<T>);
    }

    template<typename T>
    T as() const
    {
        static_assert(Details::is_dbus_type_v<T>,
                      "Attempt to read a value of type T using Gio::DBus::Message::as<T>(), "
                      "but T is not a dbus type");

        if (!is<T>()) {
            THROW_GIO_DBUS_CPP_ERROR(std::string("Attempt to read a value of type T (aka ")
                                     + Details::DBusType<T>::class_name.data() + " aka "
                                     + Details::DBusType<T>::name.data()
                                     + ") using Gio::DBus::Message::as<T>(), "
                                       "but Gio::DBus::Message contains value of type "
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
                                     + ") using Gio::DBus::Message::as<T>() (" + err.what() + ")");
        }
    }

private:
    std::unique_ptr<GVariant, decltype(&g_variant_unref)> m_variant;
};

} /* namespace Gio::DBus */

#endif /* GIO_DBUS_CPP_MESSAGE_HPP */
