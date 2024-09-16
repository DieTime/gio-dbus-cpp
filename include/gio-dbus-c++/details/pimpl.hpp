#ifndef GIO_DBUS_CPP_DETAILS_PIMPL_HPP
#define GIO_DBUS_CPP_DETAILS_PIMPL_HPP

#include "exception.hpp"

#include <memory>

/* NOLINTBEGIN(bugprone-macro-parentheses) */
#define GIO_DBUS_CPP_DECLARE_PIMPL_PARTS(Type, Impl) \
public:                                              \
    Type();                                          \
    ~Type();                                         \
                                                     \
    Type(Type &&) noexcept;                          \
    Type &operator=(Type &&) noexcept;               \
                                                     \
    Type(const Type &) = delete;                     \
    Type &operator=(const Type &) = delete;          \
                                                     \
private:                                             \
    std::unique_ptr<Impl> m_pimpl;

#define GIO_DBUS_CPP_IMPLEMENT_PIMPL_PARTS(Type, Impl) \
    Type::Type() = default;                            \
                                                       \
    Type::~Type() = default;                           \
                                                       \
    Type::Type(Type &&other) noexcept                  \
        : m_pimpl(std::move(other.m_pimpl))            \
    {}                                                 \
                                                       \
    Type &Type::operator=(Type &&other) noexcept       \
    {                                                  \
        if (this != &other) {                          \
            m_pimpl = std::move(other.m_pimpl);        \
        }                                              \
                                                       \
        return *this;                                  \
    }
/* NOLINTEND(bugprone-macro-parentheses) */

#endif /* GIO_DBUS_CPP_DETAILS_PIMPL_HPP */
