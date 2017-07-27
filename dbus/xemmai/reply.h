#ifndef DBUS__XEMMAI__REPLY_H
#define DBUS__XEMMAI__REPLY_H

#include "message.h"

namespace dbus
{

namespace xemmai
{

class t_reply : public t_proxy_of<t_reply, DBusPendingCall>
{
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
	static t_scoped f_steal(DBusPendingCall* a_value)
	{
		auto message = dbus_pending_call_steal_reply(a_value);
		if (message == NULL) t_throwable::f_throw(L"dbus_pending_call_steal_reply failed.");
		return t_message::f_construct(message);
	}

	DBusConnection* v_connection;

	t_reply(DBusPendingCall* a_value, DBusConnection* a_connection) : t_base(t_session::f_instance()->f_extension()->f_type<t_reply>(), a_value), v_connection(a_connection)
	{
		dbus_connection_ref(v_connection);
	}
	virtual void f_destroy();

public:
	static t_scoped f_construct(DBusPendingCall* a_value, DBusConnection* a_connection)
	{
		return a_value == NULL ? nullptr : f_transfer(new t_reply(a_value, a_connection));
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
	t_scoped operator()()
	{
		dbus_pending_call_block(v_value);
		return f_steal(v_value);
	}
	void operator()(t_scoped&& a_callable)
	{
		if (dbus_pending_call_set_notify(v_value, [](auto a_pending, auto a_data)
		{
			auto result = f_steal(a_pending);
			try {
				(*static_cast<t_scoped*>(a_data))(result);
				f_as<t_message&>(result).f_release();
			} catch (...) {
				f_as<t_message&>(result).f_release();
				throw;
			}
		}, new t_scoped(std::move(a_callable)), [](auto a_data)
		{
			delete static_cast<t_scoped*>(a_data);
		}) == FALSE) t_throwable::f_throw(L"dbus_pending_call_set_notify failed.");
	}
};

}

}

namespace xemmai
{

using dbus::xemmai::t_reply;

template<>
struct t_type_of<t_reply> : t_type
{
#include "cast.h"
	typedef dbus::xemmai::t_extension t_extension;

	static void f_define(t_extension* a_extension);

	using t_type::t_type;
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n);
	virtual void f_instantiate(t_object* a_class, t_stacked* a_stack, size_t a_n);
	virtual size_t f_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
};

}

#endif
