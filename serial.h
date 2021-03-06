#include <map>
#include <tuple>

#include "io.h"


template <size_t S> struct _varint_ops { };
template <> struct _varint_ops<4> {
	typedef int32_t signed_type;
	typedef uint32_t unsigned_type;
	static Source & read_signed(Source &source, signed_type &value);
	static Sink & write_signed(Sink &sink, signed_type value);
	static Source & read_unsigned(Source &source, unsigned_type &value);
	static Sink & write_unsigned(Sink &sink, unsigned_type value);
};
template <> struct _varint_ops<8> {
	typedef int64_t signed_type;
	typedef uint64_t unsigned_type;
	static Source & read_signed(Source &source, signed_type &value);
	static Sink & write_signed(Sink &sink, signed_type value);
	static Source & read_unsigned(Source &source, unsigned_type &value);
	static Sink & write_unsigned(Sink &sink, unsigned_type value);
};

template <typename T>
static inline std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>, Source &> read_varint(Source &source, T &value) {
	return _varint_ops<sizeof(T)>::read_signed(source, reinterpret_cast<typename _varint_ops<sizeof(T)>::signed_type &>(value));
}

template <typename T>
static inline std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>, Sink &> write_varint(Sink &sink, T value) {
	return _varint_ops<sizeof(T)>::write_signed(sink, value);
}

template <typename T>
static inline std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>, Source &> read_varint(Source &source, T &value) {
	return _varint_ops<sizeof(T)>::read_unsigned(source, reinterpret_cast<typename _varint_ops<sizeof(T)>::unsigned_type &>(value));
}

template <typename T>
static inline std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>, Sink &> write_varint(Sink &sink, T value) {
	return _varint_ops<sizeof(T)>::write_unsigned(sink, value);
}

template <typename T>
struct _varint {
	T x;
};

template <typename T>
static inline std::enable_if_t<std::is_integral_v<T>, _varint<T &>> varint(T &x) {
	return { x };
}

template <typename T>
static inline std::enable_if_t<std::is_integral_v<T>, _varint<const T>> varint(const T &x) {
	return { x };
}

template <typename T>
static inline _varint<std::underlying_type_t<T> &> varenum(T &ref) {
	return varint(reinterpret_cast<std::underlying_type_t<T> &>(ref));
}

template <typename T>
static inline _varint<const std::underlying_type_t<T>> varenum(const T &ref) {
	return varint(reinterpret_cast<const std::underlying_type_t<T> &>(ref));
}

template <typename T>
static inline Source & operator >> (Source &source, _varint<T> varint) {
	return read_varint(source, varint.x);
}

template <typename T>
static inline Sink & operator << (Sink &sink, _varint<T> varint) {
	return write_varint(sink, varint.x);
}

template <typename T>
static inline T read_varint(Source &source) {
	T value;
	read_varint(source, value);
	return value;
}

template <typename T>
static inline std::enable_if_t<std::is_trivially_copyable_v<T>, Source> & operator >> (Source &source, T &value) {
	source.read_fully(&value, sizeof value);
	return source;
}

template <typename T>
static inline std::enable_if_t<std::is_trivially_copyable_v<T>, Sink> & operator << (Sink &sink, const T &value) {
	sink.write_fully(&value, sizeof value);
	return sink;
}

template <typename T>
static inline std::enable_if_t<std::is_trivially_copyable_v<T>, Source> & operator >> (Source &source, std::basic_string<T> &string) {
	string.resize(read_varint<size_t>(source));
	source.read_fully(&string.front(), string.size() * sizeof(T));
	return source;
}

template <typename T>
static inline std::enable_if_t<std::is_trivially_copyable_v<T>, Sink> & operator << (Sink &sink, const std::basic_string<T> &string) {
	write_varint(sink, string.size());
	sink.write_fully(string.data(), string.size() * sizeof(T));
	return sink;
}

template <typename T>
static inline std::enable_if_t<std::is_trivially_copyable_v<T>, Source> & operator >> (Source &source, std::vector<T> &vector) {
	vector.resize(read_varint<size_t>(source));
	source.read_fully(vector.data(), vector.size() * sizeof(T));
	return source;
}

