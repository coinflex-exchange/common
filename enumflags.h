#pragma once

#include <type_traits>

#define DEFINE_ENUM_FLAG_OPS(E) \
	static_assert(std::is_enum<E>::value, "must be an enum type"); \
	static inline constexpr E operator ~ (E arg) { return static_cast<E>(~static_cast<typename std::underlying_type<E>::type>(arg)); } \
	static inline constexpr E operator & (E lhs, E rhs) { return static_cast<E>(static_cast<typename std::underlying_type<E>::type>(lhs) & static_cast<typename std::underlying_type<E>::type>(rhs)); } \
	static inline constexpr E operator | (E lhs, E rhs) { return static_cast<E>(static_cast<typename std::underlying_type<E>::type>(lhs) | static_cast<typename std::underlying_type<E>::type>(rhs)); } \
	static inline constexpr E operator ^ (E lhs, E rhs) { return static_cast<E>(static_cast<typename std::underlying_type<E>::type>(lhs) ^ static_cast<typename std::underlying_type<E>::type>(rhs)); } \
	static inline E & operator &= (E &lhs, E rhs) { return lhs = static_cast<E>(static_cast<typename std::underlying_type<E>::type>(lhs) & static_cast<typename std::underlying_type<E>::type>(rhs)); } \
	static inline E & operator |= (E &lhs, E rhs) { return lhs = static_cast<E>(static_cast<typename std::underlying_type<E>::type>(lhs) | static_cast<typename std::underlying_type<E>::type>(rhs)); } \
	static inline E & operator ^= (E &lhs, E rhs) { return lhs = static_cast<E>(static_cast<typename std::underlying_type<E>::type>(lhs) ^ static_cast<typename std::underlying_type<E>::type>(rhs)); }