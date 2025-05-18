## Settings for using graphviz with CMake.
## See: https://cmake.org/cmake/help/latest/module/CMakeGraphVizOptions.html
##
## Use as configuring parameter like:
##
## $ cmake --graphviz=libarcstk.dot && dot -Tpng libarcstk.dot -o libarcstk.png
##
## Debug-/Development only. Do not use in production builds!

## Activate dot-graph for target dependencies

set (GRAPHVIZ_CUSTOM_TARGETS ON )

## Leave defaults untouched

#set (GRAPHVIZ_EXECUTABLES    OFF )
#set (GRAPHVIZ_STATIC_LIBS    OFF )
#set (GRAPHVIZ_MODULE_LIBS    OFF )
#set (GRAPHVIZ_OBJECT_LIBS    OFF )
#set (GRAPHVIZ_INTERFACE_LIBS OFF )
#set (GRAPHVIZ_UNKNOWN_LIBS   OFF )

