#ifndef XEMMAIX__DBUS__DBUS_H
#define XEMMAIX__DBUS__DBUS_H

#ifdef _WIN32
#ifndef XEMMAIX__DBUS__EXPORT
#define XEMMAIX__DBUS__EXPORT __declspec(dllimport)
#endif
#else
#define XEMMAIX__DBUS__EXPORT
#endif

#include <xemmai/convert.h>
#include <dbus/dbus.h>

namespace xemmaix::dbus
{

using namespace xemmai;

class t_proxy;
class t_library;
class t_message;
class t_reply;
class t_connection;

template<typename C0, typename C1>
struct t_converter : private portable::t_iconv
{
	using portable::t_iconv::t_iconv;
	int f_to(char** a_p, size_t* a_n, auto a_out) const
	{
		char cs[16];
		char* p = cs;
		size_t n = sizeof(cs);
		while (iconv(v_cd, a_p, a_n, &p, &n) == size_t(-1)) {
			auto e = errno;
			if (e == EINTR) continue;
			a_out(reinterpret_cast<const C1*>(cs), (p - cs) / sizeof(C1));
			if (e != E2BIG) return e;
			p = cs;
			n = sizeof(cs);
		}
		a_out(reinterpret_cast<const C1*>(cs), (p - cs) / sizeof(C1));
		return 0;
	}
	std::basic_string<C1> operator()(std::basic_string_view<C0> a_x) const
	{
		std::basic_string<C1> s;
		auto p = reinterpret_cast<char*>(const_cast<C0*>(a_x.data()));
		size_t n = a_x.size() * sizeof(C0);
		auto append = [&](auto a_p, auto a_n)
		{
			s.append(a_p, a_p + a_n);
		};
		auto e = f_to(&p, &n, append);
		if (e == 0) e = f_to(nullptr, nullptr, append);
		if (e == 0) return s;
		throw std::system_error(e, std::generic_category());
	}
};

std::string f_convert(std::wstring_view a_x);
std::wstring f_convert(std::string_view a_x);

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
	friend std::string f_convert(std::wstring_view a_x);
	friend std::wstring f_convert(std::string_view a_x);

	static XEMMAI__PORTABLE__THREAD t_session* v_instance;

	t_library* v_library;
	t_converter<wchar_t, char> v_to_utf8;
	t_converter<char, wchar_t> v_from_utf8;

public:
#ifdef _WIN32
	static XEMMAIX__DBUS__EXPORT t_session* f_instance();
#else
	static t_session* f_instance()
	{
		if (!v_instance) f_throw(L"must be inside main."sv);
		return v_instance;
	}
#endif

	t_session(t_library* a_library);
	~t_session();
	t_library* f_library() const
	{
		return v_library;
	}
};

class t_proxy : public t_entry
{
	t_session* v_session = t_session::f_instance();
	t_root v_object = t_object::f_of(this);

protected:
	static void f_destroy(void* a_p)
	{
		static_cast<t_proxy*>(a_p)->f_destroy();
	}
	static t_pvalue f_transfer(t_object* a_value)
	{
		++a_value->f_as<t_proxy>().v_n;
		return a_value;
	}

	size_t v_n = 0;

	XEMMAIX__DBUS__EXPORT virtual void f_destroy();

public:
	XEMMAIX__DBUS__EXPORT virtual ~t_proxy() = default;
	bool f_valid() const
	{
		return v_session == t_session::f_instance();
	}
};

template<typename T, typename T_value>
class t_proxy_of : public t_proxy
{
protected:
	using t_base = t_proxy_of;

	template<typename T_type>
	static t_pvalue f_construct_shared(t_type* a_class, T_value* a_value)
	{
		T* p = f_from(a_value);
		if (p) {
			if (p->v_n > 0) T::f_unreference(a_value);
			return f_transfer(t_object::f_of(p));
		} else {
			return f_transfer(a_class->f_new<T_type>(a_value));
		}
	}

	T_value* v_value;

	t_proxy_of(T_value* a_value) : v_value(a_value)
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
	static t_pvalue f_construct(t_type* a_class, T_value* a_value)
	{
		return f_transfer(a_class->f_new<T>(a_value));
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

class t_library : public xemmai::t_library
{
	t_slot_of<t_type> v_type_message;
	t_slot_of<t_type> v_type_reply;
	t_slot_of<t_type> v_type_bus_type;
	t_slot_of<t_type> v_type_connection;

public:
	using xemmai::t_library::t_library;
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_library, t_global, f_global())
XEMMAI__LIBRARY__TYPE(t_library, message)
XEMMAI__LIBRARY__TYPE(t_library, reply)
XEMMAI__LIBRARY__TYPE_AS(t_library, DBusBusType, bus_type)
XEMMAI__LIBRARY__TYPE(t_library, connection)

template<typename T>
struct t_holds : t_bears<T>
{
	template<typename U>
	static U& f_cast(auto&& a_object)
	{
		auto& p = static_cast<t_object*>(a_object)->f_as<U>();
		if (!p.f_valid()) f_throw(L"accessing from other thread."sv);
		if (!p) f_throw(L"already destroyed."sv);
		return p;
	}
	template<typename U>
	struct t_cast
	{
		static U f_as(auto&& a_object)
		{
			return f_cast<typename t_fundamental<U>::t_type>(std::forward<decltype(a_object)>(a_object));
		}
		static bool f_is(t_object* a_object)
		{
			return reinterpret_cast<uintptr_t>(a_object) >= c_tag__OBJECT && a_object->f_type()->f_derives<typename t_fundamental<U>::t_type>();
		}
	};
	template<typename U>
	struct t_cast<U*>
	{
		static U* f_as(auto&& a_object)
		{
			return static_cast<t_object*>(a_object) ? &f_cast<U>(std::forward<decltype(a_object)>(a_object)) : nullptr;
		}
		static bool f_is(t_object* a_object)
		{
			return reinterpret_cast<uintptr_t>(a_object) == c_tag__NULL || reinterpret_cast<uintptr_t>(a_object) >= c_tag__OBJECT && a_object->f_type()->f_derives<typename t_fundamental<U>::t_type>();
		}
	};
	using t_library = xemmaix::dbus::t_library;
	using t_base = t_holds;

	static t_pvalue f_transfer(auto* a_library, auto&& a_value)
	{
		return t_object::f_of(a_value);
	}

	using t_bears<T>::t_bears;
	static void f_do_finalize(t_object* a_this)
	{
		auto& p = a_this->f_as<T>();
		assert(!p);
		p.~T();
	}
	void f_do_instantiate(t_pvalue* a_stack, size_t a_n)
	{
		a_stack[0] = this->f_construct(a_stack, a_n);
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<DBusBusType> : t_enum_of<DBusBusType, xemmaix::dbus::t_library>
{
	static t_object* f_define(t_library* a_library);

	using t_base::t_base;
};

}

#endif
