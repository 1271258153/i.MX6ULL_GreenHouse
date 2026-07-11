
add_library(Qt5::QTsLibPlugin MODULE IMPORTED)

_populate_Gui_plugin_properties(QTsLibPlugin RELEASE "generic/libqtslibplugin.so")

list(APPEND Qt5Gui_PLUGINS Qt5::QTsLibPlugin)
