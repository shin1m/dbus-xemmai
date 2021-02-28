#include "connection.h"

namespace xemmaix::dbus
{

std::string f_convert(std::wstring_view a_x)
{
	return t_session::v_instance->v_convert.to_bytes(a_x.data(), a_x.data() + a_x.size());
}

std::wstring f_convert(std::string_view a_x)
{
	return t_session::v_instance->v_convert.from_bytes(a_x.data(), a_x.data() + a_x.size());
}

t_entry::t_entry() : v_previous(t_session::f_instance()), v_next(v_previous->v_next)
{
	v_previous->v_next = v_next->v_previous = this;
}

void t_entry::f_dispose()
{
	f_unlink();
}

XEMMAI__PORTABLE__THREAD t_session* t_session::v_instance;

#ifdef _WIN32
t_session* t_session::f_instance()
{
	if (!v_instance) f_throw(L"must be inside main."sv);
	return v_instance;
}
#endif

t_session::t_session(t_extension* a_extension) : t_entry(false), v_extension(a_extension)
{
	if (v_instance) f_throw(L"already inside main."sv);
	v_instance = this;
	dbus_message_allocate_data_slot(&t_message::v_slot);
	dbus_pending_call_allocate_data_slot(&t_reply::v_slot);
	dbus_connection_allocate_data_slot(&t_connection::v_slot);
}

t_session::~t_session()
{
	while (v_next != this) v_next->f_dispose();
	dbus_message_free_data_slot(&t_message::v_slot);
	dbus_pending_call_free_data_slot(&t_reply::v_slot);
	dbus_connection_free_data_slot(&t_connection::v_slot);
//	dbus_shutdown();
	v_instance = nullptr;
}

void t_proxy::f_destroy()
{
	if (v_previous) f_unlink();
	v_object = nullptr;
}

namespace
{

void f_main(t_extension* a_extension, const t_pvalue& a_callable)
{
	t_session session(a_extension);
	a_callable();
}

}

t_extension::t_extension(t_object* a_module) : xemmai::t_extension(a_module)
{
	t_type_of<t_message>::f_define(this);
	t_type_of<t_reply>::f_define(this);
	t_type_of<DBusBusType>::f_define(this);
	t_type_of<t_connection>::f_define(this);
	f_define<void(*)(t_extension*, const t_pvalue&), f_main>(this, L"main"sv);
	a_module->f_put(t_symbol::f_instantiate(L"TIMEOUT_INFINITE"sv), f_as(DBUS_TIMEOUT_INFINITE));
	a_module->f_put(t_symbol::f_instantiate(L"TIMEOUT_USE_DEFAULT"sv), f_as(DBUS_TIMEOUT_USE_DEFAULT));
	a_module->f_put(t_symbol::f_instantiate(L"LITTLE_ENDIAN"sv), f_as(static_cast<int>(DBUS_LITTLE_ENDIAN)));
	a_module->f_put(t_symbol::f_instantiate(L"BIG_ENDIAN"sv), f_as(static_cast<int>(DBUS_BIG_ENDIAN)));
	a_module->f_put(t_symbol::f_instantiate(L"MAJOR_PROTOCOL_VERSION"sv), f_as(DBUS_MAJOR_PROTOCOL_VERSION));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_INVALID"sv), f_as(DBUS_TYPE_INVALID));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_BYTE"sv), f_as(DBUS_TYPE_BYTE));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_BOOLEAN"sv), f_as(DBUS_TYPE_BOOLEAN));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_INT16"sv), f_as(DBUS_TYPE_INT16));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_UINT16"sv), f_as(DBUS_TYPE_UINT16));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_INT32"sv), f_as(DBUS_TYPE_INT32));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_UINT32"sv), f_as(DBUS_TYPE_UINT32));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_INT64"sv), f_as(DBUS_TYPE_INT64));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_UINT64"sv), f_as(DBUS_TYPE_UINT64));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_DOUBLE"sv), f_as(DBUS_TYPE_DOUBLE));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_STRING"sv), f_as(DBUS_TYPE_STRING));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_OBJECT_PATH"sv), f_as(DBUS_TYPE_OBJECT_PATH));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_SIGNATURE"sv), f_as(DBUS_TYPE_SIGNATURE));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_UNIX_FD"sv), f_as(DBUS_TYPE_UNIX_FD));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_ARRAY"sv), f_as(DBUS_TYPE_ARRAY));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_VARIANT"sv), f_as(DBUS_TYPE_VARIANT));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_STRUCT"sv), f_as(DBUS_TYPE_STRUCT));
	a_module->f_put(t_symbol::f_instantiate(L"TYPE_DICT_ENTRY"sv), f_as(DBUS_TYPE_DICT_ENTRY));
	a_module->f_put(t_symbol::f_instantiate(L"NUMBER_OF_TYPES"sv), f_as(DBUS_NUMBER_OF_TYPES));
	a_module->f_put(t_symbol::f_instantiate(L"STRUCT_BEGIN_CHAR"sv), f_as(DBUS_STRUCT_BEGIN_CHAR));
	a_module->f_put(t_symbol::f_instantiate(L"STRUCT_END_CHAR"sv), f_as(DBUS_STRUCT_END_CHAR));
	a_module->f_put(t_symbol::f_instantiate(L"DICT_ENTRY_BEGIN_CHAR"sv), f_as(DBUS_DICT_ENTRY_BEGIN_CHAR));
	a_module->f_put(t_symbol::f_instantiate(L"DICT_ENTRY_END_CHAR"sv), f_as(DBUS_DICT_ENTRY_END_CHAR));
	a_module->f_put(t_symbol::f_instantiate(L"MAXIMUM_NAME_LENGTH"sv), f_as(DBUS_MAXIMUM_NAME_LENGTH));
	a_module->f_put(t_symbol::f_instantiate(L"MAXIMUM_SIGNATURE_LENGTH"sv), f_as(DBUS_MAXIMUM_SIGNATURE_LENGTH));
	a_module->f_put(t_symbol::f_instantiate(L"MAXIMUM_MATCH_RULE_LENGTH"sv), f_as(DBUS_MAXIMUM_MATCH_RULE_LENGTH));
	a_module->f_put(t_symbol::f_instantiate(L"MAXIMUM_MATCH_RULE_ARG_NUMBER"sv), f_as(DBUS_MAXIMUM_MATCH_RULE_ARG_NUMBER));
	a_module->f_put(t_symbol::f_instantiate(L"MAXIMUM_ARRAY_LENGTH"sv), f_as(DBUS_MAXIMUM_ARRAY_LENGTH));
	a_module->f_put(t_symbol::f_instantiate(L"MAXIMUM_ARRAY_LENGTH_BITS"sv), f_as(DBUS_MAXIMUM_ARRAY_LENGTH_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"MAXIMUM_MESSAGE_LENGTH_BITS"sv), f_as(DBUS_MAXIMUM_MESSAGE_LENGTH_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"MAXIMUM_MESSAGE_LENGTH"sv), f_as(DBUS_MAXIMUM_MESSAGE_LENGTH));
	a_module->f_put(t_symbol::f_instantiate(L"MAXIMUM_MESSAGE_UNIX_FDS"sv), f_as(DBUS_MAXIMUM_MESSAGE_UNIX_FDS));
	a_module->f_put(t_symbol::f_instantiate(L"MAXIMUM_MESSAGE_UNIX_FDS_BITS"sv), f_as(DBUS_MAXIMUM_MESSAGE_UNIX_FDS_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"MAXIMUM_TYPE_RECURSION_DEPTH"sv), f_as(DBUS_MAXIMUM_TYPE_RECURSION_DEPTH));
	a_module->f_put(t_symbol::f_instantiate(L"MESSAGE_TYPE_INVALID"sv), f_as(DBUS_MESSAGE_TYPE_INVALID));
	a_module->f_put(t_symbol::f_instantiate(L"MESSAGE_TYPE_METHOD_CALL"sv), f_as(DBUS_MESSAGE_TYPE_METHOD_CALL));
	a_module->f_put(t_symbol::f_instantiate(L"MESSAGE_TYPE_METHOD_RETURN"sv), f_as(DBUS_MESSAGE_TYPE_METHOD_RETURN));
	a_module->f_put(t_symbol::f_instantiate(L"MESSAGE_TYPE_ERROR"sv), f_as(DBUS_MESSAGE_TYPE_ERROR));
	a_module->f_put(t_symbol::f_instantiate(L"MESSAGE_TYPE_SIGNAL"sv), f_as(DBUS_MESSAGE_TYPE_SIGNAL));
	a_module->f_put(t_symbol::f_instantiate(L"NUM_MESSAGE_TYPES"sv), f_as(DBUS_NUM_MESSAGE_TYPES));
	a_module->f_put(t_symbol::f_instantiate(L"SERVICE_DBUS"sv), f_as(std::wstring(XEMMAI__MACRO__L(DBUS_SERVICE_DBUS))));
	a_module->f_put(t_symbol::f_instantiate(L"PATH_DBUS"sv), f_as(std::wstring(XEMMAI__MACRO__L(DBUS_PATH_DBUS))));
	a_module->f_put(t_symbol::f_instantiate(L"PATH_LOCAL"sv), f_as(std::wstring(XEMMAI__MACRO__L(DBUS_PATH_LOCAL))));
	a_module->f_put(t_symbol::f_instantiate(L"INTERFACE_DBUS"sv), f_as(std::wstring(XEMMAI__MACRO__L(DBUS_INTERFACE_DBUS))));
	a_module->f_put(t_symbol::f_instantiate(L"INTERFACE_INTROSPECTABLE"sv), f_as(std::wstring(XEMMAI__MACRO__L(DBUS_INTERFACE_INTROSPECTABLE))));
	a_module->f_put(t_symbol::f_instantiate(L"INTERFACE_PROPERTIES"sv), f_as(std::wstring(XEMMAI__MACRO__L(DBUS_INTERFACE_PROPERTIES))));
	a_module->f_put(t_symbol::f_instantiate(L"INTERFACE_PEER"sv), f_as(std::wstring(XEMMAI__MACRO__L(DBUS_INTERFACE_PEER))));
	a_module->f_put(t_symbol::f_instantiate(L"INTERFACE_LOCAL"sv), f_as(std::wstring(XEMMAI__MACRO__L(DBUS_INTERFACE_LOCAL))));
	a_module->f_put(t_symbol::f_instantiate(L"NAME_FLAG_ALLOW_REPLACEMENT"sv), f_as(DBUS_NAME_FLAG_ALLOW_REPLACEMENT));
	a_module->f_put(t_symbol::f_instantiate(L"NAME_FLAG_REPLACE_EXISTING"sv), f_as(DBUS_NAME_FLAG_REPLACE_EXISTING));
	a_module->f_put(t_symbol::f_instantiate(L"NAME_FLAG_DO_NOT_QUEUE"sv), f_as(DBUS_NAME_FLAG_DO_NOT_QUEUE));
	a_module->f_put(t_symbol::f_instantiate(L"REQUEST_NAME_REPLY_PRIMARY_OWNER"sv), f_as(DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER));
	a_module->f_put(t_symbol::f_instantiate(L"REQUEST_NAME_REPLY_IN_QUEUE"sv), f_as(DBUS_REQUEST_NAME_REPLY_IN_QUEUE));
	a_module->f_put(t_symbol::f_instantiate(L"REQUEST_NAME_REPLY_EXISTS"sv), f_as(DBUS_REQUEST_NAME_REPLY_EXISTS));
	a_module->f_put(t_symbol::f_instantiate(L"REQUEST_NAME_REPLY_ALREADY_OWNER"sv), f_as(DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER));
	a_module->f_put(t_symbol::f_instantiate(L"RELEASE_NAME_REPLY_RELEASED"sv), f_as(DBUS_RELEASE_NAME_REPLY_RELEASED));
	a_module->f_put(t_symbol::f_instantiate(L"RELEASE_NAME_REPLY_NON_EXISTENT"sv), f_as(DBUS_RELEASE_NAME_REPLY_NON_EXISTENT));
	a_module->f_put(t_symbol::f_instantiate(L"RELEASE_NAME_REPLY_NOT_OWNER"sv), f_as(DBUS_RELEASE_NAME_REPLY_NOT_OWNER));
	a_module->f_put(t_symbol::f_instantiate(L"START_REPLY_SUCCESS"sv), f_as(DBUS_START_REPLY_SUCCESS));
	a_module->f_put(t_symbol::f_instantiate(L"START_REPLY_ALREADY_RUNNING"sv), f_as(DBUS_START_REPLY_ALREADY_RUNNING));
}

void t_extension::f_scan(t_scan a_scan)
{
	a_scan(v_type_message);
	a_scan(v_type_reply);
	a_scan(v_type_bus_type);
	a_scan(v_type_connection);
}

}

namespace xemmai
{

void t_type_of<DBusBusType>::f_define(t_extension* a_extension)
{
	t_define<DBusBusType, intptr_t>(a_extension, L"BusType"sv)
		(L"SESSION"sv, DBUS_BUS_SESSION)
		(L"SYSTEM"sv, DBUS_BUS_SYSTEM)
		(L"STARTER"sv, DBUS_BUS_STARTER)
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmaix::dbus::t_extension(a_module);
}
