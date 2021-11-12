#include "hashing.h"

uint64_t _random_seed;
void set_random_seed(uint64_t seed) {  // A fixed pseudo-random sequence with respect to the seed should be produced after calling this method
	_random_seed = seed;
}

uint32_t hash_func1_32bit(const char* key) {
	uint32_t hash_value = _random_seed;
	for (int i=0; key[i]; i++) {
		hash_value ^= (hash_value * 233 + hash_value/666 + key[i]);
	}
	for (int i=0; key[i]; i++) {
		hash_value ^= (hash_value * 233 + hash_value/666 + key[i]);
	}
	return hash_value;
}
uint32_t hash_func2_32bit(const char* key) {
	uint32_t hash_value = _random_seed;
	for (int i=0; key[i]; i++) {
		hash_value ^= (hash_value * 2333 + hash_value/6666 + key[i]);
	}
	for (int i=0; key[i]; i++) {
		hash_value ^= (hash_value * 2333 + hash_value/6666 + key[i]);
	}
	return hash_value;
}
uint32_t hash_func3_32bit(uint32_t h){
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}