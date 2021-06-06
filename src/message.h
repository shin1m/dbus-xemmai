#ifndef XEMMAIX__DBUS__MESSAGE_H
#define XEMMAIX__DBUS__MESSAGE_H

#include "dbus.h"

namespace xemmaix::dbus
{

class t_message : public t_proxy_of<t_message, DBusMessage>
{
	friend class t_type_of<t_object>;
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

	DBusMessageIter* v_i = nullptr;

	using t_base::t_base;
	void f_get(t_list& a_list, DBusMessageIter& a_i);

public:
	static t_object* f_wrap(DBusMessage* a_value)
	{
		if (!a_value) return {};
		t_message* p = f_from(a_value);
		if (p) return t_object::f_of(p);
		return f_new<t_message>(t_session::f_instance()->f_library(), a_value);
	}
	using t_base::f_construct;
	static t_pvalue f_construct(t_type* a_class, const t_string* a_destination, std::wstring_view a_path, const t_string* a_interface, std::wstring_view a_method)
	{
		DBusMessage* p = dbus_message_new_method_call(a_destination ? f_convert(*a_destination).c_str() : NULL, f_convert(a_path).c_str(), a_interface ? f_convert(*a_interface).c_str() : NULL, f_convert(a_method).c_str());
		if (p == NULL) f_throw(L"dbus_message_new_method_call failed."sv);
		return f_construct(a_class, p);
	}
	static t_pvalue f_construct(t_type* a_class, t_message& a_call)
	{
		DBusMessage* p = dbus_message_new_method_return(a_call);
		if (p == NULL) f_throw(L"dbus_message_new_method_return failed."sv);
		return f_construct(a_class, p);
	}
	static t_pvalue f_construct(t_type* a_class, std::wstring_view a_path, std::wstring_view a_interface, std::wstring_view a_name)
	{
		DBusMessage* p = dbus_message_new_signal(f_convert(a_path).c_str(), f_convert(a_interface).c_str(), f_convert(a_name).c_str());
		if (p == NULL) f_throw(L"dbus_message_new_signal failed."sv);
		return f_construct(a_class, p);
	}
	static t_pvalue f_construct(t_type* a_class, t_message& a_to, std::wstring_view a_name, const t_string* a_message)
	{
		DBusMessage* p = dbus_message_new_error(a_to, f_convert(a_name).c_str(), a_message ? f_convert(*a_message).c_str() : NULL);
		if (p == NULL) f_throw(L"dbus_message_new_error failed."sv);
		return f_construct(a_class, p);
	}
	static t_pvalue f_construct(DBusMessage* a_value)
	{
		return f_construct(t_session::f_instance()->f_library()->f_type<t_message>(), a_value);
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
	t_pvalue f_get();
	t_pvalue f_append(int a_type, const void* a_value)
	{
		if (v_i) {
			if (dbus_message_iter_append_basic(v_i, a_type, a_value) != TRUE) f_throw(L"dbus_message_iter_append_basic failed."sv);
		} else {
			if (dbus_message_append_args(v_value, a_type, a_value, DBUS_TYPE_INVALID) != TRUE) f_throw(L"dbus_message_append_args failed."sv);
		}
		return t_object::f_of(this);
	}
	t_pvalue f_append(bool a_value)
	{
		dbus_bool_t value = a_value ? TRUE : FALSE;
		return f_append(DBUS_TYPE_BOOLEAN, &value);
	}
	t_pvalue f_append(int a_type, intptr_t a_value)
	{
		switch (a_type) {
		case DBUS_TYPE_BYTE:
		case DBUS_TYPE_INT16:
		case DBUS_TYPE_UINT16:
		case DBUS_TYPE_INT32:
		case DBUS_TYPE_UINT32:
		case DBUS_TYPE_INT64:
		case DBUS_TYPE_UINT64:
			return f_append(a_type, &a_value);
		default:
			f_throw(L"invalid type."sv);
		}
	}
	t_pvalue f_append(intptr_t a_value)
	{
		return f_append(DBUS_TYPE_INT32, a_value);
	}
	t_pvalue f_append(double a_value)
	{
		return f_append(DBUS_TYPE_DOUBLE, &a_value);
	}
	t_pvalue f_append(int a_type, std::wstring_view a_value)
	{
		switch (a_type) {
		case DBUS_TYPE_STRING:
		case DBUS_TYPE_OBJECT_PATH:
		case DBUS_TYPE_SIGNATURE:
			break;
		default:
			f_throw(L"invalid type."sv);
		}
		std::string value = f_convert(a_value);
		const char* p = value.c_str();
		return f_append(a_type, &p);
	}
	t_pvalue f_append(std::wstring_view a_value)
	{
		return f_append(DBUS_TYPE_STRING, a_value);
	}
	t_pvalue f_append(int a_type, const char* a_signature, const t_pvalue& a_callable);
	t_pvalue f_append(int a_type, std::wstring_view a_signature, const t_pvalue& a_callable)
	{
		switch (a_type) {
		case DBUS_TYPE_ARRAY:
		case DBUS_TYPE_VARIANT:
			return f_append(a_type, f_convert(a_signature).c_str(), a_callable);
		default:
			f_throw(L"invalid type."sv);
		}
	}
	t_pvalue f_append(int a_type, const t_pvalue& a_callable)
	{
		switch (a_type) {
		case DBUS_TYPE_STRUCT:
		case DBUS_TYPE_DICT_ENTRY:
			return f_append(a_type, NULL, a_callable);
		default:
			f_throw(L"invalid type."sv);
		}
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::dbus::t_message> : xemmaix::dbus::t_holds<xemmaix::dbus::t_message>
{
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
