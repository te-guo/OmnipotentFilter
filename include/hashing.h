#include<chrono>
#include<cstdlib>

#ifndef HASHING_HEADER_DEF
#define HASHING_HEADER_DEF

void set_random_seed(uint64_t seed);

uint32_t hash_func1_32bit(const char* key);
uint32_t hash_func2_32bit(const char* key);
uint32_t hash_func3_32bit(uint32_t h);

#endif