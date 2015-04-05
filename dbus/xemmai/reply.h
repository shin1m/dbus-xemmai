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
		DBusMessage* message = dbus_pending_call_steal_reply(v_value);
		if (message == NULL) t_throwable::f_throw(L"dbus_pending_call_steal_reply failed.");
		return t_message::f_construct(message);
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
	virtual t_scoped f_construct(t_object* a_class, t_scoped* a_stack, size_t a_n);
	virtual void f_instantiate(t_object* a_class, t_scoped* a_stack, size_t a_n);
	virtual size_t f_call(t_object* a_this, t_scoped* a_stack, size_t a_n);
};

}

#endif
