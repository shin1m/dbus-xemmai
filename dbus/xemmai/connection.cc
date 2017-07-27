#include "connection.h"

#include <cassert>

namespace dbus
{

namespace xemmai
{

DBusHandlerResult t_connection::f_filter(DBusConnection* a_connection, DBusMessage* a_message, void* a_data)
{
std::fprintf(stderr, "filter: %d, %s, %s, %s\n", dbus_message_get_type(a_message), dbus_message_get_path(a_message), dbus_message_get_interface(a_message), dbus_message_get_member(a_message));
	auto p = static_cast<t_connection*>(a_data);
	const char* path = dbus_message_get_path(a_message);
	const char* interface = dbus_message_get_interface(a_message);
	const char* member = dbus_message_get_member(a_message);
	auto i = p->v_matches.find(t_match(dbus_message_get_type(a_message), path == NULL ? "" : path, interface == NULL ? "" : interface, member == NULL ? "" : member));
	if (i != p->v_matches.end()) {
		i->second(t_message::f_wrap(a_message)->f_object());
		return DBUS_HANDLER_RESULT_HANDLED;
	} else if (dbus_message_is_signal(a_message, DBUS_INTERFACE_LOCAL, "Disconnected") != FALSE) {
		for (const auto& q : p->v_disconnecteds) q();
		return DBUS_HANDLER_RESULT_HANDLED;
	}
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

void t_connection::f_destroy()
{
	v_disconnecteds.clear();
	v_matches.clear();
	t_base::f_destroy();
}

void t_connection::f_add_match(int a_type, const std::wstring& a_path, const std::wstring& a_interface, const std::wstring& a_member, t_scoped&& a_callable)
{
	std::string path = f_convert(a_path);
	std::string interface = f_convert(a_interface);
	std::string member = f_convert(a_member);
	if (!v_matches.emplace(t_match(a_type, path, interface, member), a_callable).second) return;
	std::string type = dbus_message_type_to_string(a_type);
	DBusError error;
	dbus_error_init(&error);
	dbus_bus_add_match(v_value, ("type='" + type + "',path='" + path + "',interface='" + interface + "',member='" + member + "'").c_str(), &error);
	if (dbus_error_is_set(&error) != FALSE) {
		std::wstring s = L"dbus_bus_add_match failed: " + f_convert(error.name) + L", " + f_convert(error.message);
		dbus_error_free(&error);
		t_throwable::f_throw(s);
	}
}

void t_connection::f_remove_match(int a_type, const std::wstring& a_path, const std::wstring& a_interface, const std::wstring& a_member)
{
	std::string path = f_convert(a_path);
	std::string interface = f_convert(a_interface);
	std::string member = f_convert(a_member);
	if (v_matches.erase(t_match(a_type, path, interface, member)) <= 0) return;
	std::string type = dbus_message_type_to_string(a_type);
	DBusError error;
	dbus_error_init(&error);
	dbus_bus_remove_match(v_value, ("type='" + type + "',path='" + path + "',interface='" + interface + "',member='" + member + "'").c_str(), &error);
	if (dbus_error_is_set(&error) != FALSE) {
		std::wstring s = L"dbus_bus_remove_match failed: " + f_convert(error.name) + L", " + f_convert(error.message);
		dbus_error_free(&error);
		t_throwable::f_throw(s);
	}
}

}

}

namespace xemmai
{

void t_type_of<t_connection>::f_define(t_extension* a_extension)
{
	t_define<t_connection, t_object>(a_extension, L"Connection")
		(L"acquire", t_member<void (t_connection::*)(), &t_connection::f_acquire>())
		(L"release", t_member<void (t_connection::*)(), &t_connection::f_release>())
		(L"send", t_member<void (t_connection::*)(t_message&), &t_connection::f_send>())
		(L"send_with_reply", t_member<t_scoped (t_connection::*)(t_message&), &t_connection::f_send_with_reply>())
		(L"add_disconnected", t_member<void (t_connection::*)(t_scoped&&), &t_connection::f_add_disconnected>())
		(L"remove_disconnected", t_member<void (t_connection::*)(const t_value&), &t_connection::f_remove_disconnected>())
		(L"request_name", t_member<int (t_connection::*)(const std::wstring&, unsigned int), &t_connection::f_request_name>())
		(L"release_name", t_member<int (t_connection::*)(const std::wstring&), &t_connection::f_release_name>())
		(L"add_match", t_member<void (t_connection::*)(int, const std::wstring&, const std::wstring&, const std::wstring&, t_scoped&&), &t_connection::f_add_match>())
		(L"remove_match", t_member<void (t_connection::*)(int, const std::wstring&, const std::wstring&, const std::wstring&), &t_connection::f_remove_match>())
	;
}

t_type* t_type_of<t_connection>::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_type_of<t_connection>::f_finalize(t_object* a_this)
{
	auto p = static_cast<t_connection*>(a_this->f_pointer());
	assert(!*p);
	delete p;
}

t_scoped t_type_of<t_connection>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_scoped (*)(t_object*, DBusBusType), t_connection::f_construct>,
		t_construct_with<t_scoped (*)(t_object*, const std::wstring&), t_connection::f_construct>
	>::t_bind<t_connection>::f_do(a_class, a_stack, a_n);
}

void t_type_of<t_connection>::f_instantiate(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	t_destruct_n destruct(a_stack, a_n);
	a_stack[0].f_construct(f_construct(a_class, a_stack, a_n));
}

}
