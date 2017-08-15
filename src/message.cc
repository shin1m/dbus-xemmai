#include "message.h"

namespace xemmaix
{

namespace dbus
{

void t_message::f_get(t_array& a_array, DBusMessageIter& a_i)
{
	for (;; dbus_message_iter_next(&a_i)) {
		int type = dbus_message_iter_get_arg_type(&a_i);
		switch (type) {
		case DBUS_TYPE_INVALID:
			return;
		case DBUS_TYPE_ARRAY:
		case DBUS_TYPE_STRUCT:
		case DBUS_TYPE_DICT_ENTRY:
			{
				t_scoped x = t_array::f_instantiate();
				t_array& array = f_as<t_array&>(x);
				DBusMessageIter i;
				dbus_message_iter_recurse(&a_i, &i);
				f_get(array, i);
				a_array.f_push(std::move(x));
			}
			continue;
		case DBUS_TYPE_VARIANT:
			{
				DBusMessageIter i;
				dbus_message_iter_recurse(&a_i, &i);
				f_get(a_array, i);
			}
			continue;
		}
		DBusBasicValue value;
		dbus_message_iter_get_basic(&a_i, &value);
		switch (type) {
		case DBUS_TYPE_BYTE:
			a_array.f_push(t_scoped(value.byt));
			break;
		case DBUS_TYPE_BOOLEAN:
			a_array.f_push(t_scoped(value.bool_val));
			break;
		case DBUS_TYPE_INT16:
			a_array.f_push(t_scoped(value.i16));
			break;
		case DBUS_TYPE_UINT16:
			a_array.f_push(t_scoped(value.u16));
			break;
		case DBUS_TYPE_INT32:
			a_array.f_push(t_scoped(value.i32));
			break;
		case DBUS_TYPE_UINT32:
			a_array.f_push(t_scoped(value.u32));
			break;
		case DBUS_TYPE_INT64:
			a_array.f_push(t_scoped(value.i64));
			break;
		case DBUS_TYPE_UINT64:
			a_array.f_push(t_scoped(value.u64));
			break;
		case DBUS_TYPE_DOUBLE:
			a_array.f_push(t_scoped(value.dbl));
			break;
		case DBUS_TYPE_STRING:
		case DBUS_TYPE_OBJECT_PATH:
		case DBUS_TYPE_SIGNATURE:
			a_array.f_push(f_global()->f_as(f_convert(value.str)));
			break;
		case DBUS_TYPE_UNIX_FD:
			a_array.f_push(t_scoped(value.fd));
			break;
		}
	}
}

t_scoped t_message::f_get()
{
	t_scoped x = t_array::f_instantiate();
	auto& array = f_as<t_array&>(x);
	DBusMessageIter i;
	dbus_message_iter_init(v_value, &i);
	f_get(array, i);
	return x;
}

t_scoped t_message::f_append(int a_type, const char* a_signature, const t_value& a_callable)
{
	auto i = v_i;
	DBusMessageIter j;
	if (i) {
		if (dbus_message_iter_open_container(i, a_type, a_signature, &j) == FALSE) t_throwable::f_throw(L"dbus_message_iter_open_container failed.");
	} else {
		dbus_message_iter_init_append(v_value, &j);
	}
	v_i = &j;
	try {
		if (i) {
			a_callable(f_object());
			if (dbus_message_iter_close_container(i, &j) == FALSE) t_throwable::f_throw(L"dbus_message_iter_close_container failed.");
		} else {
			f_append(a_type, a_signature, a_callable);
		}
		v_i = i;
		return f_object();
	} catch (...) {
		if (i) dbus_message_iter_abandon_container(i, &j);
		v_i = i;
		throw;
	}
}

}

}

namespace xemmai
{

void t_type_of<xemmaix::dbus::t_message>::f_define(t_extension* a_extension)
{
	using namespace xemmaix::dbus;
	t_define<t_message, t_object>(a_extension, L"Message")
		(L"acquire", t_member<void(t_message::*)(), &t_message::f_acquire>())
		(L"release", t_member<void(t_message::*)(), &t_message::f_release>())
		(L"get_type", t_member<int(t_message::*)() const, &t_message::f_get_type>())
		(L"get", t_member<t_scoped(t_message::*)(), &t_message::f_get>())
		(L"append",
			t_member<t_scoped(t_message::*)(bool), &t_message::f_append>(),
			t_member<t_scoped(t_message::*)(int, intptr_t), &t_message::f_append>(),
			t_member<t_scoped(t_message::*)(intptr_t), &t_message::f_append>(),
			t_member<t_scoped(t_message::*)(double), &t_message::f_append>(),
			t_member<t_scoped(t_message::*)(int, const std::wstring&), &t_message::f_append>(),
			t_member<t_scoped(t_message::*)(const std::wstring&), &t_message::f_append>(),
			t_member<t_scoped(t_message::*)(int, const std::wstring&, const t_value&), &t_message::f_append>(),
			t_member<t_scoped(t_message::*)(int, const t_value&), &t_message::f_append>()
		)
	;
}

t_type* t_type_of<xemmaix::dbus::t_message>::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_type_of<xemmaix::dbus::t_message>::f_finalize(t_object* a_this)
{
	auto p = static_cast<xemmaix::dbus::t_message*>(a_this->f_pointer());
	assert(!*p);
	delete p;
}

t_scoped t_type_of<xemmaix::dbus::t_message>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_scoped(*)(t_object*, const std::wstring*, const std::wstring&, const std::wstring*, const std::wstring&), xemmaix::dbus::t_message::f_construct>,
		t_construct_with<t_scoped(*)(t_object*, xemmaix::dbus::t_message&), xemmaix::dbus::t_message::f_construct>,
		t_construct_with<t_scoped(*)(t_object*, const std::wstring&, const std::wstring&, const std::wstring&), xemmaix::dbus::t_message::f_construct>,
		t_construct_with<t_scoped(*)(t_object*, xemmaix::dbus::t_message&, const std::wstring&, const std::wstring*), xemmaix::dbus::t_message::f_construct>
	>::t_bind<xemmaix::dbus::t_message>::f_do(a_class, a_stack, a_n);
}

void t_type_of<xemmaix::dbus::t_message>::f_instantiate(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	t_destruct_n destruct(a_stack, a_n);
	a_stack[0].f_construct(f_construct(a_class, a_stack, a_n));
}

}