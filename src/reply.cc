#include "reply.h"

namespace xemmaix::dbus
{

void t_reply::f_destroy()
{
	dbus_connection_unref(v_connection);
	t_base::f_destroy();
}

void t_reply::f_dispose()
{
	dbus_pending_call_cancel(v_value);
	t_base::f_dispose();
}

}

namespace xemmai
{

void t_type_of<xemmaix::dbus::t_reply>::f_define(t_library* a_library)
{
	using namespace xemmaix::dbus;
	t_define{a_library}
	(L"acquire"sv, t_member<void(t_reply::*)(), &t_reply::f_acquire>())
	(L"release"sv, t_member<void(t_reply::*)(), &t_reply::f_release>())
	.f_derive<t_reply, t_object>();
}

size_t t_type_of<xemmaix::dbus::t_reply>::f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n)
{
	if (a_n > 1) f_throw(L"must be called with or without an argument."sv);
	if (a_n > 0) {
		f_as<xemmaix::dbus::t_reply&>(a_this)(a_stack[2]);
		a_stack[0] = nullptr;
	} else {
		a_stack[0] = f_as<xemmaix::dbus::t_reply&>(a_this)();
	}
	return -1;
}

}
