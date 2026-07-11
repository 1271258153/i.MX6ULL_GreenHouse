host_build {
    QT_ARCH = x86_64
    QT_BUILDABI = x86_64-little_endian-lp64
    QT_TARGET_ARCH = arm
    QT_TARGET_BUILDABI = arm-little_endian-ilp32-eabi-hardfloat
} else {
    QT_ARCH = arm
    QT_BUILDABI = arm-little_endian-ilp32-eabi-hardfloat
}
QT.global.enabled_features = cross_compile shared c++11 c99 c11 thread future concurrent
QT.global.disabled_features = framework rpath appstore-compliant debug_and_release simulator_and_device build_all c++14 c++1z force_asserts pkg-config separate_debug_info static
CONFIG += cross_compile shared release
QT_CONFIG += shared release c++11 concurrent dbus no-pkg-config reduce_exports release_tools stl
QT_VERSION = 5.12.9
QT_MAJOR_VERSION = 5
QT_MINOR_VERSION = 12
QT_PATCH_VERSION = 9
QT_GCC_MAJOR_VERSION = 4
QT_GCC_MINOR_VERSION = 9
QT_GCC_PATCH_VERSION = 4
QT_EDITION = OpenSource
