#ifndef XEMMAIX__DBUS__REPLY_H
#define XEMMAIX__DBUS__REPLY_H

#include "message.h"

namespace xemmaix::dbus
{

class t_reply : public t_proxy_of<t_reply, DBusPendingCall>
{
	friend class t_type_of<t_object>;
	friend class t_proxy_of<t_reply, DBusPendingCall>;

	static dbus_bool_t f_set_data(DBusPendingCall* a_value, dbus_int32_t a_slot, void* a_user, DBusFreeFunction a_destroy)
	{
		return dbus_pending_call_set_data(a_value, a_slot, a_user, a_destroy);
	}
	static void* f_get_data(DBusPendingCall* a_value, dbus_int32_t a_slot)
	{
		return dbus_pending_call_get_data(a_value, a_slot);
	}
	static DBusPendingCall* f_reference(DBusPendingCall* a_value)
	{
		return dbus_pending_call_ref(a_value);
	}
	static void f_unreference(DBusPendingCall* a_value)
	{
		dbus_pending_call_unref(a_value);
	}
	static t_pvalue f_steal(DBusPendingCall* a_value)
	{
		auto message = dbus_pending_call_steal_reply(a_value);
		if (message == NULL) f_throw(L"dbus_pending_call_steal_reply failed."sv);
		return t_message::f_construct(message);
	}

	DBusConnection* v_connection;

	t_reply(DBusPendingCall* a_value, DBusConnection* a_connection) : t_base(a_value), v_connection(a_connection)
	{
		dbus_connection_ref(v_connection);
	}
	virtual void f_destroy();

public:
	static t_pvalue f_construct(DBusPendingCall* a_value, DBusConnection* a_connection)
	{
		return a_value == NULL ? nullptr : f_transfer(f_new<t_reply>(t_session::f_instance()->f_library(), a_value, a_connection));
	}

	virtual void f_dispose();
	void f_acquire()
	{
		t_base::f_acquire();
	}
	void f_release()
	{
		t_base::f_release();
	}
	t_pvalue operator()()
	{
		dbus_pending_call_block(v_value);
		return f_steal(v_value);
	}
	void operator()(const t_pvalue& a_callable)
	{
		if (dbus_pending_call_set_notify(v_value, [](auto a_pending, auto a_data)
		{
			auto result = f_steal(a_pending);
			try {
				(*static_cast<t_rvalue*>(a_data))(result);
				f_as<t_message&>(result).f_release();
			} catch (...) {
				f_as<t_message&>(result).f_release();
				throw;
			}
		}, new t_rvalue(a_callable), [](auto a_data)
		{
			delete static_cast<t_rvalue*>(a_data);
		}) == FALSE) f_throw(L"dbus_pending_call_set_notify failed."sv);
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::dbus::t_reply> : t_uninstantiatable<xemmaix::dbus::t_holds<xemmaix::dbus::t_reply>>
{
	static void f_define(t_library* a_library);

	using t_base::t_base;
	static size_t f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n);
};

}

#endif