template <typename T>
static inline std::enable_if_t<std::is_trivially_copyable_v<T>, Sink> & operator << (Sink &sink, const std::vector<T> &vector) {
	write_varint(sink, vector.size());
	sink.write_fully(vector.data(), vector.size() * sizeof(T));
	return sink;
}

template <typename T, size_t N>
static inline std::enable_if_t<!std::is_trivially_copyable_v<T>, Source> & operator >> (Source &source, std::array<T, N> &array) {
	for (auto &element : array) {
		source >> element;
	}
	return source;
}

template <typename T, size_t N>
static inline std::enable_if_t<!std::is_trivially_copyable_v<T>, Sink> & operator << (Sink &sink, const std::array<T, N> &array) {
	for (auto &element : array) {
		sink << element;
	}
	return sink;
}

template <typename T>
static inline std::enable_if_t<!std::is_trivially_copyable_v<T>, Source> & operator >> (Source &source, std::vector<T> &vector) {
	vector.resize(read_varint<size_t>(source));
	for (auto &element : vector) {
		source >> element;
	}
	return source;
}

template <typename T>
static inline std::enable_if_t<!std::is_trivially_copyable_v<T>, Sink> & operator << (Sink &sink, const std::vector<T> &vector) {
	write_varint(sink, vector.size());
	for (auto &element : vector) {
		sink << element;
	}
	return sink;
}

template <typename T1, typename T2>
static inline Source & operator >> (Source &source, std::pair<T1, T2> &pair) {
	return source >> pair.first >> pair.second;
}

template <typename T1, typename T2>
static inline Sink & operator << (Sink &sink, const std::pair<T1, T2> &pair) {
	return sink << pair.first << pair.second;
}

template <typename K, typename T, typename... _>
static inline Source & operator >> (Source &source, std::map<K, T, _...> &map) {
	map.clear();
	for (auto count = read_varint<size_t>(source); count > 0; --count) {
		std::pair<K, T> pair;
		source >> pair;
		map.emplace_hint(map.end(), std::move(pair));
	}
	return source;
}

template <typename K, typename T, typename... _>
static inline Sink & operator << (Sink &sink, const std::map<K, T, _...> &map) {
	write_varint(sink, map.size());
	for (auto &pair : map) {
		sink << pair;
	}
	return sink;
}

template <size_t I, typename... Types>
static inline std::enable_if_t<(I < sizeof...(Types)), Source> & read_tuple(Source &source, std::tuple<Types...> &tuple) {
	return read_tuple<I + 1>(source >> std::get<I>(tuple), tuple);
}

template <size_t I, typename... Types>
static inline std::enable_if_t<I == sizeof...(Types), Source> & read_tuple(Source &source, std::tuple<Types...> &tuple) {
	return source;
}

template <typename... Types>
static inline Source & operator >> (Source &source, std::tuple<Types...> &tuple) {
	return read_tuple<0>(source, tuple);
}

template <size_t I, typename... Types>
static inline std::enable_if_t<(I < sizeof...(Types)), Sink> & write_tuple(Sink &sink, const std::tuple<Types...> &tuple) {
	return write_tuple<I + 1>(sink << std::get<I>(tuple), tuple);
}

template <size_t I, typename... Types>
static inline std::enable_if_t<I == sizeof...(Types), Sink> & write_tuple(Sink &sink, const std::tuple<Types...> &tuple) {
	return sink;
}

template <typename... Types>
static inline Sink & operator << (Sink &sink, const std::tuple<Types...> &tuple) {
	return write_tuple<0>(sink, tuple);
}

template <typename T>
static inline Buffer serialize(const T &value) {
	BufferSink sink;
	sink << value;
	return sink;
}

template <typename T>
static inline T deserialize(Source &source) {
	T value;
	source >> value;
	return value;
}

template <typename T>
static inline T deserialize(const void *buf, size_t buf_size) {
	MemorySource ms(buf, buf_size);
	return deserialize<T>(ms);
}
