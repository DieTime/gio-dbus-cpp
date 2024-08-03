#ifndef GIO_DBUS_CPP_COMMON_HPP
#define GIO_DBUS_CPP_COMMON_HPP

#ifdef GIO_DBUS_CPP_BUILD_SHARED_LIBRARY
#define GIO_DBUS_CPP_EXPORT __attribute__((__visibility__("default")))
#else
#define GIO_DBUS_CPP_EXPORT
#endif

#ifndef GIO_DBUS_CPP_ERROR_NAME
#define GIO_DBUS_CPP_ERROR_NAME "gio.dbus.cpp.Error"
#endif

#endif /* GIO_DBUS_CPP_COMMON_HPP */
