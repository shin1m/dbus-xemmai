#ifndef XEMMAIX__DBUS__DBUS_H
#define XEMMAIX__DBUS__DBUS_H

#ifdef _WIN32
#ifndef XEMMAIX__DBUS__EXPORT
#define XEMMAIX__DBUS__EXPORT __declspec(dllimport)
#endif
#else
#define XEMMAIX__DBUS__EXPORT
#endif

#include <codecvt>
#include <iterator>
#include <locale>
#include <xemmai/convert.h>
#include <xemmai/array.h>
#include <xemmai/bytes.h>
#include <dbus/dbus.h>

namespace xemmaix::dbus
{

using namespace xemmai;

class t_proxy;
class t_extension;
class t_message;
class t_reply;
class t_connection;

std::string f_convert(const std::wstring& a_string);
std::wstring f_convert(const std::string& a_string);

t_scoped f_tuple(t_scoped&& a_0, t_scoped&& a_1);
t_scoped f_tuple(t_scoped&& a_0, t_scoped&& a_1, t_scoped&& a_2, t_scoped&& a_3);
t_scoped f_tuple(t_scoped&& a_0, t_scoped&& a_1, t_scoped&& a_2, t_scoped&& a_3, t_scoped&& a_4);
t_scoped f_tuple(t_scoped&& a_0, t_scoped&& a_1, t_scoped&& a_2, t_scoped&& a_3, t_scoped&& a_4, t_scoped&& a_5);

class t_entry
{
protected:
	t_entry* v_previous;
	t_entry* v_next;

	t_entry(bool) : v_previous(this), v_next(this)
	{
	}
	XEMMAIX__DBUS__EXPORT t_entry();
	void f_unlink()
	{
		v_previous->v_next = v_next;
		v_next->v_previous = v_previous;
		v_previous = v_next = nullptr;
	}

public:
	XEMMAIX__DBUS__EXPORT virtual void f_dispose();
};

class t_session : public t_entry
{
	friend class t_proxy;
	friend std::string f_convert(const std::wstring& a_string);
	friend std::wstring f_convert(const std::string& a_string);

	static XEMMAI__PORTABLE__THREAD t_session* v_instance;

	t_extension* v_extension;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> v_convert;

public:
#ifdef _WIN32
	static XEMMAIX__DBUS__EXPORT t_session* f_instance();
#else
	static t_session* f_instance()
	{
		if (!v_instance) t_throwable::f_throw(L"must be inside main.");
		return v_instance;
	}
#endif

	t_session(t_extension* a_extension);
	~t_session();
	t_extension* f_extension() const
	{
		return v_extension;
	}
};

class t_proxy : public t_entry
{
	t_session* v_session;
	t_scoped v_object;

protected:
	static void f_destroy(void* a_p)
	{
		static_cast<t_proxy*>(a_p)->f_destroy();
	}
	static t_scoped f_transfer(t_proxy* a_proxy)
	{
		++a_proxy->v_n;
		return a_proxy->v_object;
	}

	size_t v_n = 0;

	t_proxy(t_object* a_class) : v_session(t_session::f_instance()), v_object(t_object::f_allocate(a_class))
	{
		v_object.f_pointer__(this);
	}
	XEMMAIX__DBUS__EXPORT virtual void f_destroy();

public:
	XEMMAIX__DBUS__EXPORT virtual ~t_proxy() = default;
	bool f_valid() const
	{
		return v_session == t_session::f_instance();
	}
	t_object* f_object() const
	{
		return v_object;
	}
};

template<typename T, typename T_value>
class t_proxy_of : public t_proxy
{
protected:
	typedef t_proxy_of t_base;

	template<typename T_type>
	static t_scoped f_construct_shared(T_value* a_value)
	{
		T* p = f_from(a_value);
		if (p) {
			if (p->v_n > 0) T::f_unreference(a_value);
			return f_transfer(p);
		} else {
			return f_transfer(new T_type(a_value));
		}
	}

	T_value* v_value;

	t_proxy_of(t_object* a_class, T_value* a_value) : t_proxy(a_class), v_value(a_value)
	{
		T::f_set_data(v_value, v_slot, this, t_proxy::f_destroy);
	}
	virtual void f_destroy();

public:
	static XEMMAIX__DBUS__EXPORT dbus_int32_t v_slot;

	static T* f_from(T_value* a_value)
	{
		return static_cast<T*>(T::f_get_data(a_value, v_slot));
	}
	static t_scoped f_construct(T_value* a_value)
	{
		return f_transfer(new T(a_value));
	}

	virtual void f_dispose();
	operator T_value*() const
	{
		return v_value;
	}
	void f_acquire()
	{
		if (v_n <= 0) T::f_reference(v_value);
		++v_n;
	}
	void f_release()
	{
		if (v_n > 0 && --v_n <= 0) T::f_unreference(v_value);
	}
};

template<typename T, typename T_value>
void t_proxy_of<T, T_value>::f_destroy()
{
//	std::fprintf(stderr, "destroy %s(%p)\n", typeid(v_value).name(), v_value);
	v_value = nullptr;
	t_proxy::f_destroy();
}

template<typename T, typename T_value>
dbus_int32_t t_proxy_of<T, T_value>::v_slot = -1;

template<typename T, typename T_value>
void t_proxy_of<T, T_value>::f_dispose()
{
//	std::fprintf(stderr, "dispose %s(%p) %d\n", typeid(v_value).name(), v_value, v_n);
	t_proxy::f_dispose();
	if (v_n > 0) {
		v_n = 0;
		T::f_unreference(v_value);
	}
}

class t_extension : public xemmai::t_extension
{
	template<typename T, typename T_super> friend class xemmai::t_define;

	t_slot v_type_message;
	t_slot v_type_reply;
	t_slot v_type_bus_type;
	t_slot v_type_connection;

	template<typename T>
	void f_type__(t_scoped&& a_type);

public:
	t_extension(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
	{
		return f_global();
	}
	template<typename T>
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_scoped f_as(const T& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), a_value);
	}
};

template<>
inline void t_extension::f_type__<t_message>(t_scoped&& a_type)
{
	v_type_message = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_reply>(t_scoped&& a_type)
{
	v_type_reply = std::move(a_type);
}

template<>
inline void t_extension::f_type__<DBusBusType>(t_scoped&& a_type)
{
	v_type_bus_type = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_connection>(t_scoped&& a_type)
{
	v_type_connection = std::move(a_type);
}

template<>
inline const t_extension* t_extension::f_extension<t_extension>() const
{
	return this;
}

template<>
inline t_object* t_extension::f_type<t_message>() const
{
	return v_type_message;
}

template<>
inline t_object* t_extension::f_type<t_reply>() const
{
	return v_type_reply;
}

template<>
inline t_object* t_extension::f_type<DBusBusType>() const
{
	return v_type_bus_type;
}

template<>
inline t_object* t_extension::f_type<t_connection>() const
{
	return v_type_connection;
}

}

namespace xemmai
{

template<>
struct t_type_of<DBusBusType> : t_enum_of<DBusBusType, xemmaix::dbus::t_extension>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

}

#endif
