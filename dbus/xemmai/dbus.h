#ifndef DBUS__XEMMAI__DBUS_H
#define DBUS__XEMMAI__DBUS_H

#ifdef _WIN32
#ifndef DBUS__XEMMAI__EXPORT
#define DBUS__XEMMAI__EXPORT __declspec(dllimport)
#endif
#else
#define DBUS__XEMMAI__EXPORT
#endif

#include <cerrno>
#include <iterator>
#include <iconv.h>
#include <xemmai/convert.h>
#include <xemmai/array.h>
#include <xemmai/bytes.h>
#include <dbus/dbus.h>

namespace dbus
{

namespace xemmai
{

using ::xemmai::t_object;
using ::xemmai::t_scan;
using ::xemmai::t_value;
using ::xemmai::t_slot;
using ::xemmai::t_scoped;
using ::xemmai::t_fundamental;
using ::xemmai::t_type_of;
using ::xemmai::f_check;
using ::xemmai::f_as;
using ::xemmai::t_define;
using ::xemmai::f_global;
using ::xemmai::t_tuple;
using ::xemmai::t_throwable;
using ::xemmai::t_array;
using ::xemmai::t_bytes;
using ::xemmai::t_safe_region;

class t_proxy;
class t_extension;
class t_message;
class t_reply;
class t_connection;

template<typename C0, typename C1, size_t N = 256>
class t_converter
{
	iconv_t v_cd;

public:
	t_converter(const char* a_from, const char* a_to) : v_cd(iconv_open(a_to, a_from))
	{
	}
	~t_converter()
	{
		iconv_close(v_cd);
	}
	template<typename I, typename O>
	O operator()(I f, I l, O d) const;
};

template<typename C0, typename C1, size_t N>
template<typename I, typename O>
O t_converter<C0, C1, N>::operator()(I f, I l, O d) const
{
	char cs0[N];
	char cs1[N];
	char* p0 = cs0;
	while (f != l || p0 > cs0) {
		while (f != l && p0 + sizeof(C0) <= cs0 + sizeof(cs0)) {
			*reinterpret_cast<C0*>(p0) = *f;
			p0 += sizeof(C0);
			++f;
		}
		size_t n0 = p0 - cs0;
		p0 = cs0;
		char* p1 = cs1;
		size_t n1 = sizeof(cs1);
		do {
			size_t n = iconv(v_cd, &p0, &n0, &p1, &n1);
			if (n == static_cast<size_t>(-1)) {
				if (errno == EILSEQ) {
					if (n1 < sizeof(C1)) break;
					*reinterpret_cast<C1*>(p1) = '?';
					p1 += sizeof(C1);
					n1 -= sizeof(C1);
				} else if (errno == EINVAL) {
					if (p0 > cs0) break;
				} else {
					break;
				}
				p0 += sizeof(C0);
				n0 -= sizeof(C0);
			}
		} while (n0 > 0);
		d = std::copy(reinterpret_cast<const C1*>(cs1), reinterpret_cast<const C1*>(p1), d);
		p0 = std::copy(p0, p0 + n0, static_cast<char*>(cs0));
	}
	char* p1 = cs1;
	size_t n1 = sizeof(cs1);
	if (iconv(v_cd, NULL, NULL, &p1, &n1) != static_cast<size_t>(-1)) d = std::copy(reinterpret_cast<const C1*>(cs1), reinterpret_cast<const C1*>(p1), d);
	return d;
}

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
	DBUS__XEMMAI__EXPORT t_entry();
	void f_unlink()
	{
		v_previous->v_next = v_next;
		v_next->v_previous = v_previous;
		v_previous = v_next = nullptr;
	}

public:
	DBUS__XEMMAI__EXPORT virtual void f_dispose();
};

class t_session : public t_entry
{
	friend class t_proxy;
	friend std::string f_convert(const std::wstring& a_string);
	friend std::wstring f_convert(const std::string& a_string);

	static XEMMAI__PORTABLE__THREAD t_session* v_instance;

	t_extension* v_extension;
	t_converter<wchar_t, char> v_encoder;
	t_converter<char, wchar_t> v_decoder;

public:
#ifdef _WIN32
	static DBUS__XEMMAI__EXPORT t_session* f_instance();
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
	DBUS__XEMMAI__EXPORT virtual void f_destroy();

public:
	DBUS__XEMMAI__EXPORT virtual ~t_proxy();
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
	static DBUS__XEMMAI__EXPORT dbus_int32_t v_slot;

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
//	std::fprintf(stderr, "dispose %s(%p)\n", typeid(v_value).name(), v_value);
	t_proxy::f_dispose();
	if (v_n > 0) {
		v_n = 0;
		T::f_unreference(v_value);
	}
}

class t_extension : public ::xemmai::t_extension
{
	template<typename T, typename T_super> friend class t_define;

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

}

namespace xemmai
{

template<>
struct t_type_of<DBusBusType> : t_enum_of<DBusBusType, dbus::xemmai::t_extension>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

}

#endif
