#pragma once

#include<chrono>
#include<cstdlib>

extern uint64_t _random_seed;
extern void set_random_seed(uint64_t seed);  // A fixed pseudo-random sequence with respect to the seed should be produced after calling this method

extern uint32_t hash_func1_32bit(const char* key);
extern uint32_t hash_func2_32bit(const char* key);
extern uint32_t hash_func3_32bit(uint32_t h);