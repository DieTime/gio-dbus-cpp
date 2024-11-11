#ifndef GIO_DBUS_CPP_CONTEXT_TYPE_HPP
#define GIO_DBUS_CPP_CONTEXT_TYPE_HPP

namespace Gio {

enum ContextType {
    Global = 0,
    ThreadDefault,
    New,
    NewAsThreadDefault,
};

} /* namespace Gio */

#endif /* GIO_DBUS_CPP_CONTEXT_TYPE_HPP */
