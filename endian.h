#pragma once

#include "compiler.h"

#ifdef __APPLE__
#include <machine/endian.h>
#else
#include <endian.h>
#endif

#ifndef BYTE_ORDER
# error "BYTE_ORDER is not defined"
#endif


template <typename T>
class le;


template <typename T>
class be {
	static_assert(has_bswap<T>::value, "type parameter must be a byte-swappable type");

private:
	T value_be;

public:
	constexpr be() = default;
#if BYTE_ORDER == BIG_ENDIAN
	constexpr be(const T &value) : value_be(value) { }
	constexpr _pure operator T () const { return value_be; }
#elif BYTE_ORDER == LITTLE_ENDIAN
	constexpr be(const T &value) : value_be(bswap(value)) { }
	constexpr _pure operator T () const { return bswap(value_be); }
#endif
	constexpr bool _pure operator == (const be<T> &rhs) const { return value_be == rhs.value_be; }
	constexpr bool _pure operator != (const be<T> &rhs) const { return value_be != rhs.value_be; }

};


template <typename T>
class le {
	static_assert(has_bswap<T>::value, "type parameter must be a byte-swappable type");

private:
	T value_le;

public:
	constexpr le() = default;
#if BYTE_ORDER == BIG_ENDIAN
	constexpr le(const T &value) : value_le(bswap(value)) { }
	constexpr _pure operator T () const { return bswap(value_le); }
#elif BYTE_ORDER == LITTLE_ENDIAN
	constexpr le(const T &value) : value_le(value) { }
	constexpr _pure operator T () const { return value_le; }
#endif
	constexpr bool _pure operator == (const le<T> &rhs) const { return value_le == rhs.value_le; }
	constexpr bool _pure operator != (const le<T> &rhs) const { return value_le != rhs.value_le; }

};


template <typename T>
struct be_explicit : public be<T> {
	using be<T>::be;
	operator T () const __attribute__ ((__error__ ("byte-order conversion has no effect")));
};


template <typename T>
struct le_explicit : public le<T> {
	using le<T>::le;
	operator T () const  __attribute__ ((__error__ ("byte-order conversion has no effect")));
};


template <typename T> static constexpr be<T> & _const as_be(T &v) { return reinterpret_cast<be<T> &>(v); }
template <typename T> static constexpr const be<T> & _const as_be(const T &v) { return reinterpret_cast<const be<T> &>(v); }
template <typename T> static constexpr be_explicit<T> _pure htobe(const T &v) { return v; }
template <typename T> static constexpr T _pure betoh(const be<T> &v) { return v; }

template <typename T> static constexpr le<T> & _const as_le(T &v) { return reinterpret_cast<le<T> &>(v); }
template <typename T> static constexpr const le<T> & _const as_le(const T &v) { return reinterpret_cast<const le<T> &>(v); }
template <typename T> static constexpr le_explicit<T> _pure htole(const T &v) { return v; }
template <typename T> static constexpr T _pure letoh(const le<T> &v) { return v; }
