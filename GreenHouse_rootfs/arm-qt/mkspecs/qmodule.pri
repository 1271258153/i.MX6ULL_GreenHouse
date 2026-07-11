EXTRA_INCLUDEPATH += /home/cx/Desktop/linux/tslib-master/arm-tslib/include
EXTRA_LIBDIR += /home/cx/Desktop/linux/tslib-master/arm-tslib/lib
host_build {
    QT_CPU_FEATURES.x86_64 = mmx sse sse2
} else {
    QT_CPU_FEATURES.arm = neon
}
QT.global_private.enabled_features = alloca_h alloca dbus gui network posix_fallocate reduce_exports release_tools sql testlib widgets xml
QT.global_private.disabled_features = sse2 alloca_malloc_h android-style-assets avx2 dbus-linked private_tests gc_binaries libudev reduce_relocations stack-protector-strong system-zlib
QT_COORD_TYPE = double
CONFIG += cross_compile compile_examples enable_new_dtags largefile neon precompile_header
QT_BUILD_PARTS += libs examples
QT_HOST_CFLAGS_DBUS += 
