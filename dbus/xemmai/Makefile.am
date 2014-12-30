xemmaidir = $(XEMMAI_MODULE_PATH)

xemmai_LTLIBRARIES = dbus.la
AM_CPPFLAGS = $(DBUS_CFLAGS) $(XEMMAI_CFLAGS)
AM_CXXFLAGS = -std=c++11
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
	cast.h \
	message.h \
	message.cc \
	reply.h \
	reply.cc \
	connection.h \
	connection.cc \
	dbus.h \
	dbus.cc
