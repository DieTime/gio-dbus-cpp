#ifndef GIO_DBUS_CPP_COMMON_HPP
#define GIO_DBUS_CPP_COMMON_HPP

#ifdef GIO_DBUS_CPP_BUILD_SHARED_LIBRARY
#define GIO_DBUS_CPP_EXPORT __attribute__((__visibility__("default")))
#else
#define GIO_DBUS_CPP_EXPORT
#endif

#endif /* GIO_DBUS_CPP_COMMON_HPP */
