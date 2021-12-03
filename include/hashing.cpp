#include "hashing.h"
#include "MurmurHash3.h"
#include <cstring>

uint64_t _random_seed = 0x8091a2b3c4d5e6f7;
void set_random_seed(uint64_t seed) {  // A fixed pseudo-random sequence with respect to the seed should be produced after calling this method
	_random_seed = seed;
}

uint64_t* hash_func_64bit(uint64_t key) {
	uint64_t ret[2];
	MurmurHash3_x64_128(&key, 8, _random_seed, ret);
	return ret;
}

uint64_t* hash_func_64bit(const char* key) {
	uint64_t ret[2];
	MurmurHash3_x64_128(key, strlen(key), _random_seed, ret);
	return ret;
}


// The following hash functions are to be discarded 
uint32_t hash_func1_32bit(const char* key) {
	uint64_t hash_value = _random_seed;
	for (int i=0; key[i]; i++) {
		hash_value ^= (hash_value * 233 + hash_value/666 + key[i]);
	}
	for (int i=0; key[i]; i++) {
		hash_value ^= (hash_value * 233 + hash_value/666 + key[i]);
	}
	return hash_value;
}
uint32_t hash_func2_32bit(const char* key) {
	uint64_t hash_value = _random_seed;
	for (int i=0; key[i]; i++) {
		hash_value ^= (hash_value * 2333 + hash_value/6666 + key[i]);
	}
	for (int i=0; key[i]; i++) {
		hash_value ^= (hash_value * 2333 + hash_value/6666 + key[i]);
	}
	return hash_value;
}
uint32_t hash_func3_32bit(uint32_t fp){
	uint64_t h = fp ^ _random_seed;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}
uint32_t hash_func4_32bit(uint32_t fp){
	uint64_t h = fp + _random_seed;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}