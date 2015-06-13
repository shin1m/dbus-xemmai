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
	t_reply* p = static_cast<t_reply*>(a_this->f_pointer());
	assert(!*p);
	delete p;
}

t_scoped t_type_of<t_reply>::f_construct(t_object* a_class, t_scoped* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<t_reply>::f_instantiate(t_object* a_class, t_scoped* a_stack, size_t a_n)
{
	a_stack[0].f_construct(f_construct(a_class, a_stack, a_n));
	a_n += 2;
	for (size_t i = 2; i < a_n; ++i) a_stack[i] = nullptr;
}

size_t t_type_of<t_reply>::f_call(t_object* a_this, t_scoped* a_stack, size_t a_n)
{
	if (a_n > 0) t_throwable::f_throw(L"must be called without an argument.");
	t_native_context context;
	a_stack[0].f_construct(f_as<t_reply&>(a_this)());
	context.f_done();
	return -1;
}

}
