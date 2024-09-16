#include <gio-dbus-c++/gio-dbus-c++.hpp>
#include <iostream>

std::ostream &operator<<(std::ostream &stream, const Gio::DBus::Connection &connection)
{
    std::cout << "Connection:" << std::endl
              << "   - Name: '" << connection.unique_name() << "'" << std::endl;

    return stream;
}

std::ostream &operator<<(std::ostream &stream, const Gio::DBus::Proxy &proxy)
{
    std::cout << "Proxy:" << std::endl
              << "   - Service: '" << proxy.service() << "'" << std::endl
              << "   - Object: '" << proxy.object() << "'" << std::endl
              << "   - Interface: '" << proxy.interface() << "'" << std::endl;

    return stream;
}

std::ostream &operator<<(std::ostream &stream, const Gio::DBus::Error &error)
{
    std::cout << "Error:" << std::endl
              << "   - Name: '" << error.name() << "'" << std::endl
              << "   - Message: '" << error.message() << "'" << std::endl;

    return stream;
}

void on_name_owner_changed(const std::tuple<std::string, std::string, std::string> &data)
{
    std::cout << "NameOwnerChanged:" << std::endl
              << "   - Name: '" << std::get<0>(data) << "'" << std::endl
              << "   - Previous: '" << std::get<1>(data) << "'" << std::endl
              << "   - New: '" << std::get<2>(data) << "'" << std::endl
              << std::endl;
}

int main()
{
    Gio::DBus::Connection connection;
    Gio::DBus::Proxy proxy;
    Gio::DBus::Subscription subscription;

    try {
        connection = Gio::DBus::Connection(Gio::DBus::ConnectionType::Session);
        proxy = Gio::DBus::Proxy(connection,
                                 "org.freedesktop.DBus",
                                 "/org/freedesktop/DBus",
                                 "org.freedesktop.DBus");

        std::cout << connection << std::endl;
        std::cout << proxy << std::endl;

        std::vector<std::string> names = proxy.call("ListNames");

        std::cout << "Services:" << std::endl;
        for (const auto &name: names) {
            std::cout << "  - '" << name << "'" << std::endl;
        }

        subscription = proxy.subscribe_to_signal("NameOwnerChanged", on_name_owner_changed);
    }
    catch (const Gio::DBus::Error &error) {
        std::cerr << error << std::endl;
    }

    return 0;
}
