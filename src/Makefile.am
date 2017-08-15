xemmaiincludedir = $(XEMMAI_INCLUDE_PATH)/xemmaix/dbus

xemmaiinclude_HEADERS = \
	cast.h \
	message.h \
	reply.h \
	connection.h \
	dbus.h

xemmaimoduledir = $(XEMMAI_MODULE_PATH)

xemmaimodule_LTLIBRARIES = dbus.la
AM_CPPFLAGS = $(DBUS_CFLAGS) $(XEMMAI_CFLAGS)
AM_CXXFLAGS =
AM_LDFLAGS = -module -avoid-version
if DEBUG
AM_CXXFLAGS += -g
else
AM_CPPFLAGS += -DNDEBUG
AM_CXXFLAGS += -O3
endif
if PROFILE
AM_CXXFLAGS += -pg
AM_LDFLAGS += -pg
endif
dbus_la_LIBADD = $(DBUS_LIBS) $(XEMMAI_LIBS)
dbus_la_SOURCES = \
	message.cc \
	reply.cc \
	connection.cc \
	dbus.cc
