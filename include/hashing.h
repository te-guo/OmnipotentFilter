#pragma once

#include<chrono>
#include<cstdlib>

extern uint64_t _random_seed;
extern void set_random_seed(uint64_t seed);  // A fixed pseudo-random sequence with respect to the seed should be produced after calling this method

extern uint64_t hash_func_64bit(uint64_t key);
extern uint64_t hash_func_64bit(const char* key);
#define hash_func1_64bit hash_func_64bit

extern uint32_t hash_func3_32bit(uint32_t h);
extern uint32_t hash_func4_32bit(uint32_t h);