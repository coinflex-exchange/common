#pragma once

#include <climits>
#include <cstdint>
#include <type_traits>

#ifdef __GNUC__

#define _const __attribute__ ((__const__))
#define _hidden __attribute__ ((__visibility__ ("hidden")))
#define _nonnull(...) __attribute__ ((__nonnull__ (__VA_ARGS__)))
#define _noreturn __attribute__ ((__noreturn__))
#define _pure __attribute__ ((__pure__))
#define _unused __attribute__ ((__unused__))
#define _visible __attribute__ ((__visibility__ ("default")))
#define _weak __attribute__ ((__weak__))
#define _wur __attribute__ ((__warn_usused_result__))

#define _restrict __restrict

static constexpr int _ffs(int v) { return __builtin_ffs(v); }
static constexpr int _ffs(long v) { return __builtin_ffsl(v); }
static constexpr int _ffs(long long v) { return __builtin_ffsll(v); }

static constexpr int _clz(unsigned v) { return __builtin_clz(v); }
static constexpr int _clz(unsigned long v) { return __builtin_clzl(v); }
static constexpr int _clz(unsigned long long v) { return __builtin_clzll(v); }

static constexpr int _ctz(unsigned v) { return __builtin_ctz(v); }
static constexpr int _ctz(unsigned long v) { return __builtin_ctzl(v); }
static constexpr int _ctz(unsigned long long v) { return __builtin_ctzll(v); }

static constexpr int _clrsb(int v) { return __builtin_clrsb(v); }
static constexpr int _clrsb(long v) { return __builtin_clrsbl(v); }
static constexpr int _clrsb(long long v) { return __builtin_clrsbll(v); }

static constexpr int _popcount(unsigned v) { return __builtin_popcount(v); }
static constexpr int _popcount(unsigned long v) { return __builtin_popcountl(v); }
static constexpr int _popcount(unsigned long long v) { return __builtin_popcountll(v); }

static constexpr int _parity(unsigned v) { return __builtin_parity(v); }
static constexpr int _parity(unsigned long v) { return __builtin_parityl(v); }
static constexpr int _parity(unsigned long long v) { return __builtin_parityll(v); }

static constexpr unsigned rotl(unsigned v, unsigned s) { return v << s | v >> sizeof v * 8 - s; }
static constexpr unsigned long rotl(unsigned long v, unsigned s) { return v << s | v >> sizeof v * 8 - s; }
static constexpr unsigned long long rotl(unsigned long long v, unsigned s) { return v << s | v >> sizeof v * 8 - s; }

static constexpr unsigned rotr(unsigned v, unsigned s) { return v >> s | v << sizeof v * 8 - s; }
static constexpr unsigned long rotr(unsigned long v, unsigned s) { return v >> s | v << sizeof v * 8 - s; }
static constexpr unsigned long long rotr(unsigned long long v, unsigned s) { return v >> s | v << sizeof v * 8 - s; }

static constexpr int16_t bswap(int16_t v) { return __builtin_bswap16(v); }
static constexpr uint16_t bswap(uint16_t v) { return __builtin_bswap16(v); }
static constexpr int32_t bswap(int32_t v) { return __builtin_bswap32(v); }
static constexpr uint32_t bswap(uint32_t v) { return __builtin_bswap32(v); }
static constexpr int64_t bswap(int64_t v) { return __builtin_bswap64(v); }
static constexpr uint64_t bswap(uint64_t v) { return __builtin_bswap64(v); }

#if ULONG_MAX == UINT32_MAX && UINT32_MAX == UINT_MAX
static constexpr long bswap(long v) { return __builtin_bswap32(v); }
static constexpr unsigned long bswap(unsigned long v) { return __builtin_bswap32(v); }
#elif ULONG_LONG_MAX == UINT64_MAX && UINT64_MAX == ULONG_MAX
static constexpr long long bswap(long long v) { return __builtin_bswap64(v); }
static constexpr unsigned long long bswap(unsigned long long v) { return __builtin_bswap64(v); }
#endif

#ifdef __SIZEOF_INT128__
static inline unsigned __int128 _const __bswap128(unsigned __int128 v) {
	union {
		uint64_t q[2];
		unsigned __int128 o;
	} u;
	u.o = v;
	uint64_t t = __builtin_bswap64(u.q[0]);
	u.q[0] = __builtin_bswap64(u.q[1]);
	u.q[1] = t;
	return u.o;
}
static inline signed __int128 _const bswap(signed __int128 v) { return __bswap128(v); }
static inline unsigned __int128 _const bswap(unsigned __int128 v) { return __bswap128(v); }
#endif

#else

#define _const
#define _hidden
#define _nonnull(...)
#define _noreturn
#define _pure
#define _unused
#define _visible
#define _wur

#define _restrict

#endif

template <typename T> static constexpr typename std::enable_if<std::is_enum<T>::value, T>::type bswap(T v) { return static_cast<T>(bswap(static_cast<typename std::underlying_type<T>::type>(v))); }

template <typename T, typename Enable = void>
struct has_bswap : std::false_type { };

template <typename T>
struct has_bswap<T, typename std::enable_if<std::is_same<decltype(bswap(std::declval<T>())), typename std::remove_reference<T>::type>::value>::type> : std::true_type { };
