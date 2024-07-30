# gio-dbus-cpp

C++ DBus library based on [gio-2.0](https://docs.gtk.org/gio/index.html).

## Building

This project uses `meson` build systems.

```shell
$ meson setup _
$ meson compile -C _
```

## Linting

The `clang-format-15` and `clang-tidy-15` utilities must be installed.

```shell
$ scripts/lint-clang-format.sh
$ scripts/lint-clang-tidy.sh
$ scripts/lint-all.sh
```

If you have a PVS-Studio licence, you can also perform static code analysis.

```shell
$ scripts/lint-pvs-studio.sh
$ scripts/lint-extra.sh
```

## Supported platforms

At the moment, the library has only been tested on Linux platforms (Ubuntu 22.04).

## SAST Tools

[PVS-Studio](https://pvs-studio.com/en/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) - static analyzer for C, C++, C#, and Java code.
