#include "message.h"

namespace xemmaix::dbus
{

void t_message::f_get(t_list& a_list, DBusMessageIter& a_i)
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
				auto x = t_list::f_instantiate();
				auto& list = f_as<t_list&>(x);
				DBusMessageIter i;
				dbus_message_iter_recurse(&a_i, &i);
				f_get(list, i);
				a_list.f_push(x);
			}
			continue;
		case DBUS_TYPE_VARIANT:
			{
				DBusMessageIter i;
				dbus_message_iter_recurse(&a_i, &i);
				f_get(a_list, i);
			}
			continue;
		}
		DBusBasicValue value;
		dbus_message_iter_get_basic(&a_i, &value);
		switch (type) {
		case DBUS_TYPE_BYTE:
			a_list.f_push(value.byt);
			break;
		case DBUS_TYPE_BOOLEAN:
			a_list.f_push(value.bool_val != FALSE);
			break;
		case DBUS_TYPE_INT16:
			a_list.f_push(value.i16);
			break;
		case DBUS_TYPE_UINT16:
			a_list.f_push(value.u16);
			break;
		case DBUS_TYPE_INT32:
			a_list.f_push(value.i32);
			break;
		case DBUS_TYPE_UINT32:
			a_list.f_push(value.u32);
			break;
		case DBUS_TYPE_INT64:
			a_list.f_push(value.i64);
			break;
		case DBUS_TYPE_UINT64:
			a_list.f_push(value.u64);
			break;
		case DBUS_TYPE_DOUBLE:
			a_list.f_push(value.dbl);
			break;
		case DBUS_TYPE_STRING:
		case DBUS_TYPE_OBJECT_PATH:
		case DBUS_TYPE_SIGNATURE:
			a_list.f_push(f_global()->f_as(f_convert(value.str)));
			break;
		case DBUS_TYPE_UNIX_FD:
			a_list.f_push(value.fd);
			break;
		}
	}
}

t_pvalue t_message::f_get()
{
	auto x = t_list::f_instantiate();
	auto& list = f_as<t_list&>(x);
	DBusMessageIter i;
	dbus_message_iter_init(v_value, &i);
	f_get(list, i);
	return x;
}

t_pvalue t_message::f_append(int a_type, const char* a_signature, const t_pvalue& a_callable)
{
	auto i = v_i;
	DBusMessageIter j;
	if (i) {
		if (dbus_message_iter_open_container(i, a_type, a_signature, &j) == FALSE) f_throw(L"dbus_message_iter_open_container failed."sv);
	} else {
		dbus_message_iter_init_append(v_value, &j);
	}
	v_i = &j;
	try {
		if (i) {
			a_callable(t_object::f_of(this));
			if (dbus_message_iter_close_container(i, &j) == FALSE) f_throw(L"dbus_message_iter_close_container failed."sv);
		} else {
			f_append(a_type, a_signature, a_callable);
		}
		v_i = i;
		return t_object::f_of(this);
	} catch (...) {
		if (i) dbus_message_iter_abandon_container(i, &j);
		v_i = i;
		throw;
	}
}

}

namespace xemmai
{

void t_type_of<xemmaix::dbus::t_message>::f_define(t_library* a_library)
{
	using namespace xemmaix::dbus;
	t_define{a_library}
	(L"acquire"sv, t_member<void(t_message::*)(), &t_message::f_acquire>())
	(L"release"sv, t_member<void(t_message::*)(), &t_message::f_release>())
	(L"get_type"sv, t_member<int(t_message::*)() const, &t_message::f_get_type>())
	(L"get"sv, t_member<t_pvalue(t_message::*)(), &t_message::f_get>())
	(L"boolean"sv, t_member<t_pvalue(t_message::*)(bool), &t_message::f_boolean>())
	(L"byte"sv, t_member<t_pvalue(t_message::*)(uint8_t), &t_message::f_number<DBUS_TYPE_BYTE, uint8_t>>())
	(L"int16"sv, t_member<t_pvalue(t_message::*)(int16_t), &t_message::f_number<DBUS_TYPE_INT16, int16_t>>())
	(L"uint16"sv, t_member<t_pvalue(t_message::*)(uint16_t), &t_message::f_number<DBUS_TYPE_UINT16, uint16_t>>())
	(L"int32"sv, t_member<t_pvalue(t_message::*)(int32_t), &t_message::f_number<DBUS_TYPE_INT32, int32_t>>())
	(L"uint32"sv, t_member<t_pvalue(t_message::*)(uint32_t), &t_message::f_number<DBUS_TYPE_UINT32, uint32_t>>())
	(L"int64"sv, t_member<t_pvalue(t_message::*)(int64_t), &t_message::f_number<DBUS_TYPE_INT64, int64_t>>())
	(L"uint64"sv, t_member<t_pvalue(t_message::*)(uint64_t), &t_message::f_number<DBUS_TYPE_UINT64, uint64_t>>())
	(L"double"sv, t_member<t_pvalue(t_message::*)(double), &t_message::f_number<DBUS_TYPE_DOUBLE, double>>())
	(L"string"sv, t_member<t_pvalue(t_message::*)(std::wstring_view), &t_message::f_string<DBUS_TYPE_STRING>>())
	(L"object_path"sv, t_member<t_pvalue(t_message::*)(std::wstring_view), &t_message::f_string<DBUS_TYPE_OBJECT_PATH>>())
	(L"signature"sv, t_member<t_pvalue(t_message::*)(std::wstring_view), &t_message::f_string<DBUS_TYPE_SIGNATURE>>())
	(L"array"sv, t_member<t_pvalue(t_message::*)(std::wstring_view, const t_pvalue&), &t_message::f_container<DBUS_TYPE_ARRAY>>())
	(L"variant"sv, t_member<t_pvalue(t_message::*)(std::wstring_view, const t_pvalue&), &t_message::f_container<DBUS_TYPE_VARIANT>>())
	(L"struct"sv, t_member<t_pvalue(t_message::*)(const t_pvalue&), &t_message::f_container<DBUS_TYPE_STRUCT>>())
	(L"dict_entry"sv, t_member<t_pvalue(t_message::*)(const t_pvalue&), &t_message::f_container<DBUS_TYPE_DICT_ENTRY>>())
	.f_derive<t_message, t_object>();
}

t_pvalue t_type_of<xemmaix::dbus::t_message>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_pvalue(*)(t_type*, const t_string*, std::wstring_view, const t_string*, std::wstring_view), xemmaix::dbus::t_message::f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, xemmaix::dbus::t_message&), xemmaix::dbus::t_message::f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, std::wstring_view, std::wstring_view, std::wstring_view), xemmaix::dbus::t_message::f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, xemmaix::dbus::t_message&, std::wstring_view, const t_string*), xemmaix::dbus::t_message::f_construct>
	>::t_bind<xemmaix::dbus::t_message>::f_do(this, a_stack, a_n);
}

}
