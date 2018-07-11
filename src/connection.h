#ifndef XEMMAIX__DBUS__CONNECTION_H
#define XEMMAIX__DBUS__CONNECTION_H

#include <set>

#include "reply.h"

namespace xemmaix::dbus
{

class t_connection : public t_proxy_of<t_connection, DBusConnection>
{
	friend class t_proxy_of<t_connection, DBusConnection>;

	struct t_match
	{
		int v_type;
		std::string v_path;
		std::string v_interface;
		std::string v_member;

		t_match(int a_type, const std::string& a_path, const std::string& a_interface, const std::string& a_member = std::string()) : v_type(a_type), v_path(a_path), v_interface(a_interface), v_member(a_member)
		{
		}
		bool operator<(const t_match& a_match) const
		{
			if (v_type < a_match.v_type) return true;
			if (v_type > a_match.v_type) return false;
			if (v_path < a_match.v_path) return true;
			if (v_path > a_match.v_path) return false;
			if (v_interface < a_match.v_interface) return true;
			if (v_interface > a_match.v_interface) return false;
			return v_member < a_match.v_member;
		}
	};

	static dbus_bool_t f_set_data(DBusConnection* a_value, dbus_int32_t a_slot, void* a_user, DBusFreeFunction a_destroy)
	{
		return dbus_connection_set_data(a_value, a_slot, a_user, a_destroy);
	}
	static void* f_get_data(DBusConnection* a_value, dbus_int32_t a_slot)
	{
		return dbus_connection_get_data(a_value, a_slot);
	}
	static DBusConnection* f_reference(DBusConnection* a_value)
	{
		return dbus_connection_ref(a_value);
	}
	static void f_unreference(DBusConnection* a_value)
	{
		dbus_connection_close(a_value);
		dbus_connection_unref(a_value);
	}
	static DBusHandlerResult f_filter(DBusConnection* a_connection, DBusMessage* a_message, void* a_data);

	std::set<t_scoped> v_disconnecteds;
	std::map<t_match, t_scoped> v_matches;

	t_connection(DBusConnection* a_value) : t_base(t_session::f_instance()->f_extension()->f_type<t_connection>(), a_value)
	{
		if (dbus_connection_add_filter(v_value, f_filter, this, NULL) == FALSE) t_throwable::f_throw(L"dbus_connection_add_filter failed.");
	}
	virtual void f_destroy();

public:
	static t_connection* f_wrap(DBusConnection* a_value)
	{
		if (!a_value) return nullptr;
		t_connection* p = f_from(a_value);
		return p ? p : new t_connection(a_value);
	}
	static t_scoped f_construct(t_type* a_class, DBusBusType a_type)
	{
		DBusError error;
		dbus_error_init(&error);
		DBusConnection* p = dbus_bus_get_private(a_type, &error);
		if (p == NULL) {
			std::wstring s = L"dbus_bus_get_private failed: " + f_convert(error.name) + L", " + f_convert(error.message);
			dbus_error_free(&error);
			t_throwable::f_throw(s);
		}
		dbus_connection_set_exit_on_disconnect(p, FALSE);
		return f_construct_shared<t_connection>(p);
	}
	static t_scoped f_construct(t_type* a_class, const std::wstring& a_address)
	{
		DBusError error;
		dbus_error_init(&error);
		DBusConnection* p = dbus_connection_open_private(f_convert(a_address).c_str(), &error);
		if (p == NULL) {
			std::wstring s = L"dbus_connection_open_private failed: " + f_convert(error.name) + L", " + f_convert(error.message);
			dbus_error_free(&error);
			t_throwable::f_throw(s);
		}
		return f_construct_shared<t_connection>(p);
	}

	void f_acquire()
	{
		t_base::f_acquire();
	}
	void f_release()
	{
		t_base::f_release();
	}
	void f_send(t_message& a_message)
	{
		if (dbus_connection_send(v_value, a_message, NULL) == FALSE) t_throwable::f_throw(L"dbus_connection_send failed.");
	}
	t_scoped f_send_with_reply(t_message& a_message)
	{
		DBusPendingCall* p;
		if (dbus_connection_send_with_reply(v_value, a_message, &p, DBUS_TIMEOUT_USE_DEFAULT) == FALSE) t_throwable::f_throw(L"dbus_connection_send_with_reply failed.");
		return t_reply::f_construct(p, v_value);
	}
	void f_add_disconnected(t_scoped&& a_callable)
	{
		v_disconnecteds.insert(a_callable);
	}
	void f_remove_disconnected(const t_value& a_callable)
	{
		v_disconnecteds.erase(a_callable);
	}
	int f_request_name(const std::wstring& a_name, unsigned int a_flags)
	{
		DBusError error;
		dbus_error_init(&error);
		int result = dbus_bus_request_name(v_value, f_convert(a_name).c_str(), a_flags, &error);
		if (result == -1) {
			std::wstring s = L"dbus_bus_request_name failed: " + f_convert(error.name) + L", " + f_convert(error.message);
			dbus_error_free(&error);
			t_throwable::f_throw(s);
		}
		return result;
	}
	int f_release_name(const std::wstring& a_name)
	{
		DBusError error;
		dbus_error_init(&error);
		int result = dbus_bus_release_name(v_value, f_convert(a_name).c_str(), &error);
		if (result == -1) {
			std::wstring s = L"dbus_bus_release_name failed: " + f_convert(error.name) + L", " + f_convert(error.message);
			dbus_error_free(&error);
			t_throwable::f_throw(s);
		}
		return result;
	}
	void f_add_match(int a_type, const std::wstring& a_path, const std::wstring& a_interface, const std::wstring& a_member, t_scoped&& a_callable);
	void f_remove_match(int a_type, const std::wstring& a_path, const std::wstring& a_interface, const std::wstring& a_member);
};

class t_container_builder
{
	DBusMessageIter& v_parent;
	DBusMessageIter v_i;

public:
	t_container_builder(DBusMessageIter& a_parent, int a_type, const char* a_signature) : v_parent(a_parent)
	{
		dbus_message_iter_open_container(&v_parent, a_type, a_signature, &v_i);
	}
	~t_container_builder()
	{
		dbus_message_iter_close_container(&v_parent, &v_i);
	}
	operator DBusMessageIter&()
	{
		return v_i;
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::dbus::t_connection> : xemmaix::dbus::t_holds<xemmaix::dbus::t_connection>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif
