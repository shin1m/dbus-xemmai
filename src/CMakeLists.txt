add_library(dbus MODULE message.cc reply.cc connection.cc dbus.cc)
target_link_libraries(dbus xemmai dbus-1)
set_property(TARGET dbus PROPERTY PREFIX "")
install(FILES message.h reply.h connection.h dbus.h DESTINATION include/xemmai-${xemmai_VERSION_MAJOR}/xemmaix/dbus)
