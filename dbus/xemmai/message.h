#ifndef DBUS__XEMMAI__MESSAGE_H
#define DBUS__XEMMAI__MESSAGE_H

#include "dbus.h"

namespace dbus
{

namespace xemmai
{

class t_message : public t_proxy_of<t_message, DBusMessage>
{
	friend class t_proxy_of<t_message, DBusMessage>;

	static dbus_bool_t f_set_data(DBusMessage* a_value, dbus_int32_t a_slot, void* a_user, DBusFreeFunction a_destroy)
	{
		return dbus_message_set_data(a_value, a_slot, a_user, a_destroy);
	}
	static void* f_get_data(DBusMessage* a_value, dbus_int32_t a_slot)
	{
		return dbus_message_get_data(a_value, a_slot);
	}
	static DBusMessage* f_reference(DBusMessage* a_value)
	{
		return dbus_message_ref(a_value);
	}
	static void f_unreference(DBusMessage* a_value)
	{
		dbus_message_unref(a_value);
	}

	t_message(DBusMessage* a_value) : t_base(t_session::f_instance()->f_extension()->f_type<t_message>(), a_value)
	{
	}
	void f_get(t_array& a_array, DBusMessageIter& a_i);

public:
	static t_message* f_wrap(DBusMessage* a_value)
	{
		if (!a_value) return nullptr;
		t_message* p = f_from(a_value);
		return p ? p : new t_message(a_value);
	}
	using t_base::f_construct;
	static t_scoped f_construct(t_object* a_class, const std::wstring* a_destination, const std::wstring& a_path, const std::wstring* a_interface, const std::wstring& a_method)
	{
		DBusMessage* p = dbus_message_new_method_call(a_destination ? f_convert(*a_destination).c_str() : NULL, f_convert(a_path).c_str(), a_interface ? f_convert(*a_interface).c_str() : NULL, f_convert(a_method).c_str());
		if (p == NULL) t_throwable::f_throw(L"dbus_message_new_method_call failed.");
		return f_construct(p);
	}
	static t_scoped f_construct(t_object* a_class, t_message& a_call)
	{
		DBusMessage* p = dbus_message_new_method_return(a_call);
		if (p == NULL) t_throwable::f_throw(L"dbus_message_new_method_return failed.");
		return f_construct(p);
	}
	static t_scoped f_construct(t_object* a_class, const std::wstring& a_path, const std::wstring& a_interface, const std::wstring& a_name)
	{
		DBusMessage* p = dbus_message_new_signal(f_convert(a_path).c_str(), f_convert(a_interface).c_str(), f_convert(a_name).c_str());
		if (p == NULL) t_throwable::f_throw(L"dbus_message_new_signal failed.");
		return f_construct(p);
	}
	static t_scoped f_construct(t_object* a_class, t_message& a_to, const std::wstring& a_name, const std::wstring* a_message)
	{
		DBusMessage* p = dbus_message_new_error(a_to, f_convert(a_name).c_str(), a_message ? f_convert(*a_message).c_str() : NULL);
		if (p == NULL) t_throwable::f_throw(L"dbus_message_new_error failed.");
		return f_construct(p);
	}

	void f_acquire()
	{
		t_base::f_acquire();
	}
	void f_release()
	{
		t_base::f_release();
	}
	int f_get_type() const
	{
		return dbus_message_get_type(v_value);
	}
	t_scoped f_get();
	t_scoped f_append(bool a_value)
	{
		dbus_bool_t value = a_value ? TRUE : FALSE;
		if (dbus_message_append_args(v_value, DBUS_TYPE_BOOLEAN, &value, DBUS_TYPE_INVALID) != TRUE) t_throwable::f_throw(L"dbus_message_append_args failed.");
		return f_object();
	}
	t_scoped f_append(int a_type, intptr_t a_value)
	{
		switch (a_type) {
		case DBUS_TYPE_BYTE:
		case DBUS_TYPE_INT16:
		case DBUS_TYPE_UINT16:
		case DBUS_TYPE_INT32:
		case DBUS_TYPE_UINT32:
		case DBUS_TYPE_INT64:
		case DBUS_TYPE_UINT64:
			break;
		default:
			t_throwable::f_throw(L"invalid type.");
		}
		if (dbus_message_append_args(v_value, a_type, &a_value, DBUS_TYPE_INVALID) != TRUE) t_throwable::f_throw(L"dbus_message_append_args failed.");
		return f_object();
	}
	t_scoped f_append(intptr_t a_value)
	{
		return f_append(DBUS_TYPE_INT32, a_value);
	}
	t_scoped f_append(double a_value)
	{
		if (dbus_message_append_args(v_value, DBUS_TYPE_DOUBLE, &a_value, DBUS_TYPE_INVALID) != TRUE) t_throwable::f_throw(L"dbus_message_append_args failed.");
		return f_object();
	}
	t_scoped f_append(int a_type, const std::wstring& a_value)
	{
		switch (a_type) {
		case DBUS_TYPE_STRING:
		case DBUS_TYPE_OBJECT_PATH:
		case DBUS_TYPE_SIGNATURE:
			break;
		default:
			t_throwable::f_throw(L"invalid type.");
		}
		std::string value = f_convert(a_value);
		const char* p = value.c_str();
		if (dbus_message_append_args(v_value, a_type, &p, DBUS_TYPE_INVALID) != TRUE) t_throwable::f_throw(L"dbus_message_append_args failed.");
		return f_object();
	}
	t_scoped f_append(const std::wstring& a_value)
	{
		return f_append(DBUS_TYPE_STRING, a_value);
	}
};

}

}

namespace xemmai
{

using dbus::xemmai::t_message;

template<>
struct t_type_of<t_message> : t_type
{
#include "cast.h"
	typedef dbus::xemmai::t_extension t_extension;

	static void f_define(t_extension* a_extension);

	using t_type::t_type;
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
