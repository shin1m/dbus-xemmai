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

	t_proxy(t_type* a_class) : v_session(t_session::f_instance()), v_object(t_object::f_allocate(a_class))
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

	t_proxy_of(t_type* a_class, T_value* a_value) : t_proxy(a_class), v_value(a_value)
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
	t_slot_of<t_type> v_type_message;
	t_slot_of<t_type> v_type_reply;
	t_slot_of<t_type> v_type_bus_type;
	t_slot_of<t_type> v_type_connection;

public:
	t_extension(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
	{
		return f_global();
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return f_global()->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_extension*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_scoped f_as(T&& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_extension* t_extension::f_extension<t_extension>() const
{
	return this;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_message>()
{
	return v_type_message;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_reply>()
{
	return v_type_reply;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<DBusBusType>()
{
	return v_type_bus_type;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_connection>()
{
	return v_type_connection;
}

template<typename T>
struct t_holds : t_underivable<t_bears<T>>
{
	template<typename T0>
	struct t_cast
	{
		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			auto p = static_cast<T0*>(t_base::f_object(std::forward<T1>(a_object))->f_pointer());
			if (!p->f_valid()) t_throwable::f_throw(L"accessing from other thread.");
			if (!*p) t_throwable::f_throw(L"already destroyed.");
			return p;
		}
	};
	template<typename T0>
	struct t_as
	{
		template<typename T1>
		static T0 f_call(T1&& a_object)
		{
			return *t_cast<typename t_fundamental<T0>::t_type>::f_call(std::forward<T1>(a_object));
		}
	};
	template<typename T0>
	struct t_as<T0*>
	{
		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			return reinterpret_cast<size_t>(t_base::f_object(std::forward<T1>(a_object))) == t_value::e_tag__NULL ? nullptr : t_cast<T0>::f_call(std::forward<T1>(a_object));
		}
	};
	template<typename T0>
	struct t_is
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			auto p = t_base::f_object(std::forward<T1>(a_object));
			return reinterpret_cast<size_t>(p) >= t_value::e_tag__OBJECT && p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
		}
	};
	template<typename T0>
	struct t_is<T0*>
	{
		template<typename T1>
		static bool f_call(T1 a_object)
		{
			auto p = t_base::f_object(std::forward<T1>(a_object));
			switch (reinterpret_cast<size_t>(p)) {
			case t_value::e_tag__NULL:
				return true;
			case t_value::e_tag__BOOLEAN:
			case t_value::e_tag__INTEGER:
			case t_value::e_tag__FLOAT:
				return false;
			default:
				return p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
			}
		}
	};
	typedef xemmaix::dbus::t_extension t_extension;
	typedef t_holds t_base;

	template<typename T_extension, typename T_value>
	static t_scoped f_transfer(T_extension* a_extension, T_value&& a_value)
	{
		return a_value->f_object();
	}

	using t_underivable<t_bears<T>>::t_underivable;
	virtual void f_finalize(t_object* a_this)
	{
		auto p = static_cast<T*>(a_this->f_pointer());
		assert(!*p);
		delete p;
	}
	virtual void f_instantiate(t_stacked* a_stack, size_t a_n)
	{
		t_destruct_n destruct(a_stack, a_n);
		a_stack[0].f_construct(this->f_construct(a_stack, a_n));
	}
};

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
