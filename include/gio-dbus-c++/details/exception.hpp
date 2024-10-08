#ifndef GIO_DBUS_CPP_DETAILS_EXCEPTION_HPP
#define GIO_DBUS_CPP_DETAILS_EXCEPTION_HPP

#include "../error.hpp"

#define THROW_GIO_DBUS_CPP_ERROR(MESSAGE) \
    throw Gio::DBus::Error(GIO_DBUS_CPP_ERROR_NAME, (MESSAGE));

#endif /* GIO_DBUS_CPP_DETAILS_EXCEPTION_HPP */
