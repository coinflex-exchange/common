#pragma once

#include <ostream>
#include <type_traits>

#define DEFINE_ENUM_FLAG_OPS(E) \
	static_assert(std::is_enum_v<E>, "must be an enum type"); \
	static constexpr E operator ~ (E arg) noexcept { return static_cast<E>(~static_cast<std::underlying_type_t<E>>(arg)); } \
	static constexpr E operator & (E lhs, E rhs) noexcept { return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) & static_cast<std::underlying_type_t<E>>(rhs)); } \
	static constexpr E operator | (E lhs, E rhs) noexcept { return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) | static_cast<std::underlying_type_t<E>>(rhs)); } \
	static constexpr E operator ^ (E lhs, E rhs) noexcept { return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) ^ static_cast<std::underlying_type_t<E>>(rhs)); } \
	static inline E & operator &= (E &lhs, E rhs) noexcept { return lhs = static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) & static_cast<std::underlying_type_t<E>>(rhs)); } \
	static inline E & operator |= (E &lhs, E rhs) noexcept { return lhs = static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) | static_cast<std::underlying_type_t<E>>(rhs)); } \
	static inline E & operator ^= (E &lhs, E rhs) noexcept { return lhs = static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) ^ static_cast<std::underlying_type_t<E>>(rhs)); } \
	static inline std::ostream & operator << (std::ostream &os, E e) { \
		auto orig_flags = os.flags(std::ios_base::hex | std::ios_base::showbase); \
		os << +static_cast<std::underlying_type_t<E>>(e); \
		os.flags(orig_flags); \
		return os; \
	}
