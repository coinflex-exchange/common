#include "hmac.h"


template <typename Hash_Type>
constexpr size_t HMAC<Hash_Type>::digest_size;

template <typename Hash_Type>
HMAC<Hash_Type>::HMAC(const void *key, size_t n) {
	if (n > sizeof secret) {
		hash.write(key, n);
		std::memcpy(secret, hash.digest().data(), n = std::min(hash_type::digest_size, sizeof secret));
		hash = hash_type();
	}
	else {
		std::memcpy(secret, key, n);
	}
	if (n < hash_type::block_size) {
		std::memset(secret + n, 0, sizeof secret - n);
	}
	for (size_t i = 0; i < sizeof secret; ++i) {
		secret[i] ^= 0x36;
	}
	hash.write(secret, sizeof secret);
}

template <typename Hash_Type>
const typename HMAC<Hash_Type>::digest_type & HMAC<Hash_Type>::digest() {
	auto ihash = hash.digest();
	hash = hash_type();
	for (size_t i = 0; i < sizeof secret; ++i) {
		secret[i] ^= 0x36 ^ 0x5c;
	}
	hash.write(secret, sizeof secret);
	hash.write(ihash.data(), ihash.size());
	return hash.digest();
}
