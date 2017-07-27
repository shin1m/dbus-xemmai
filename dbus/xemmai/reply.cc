#include "reply.h"

namespace dbus
{

namespace xemmai
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

}

namespace xemmai
{

void t_type_of<t_reply>::f_define(t_extension* a_extension)
{
	t_define<t_reply, t_object>(a_extension, L"Reply")
		(L"acquire", t_member<void (t_reply::*)(), &t_reply::f_acquire>())
		(L"release", t_member<void (t_reply::*)(), &t_reply::f_release>())
	;
}

t_type* t_type_of<t_reply>::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_type_of<t_reply>::f_finalize(t_object* a_this)
{
	auto p = static_cast<t_reply*>(a_this->f_pointer());
	assert(!*p);
	delete p;
}

t_scoped t_type_of<t_reply>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<t_reply>::f_instantiate(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	t_destruct_n destruct(a_stack, a_n);
	a_stack[0].f_construct(f_construct(a_class, a_stack, a_n));
}

size_t t_type_of<t_reply>::f_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
{
	if (a_n > 1) t_throwable::f_throw(a_stack, a_n, L"must be called with or without an argument.");
	if (a_n > 0) {
		f_as<t_reply&>(a_this)(std::move(a_stack[2]));
		a_stack[0].f_construct();
	} else {
		a_stack[0].f_construct(f_as<t_reply&>(a_this)());
	}
	return -1;
}

}
