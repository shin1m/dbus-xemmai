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

t_session::t_session(t_library* a_library) : t_entry(false), v_library(a_library)
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

void f_main(t_library* a_library, const t_pvalue& a_callable)
{
	t_session session(a_library);
	a_callable();
}

}

void t_library::f_scan(t_scan a_scan)
{
	a_scan(v_type_message);
	a_scan(v_type_reply);
	a_scan(v_type_bus_type);
	a_scan(v_type_connection);
}

std::vector<std::pair<t_root, t_rvalue>> t_library::f_define()
{
	t_type_of<t_message>::f_define(this);
	t_type_of<t_reply>::f_define(this);
	t_type_of<t_connection>::f_define(this);
	return t_define(this)
	(L"Message"sv, static_cast<t_object*>(v_type_message))
	(L"Reply"sv, static_cast<t_object*>(v_type_reply))
	(L"BusType"sv, t_type_of<DBusBusType>::f_define(this))
	(L"Connection"sv, static_cast<t_object*>(v_type_connection))
	(L"main"sv, t_static<void(*)(t_library*, const t_pvalue&), f_main>())
	(L"TIMEOUT_INFINITE"sv, DBUS_TIMEOUT_INFINITE)
	(L"TIMEOUT_USE_DEFAULT"sv, DBUS_TIMEOUT_USE_DEFAULT)
	(L"LITTLE_ENDIAN"sv, static_cast<int>(DBUS_LITTLE_ENDIAN))
	(L"BIG_ENDIAN"sv, static_cast<int>(DBUS_BIG_ENDIAN))
	(L"MAJOR_PROTOCOL_VERSION"sv, DBUS_MAJOR_PROTOCOL_VERSION)
	(L"TYPE_INVALID"sv, DBUS_TYPE_INVALID)
	(L"TYPE_BYTE"sv, DBUS_TYPE_BYTE)
	(L"TYPE_BOOLEAN"sv, DBUS_TYPE_BOOLEAN)
	(L"TYPE_INT16"sv, DBUS_TYPE_INT16)
	(L"TYPE_UINT16"sv, DBUS_TYPE_UINT16)
	(L"TYPE_INT32"sv, DBUS_TYPE_INT32)
	(L"TYPE_UINT32"sv, DBUS_TYPE_UINT32)
	(L"TYPE_INT64"sv, DBUS_TYPE_INT64)
	(L"TYPE_UINT64"sv, DBUS_TYPE_UINT64)
	(L"TYPE_DOUBLE"sv, DBUS_TYPE_DOUBLE)
	(L"TYPE_STRING"sv, DBUS_TYPE_STRING)
	(L"TYPE_OBJECT_PATH"sv, DBUS_TYPE_OBJECT_PATH)
	(L"TYPE_SIGNATURE"sv, DBUS_TYPE_SIGNATURE)
	(L"TYPE_UNIX_FD"sv, DBUS_TYPE_UNIX_FD)
	(L"TYPE_ARRAY"sv, DBUS_TYPE_ARRAY)
	(L"TYPE_VARIANT"sv, DBUS_TYPE_VARIANT)
	(L"TYPE_STRUCT"sv, DBUS_TYPE_STRUCT)
	(L"TYPE_DICT_ENTRY"sv, DBUS_TYPE_DICT_ENTRY)
	(L"NUMBER_OF_TYPES"sv, DBUS_NUMBER_OF_TYPES)
	(L"STRUCT_BEGIN_CHAR"sv, DBUS_STRUCT_BEGIN_CHAR)
	(L"STRUCT_END_CHAR"sv, DBUS_STRUCT_END_CHAR)
	(L"DICT_ENTRY_BEGIN_CHAR"sv, DBUS_DICT_ENTRY_BEGIN_CHAR)
	(L"DICT_ENTRY_END_CHAR"sv, DBUS_DICT_ENTRY_END_CHAR)
	(L"MAXIMUM_NAME_LENGTH"sv, DBUS_MAXIMUM_NAME_LENGTH)
	(L"MAXIMUM_SIGNATURE_LENGTH"sv, DBUS_MAXIMUM_SIGNATURE_LENGTH)
	(L"MAXIMUM_MATCH_RULE_LENGTH"sv, DBUS_MAXIMUM_MATCH_RULE_LENGTH)
	(L"MAXIMUM_MATCH_RULE_ARG_NUMBER"sv, DBUS_MAXIMUM_MATCH_RULE_ARG_NUMBER)
	(L"MAXIMUM_ARRAY_LENGTH"sv, DBUS_MAXIMUM_ARRAY_LENGTH)
	(L"MAXIMUM_ARRAY_LENGTH_BITS"sv, DBUS_MAXIMUM_ARRAY_LENGTH_BITS)
	(L"MAXIMUM_MESSAGE_LENGTH_BITS"sv, DBUS_MAXIMUM_MESSAGE_LENGTH_BITS)
	(L"MAXIMUM_MESSAGE_LENGTH"sv, DBUS_MAXIMUM_MESSAGE_LENGTH)
	(L"MAXIMUM_MESSAGE_UNIX_FDS"sv, DBUS_MAXIMUM_MESSAGE_UNIX_FDS)
	(L"MAXIMUM_MESSAGE_UNIX_FDS_BITS"sv, DBUS_MAXIMUM_MESSAGE_UNIX_FDS_BITS)
	(L"MAXIMUM_TYPE_RECURSION_DEPTH"sv, DBUS_MAXIMUM_TYPE_RECURSION_DEPTH)
	(L"MESSAGE_TYPE_INVALID"sv, DBUS_MESSAGE_TYPE_INVALID)
	(L"MESSAGE_TYPE_METHOD_CALL"sv, DBUS_MESSAGE_TYPE_METHOD_CALL)
	(L"MESSAGE_TYPE_METHOD_RETURN"sv, DBUS_MESSAGE_TYPE_METHOD_RETURN)
	(L"MESSAGE_TYPE_ERROR"sv, DBUS_MESSAGE_TYPE_ERROR)
	(L"MESSAGE_TYPE_SIGNAL"sv, DBUS_MESSAGE_TYPE_SIGNAL)
	(L"NUM_MESSAGE_TYPES"sv, DBUS_NUM_MESSAGE_TYPES)
	(L"SERVICE_DBUS"sv, std::wstring(XEMMAI__MACRO__L(DBUS_SERVICE_DBUS)))
	(L"PATH_DBUS"sv, std::wstring(XEMMAI__MACRO__L(DBUS_PATH_DBUS)))
	(L"PATH_LOCAL"sv, std::wstring(XEMMAI__MACRO__L(DBUS_PATH_LOCAL)))
	(L"INTERFACE_DBUS"sv, std::wstring(XEMMAI__MACRO__L(DBUS_INTERFACE_DBUS)))
	(L"INTERFACE_INTROSPECTABLE"sv, std::wstring(XEMMAI__MACRO__L(DBUS_INTERFACE_INTROSPECTABLE)))
	(L"INTERFACE_PROPERTIES"sv, std::wstring(XEMMAI__MACRO__L(DBUS_INTERFACE_PROPERTIES)))
	(L"INTERFACE_PEER"sv, std::wstring(XEMMAI__MACRO__L(DBUS_INTERFACE_PEER)))
	(L"INTERFACE_LOCAL"sv, std::wstring(XEMMAI__MACRO__L(DBUS_INTERFACE_LOCAL)))
	(L"NAME_FLAG_ALLOW_REPLACEMENT"sv, DBUS_NAME_FLAG_ALLOW_REPLACEMENT)
	(L"NAME_FLAG_REPLACE_EXISTING"sv, DBUS_NAME_FLAG_REPLACE_EXISTING)
	(L"NAME_FLAG_DO_NOT_QUEUE"sv, DBUS_NAME_FLAG_DO_NOT_QUEUE)
	(L"REQUEST_NAME_REPLY_PRIMARY_OWNER"sv, DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
	(L"REQUEST_NAME_REPLY_IN_QUEUE"sv, DBUS_REQUEST_NAME_REPLY_IN_QUEUE)
	(L"REQUEST_NAME_REPLY_EXISTS"sv, DBUS_REQUEST_NAME_REPLY_EXISTS)
	(L"REQUEST_NAME_REPLY_ALREADY_OWNER"sv, DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER)
	(L"RELEASE_NAME_REPLY_RELEASED"sv, DBUS_RELEASE_NAME_REPLY_RELEASED)
	(L"RELEASE_NAME_REPLY_NON_EXISTENT"sv, DBUS_RELEASE_NAME_REPLY_NON_EXISTENT)
	(L"RELEASE_NAME_REPLY_NOT_OWNER"sv, DBUS_RELEASE_NAME_REPLY_NOT_OWNER)
	(L"START_REPLY_SUCCESS"sv, DBUS_START_REPLY_SUCCESS)
	(L"START_REPLY_ALREADY_RUNNING"sv, DBUS_START_REPLY_ALREADY_RUNNING)
	;
}

}

namespace xemmai
{

t_object* t_type_of<DBusBusType>::f_define(t_library* a_library)
{
	return t_base::f_define(a_library, [](auto a_fields)
	{
		a_fields
		(L"SESSION"sv, DBUS_BUS_SESSION)
		(L"SYSTEM"sv, DBUS_BUS_SYSTEM)
		(L"STARTER"sv, DBUS_BUS_STARTER)
		;
	});
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<xemmaix::dbus::t_library>(a_handle);
}
