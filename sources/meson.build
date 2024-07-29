dependencies = [
    dependency('gio-unix-2.0'),
]

sources = [
    'connection.cpp',
    'error.cpp',
    'proxy.cpp',
    'timeout.cpp',
]

args = [
    '-DGIO_DBUS_CPP_BUILD_SHARED_LIBRARY'
]

gio_dbus_cpp = library(
    'gio-dbus-c++',
    sources,
    include_directories: ['../include/gio-dbus-c++'],
    dependencies: dependencies,
    cpp_args: args,
    gnu_symbol_visibility: 'hidden',
    version: meson.project_version(),
    soversion: 0,
)

gio_dbus_cpp_dep = declare_dependency(
    link_with: gio_dbus_cpp,
    dependencies: dependencies,
    include_directories: ['../include'],
)
