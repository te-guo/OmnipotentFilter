#pragma once
#include "immintrin.h"
#include "hashing.h"
#include <cstdio>
#include <iostream>
#include <cassert>
#include <cstring>
#include <utility>

#define SLOT_N 8
#define FINGERPRINT_T uint16_t

class BucketBase {
public:
	virtual void insert(const uint64_t &f) = 0;
	virtual int query(const uint64_t &f) = 0;
	virtual bool remove(const uint64_t &f) = 0;
	virtual void replace(const int &i, const uint64_t &f) = 0;
	virtual uint64_t get(const int &t) = 0;
	virtual int size() const = 0;
};


class Bucket : public BucketBase {
	FINGERPRINT_T a[SLOT_N];
	uint8_t num;   // number of slots
public:
	Bucket() {
		memset(a, 0, sizeof(a));
		num = 0;
	}
	int size() const {return num;}
	bool is_full() const {return num == SLOT_N;}
	FINGERPRINT_T& operator [] (const int &t) {return a[t];}
	uint64_t get(const int &t) {return a[t];}
	
	void insert(const uint64_t &f) {
		a[num++] = f;
	}
	int query(const uint64_t &f) {
		const __m128i item = _mm_set1_epi16(f);
        __m128i fp = _mm_set_epi16(a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0]);
        int matched = 0;
        __m128i a_comp = _mm_cmpeq_epi16(item, fp);
        matched = _mm_movemask_epi8(a_comp);
        if (matched != 0) {
           return 1;
        }
        return -1;
	}
	bool remove(const uint64_t &f) {
		for (int i = 0; i < num; i++) {
			if (a[i] == f) {
				a[i] = a[--num];
				a[num] = 0;
				return true;
			}
		}
		return false;
	}
	void replace(const int &i, const uint64_t &f) {
		a[i] = f;
	}
};

class TableFractionBase {
public:
	uint8_t level;
	TableFractionBase(uint8_t _level) : level(_level) {
	}
	virtual BucketBase* get(const uint64_t &index) = 0;
	virtual double memory_consumption_in_bytes() = 0;
	virtual ~TableFractionBase() = default;
};
class TableFraction : public TableFractionBase{
	Bucket *b;
public:
	TableFraction (uint8_t _level) : TableFractionBase(_level) {
		b = new Bucket [1 << _level];
	}
	BucketBase* get(const uint64_t &index) override {
		return (BucketBase*) (b + index);
	}
	double memory_consumption_in_bytes() {
		return (SLOT_N * 2 + 1) * (1 << level);
	}
	~TableFraction() {
		delete [] b;
	}
};

const int FRAC_K = 0;
class OmnipotentFilter {
private:
	TableFractionBase *H[1<<FRAC_K], *L[1<<FRAC_K];
	int rfl; // raw fingerprint length
	bool is_dynamic;
public:
	OmnipotentFilter(int bits_recorded_per_key, int init_filter_size, bool auto_resize = true) {
		int l = 0;
		while (init_filter_size > (1<<l)) l++;
		rfl = std::max(FRAC_K, bits_recorded_per_key);
		is_dynamic = auto_resize;
		int init_l = std::max(0, l - FRAC_K - 4);
		for (int i = 0; i < (1 << FRAC_K); i++) {
			// [TODO] use different table fraction according to number of required bits
			H[i] = new TableFraction(init_l);
			L[i]  = new TableFraction(init_l);
		}
		std::cerr << "init with rfl = " << rfl << ", init_l = " << init_l << std::endl;
	}
	~OmnipotentFilter() {
		for (int i = 0; i < (1 << FRAC_K); i++) {
			delete H[i];
			delete L[i];
		}
	}
private:
	#define high_fingerprint(key, level) ((key) & ((1ull << (rfl - FRAC_K - level)) - 1))
	#define high_index(key, level) ((key & ((1ull << (rfl - FRAC_K)) - 1)) >> (rfl - FRAC_K - level))
	#define high_fracid(key) ((key) >> (rfl - FRAC_K))
	#define high_fingerprint_mask(level) ((1ull << (rfl - FRAC_K - level)) - 1)
	#define low_fingerprint(key, level) ((key) >> (FRAC_K + level))
	#define low_index(key, level) (((key) >> FRAC_K) & ((1 << level) - 1))
	#define low_fracid(key) ((key) & ((1ull << FRAC_K) - 1))
	#define low_fingerprint_mask(level) (((1ull << (rfl - FRAC_K - level)) - 1) << (FRAC_K + level))
	std::pair<TableFractionBase*, BucketBase*> locate_high(uint64_t key) {
		auto high_fraction = H[high_fracid(key)];
		auto high_bucket = high_fraction->get(high_index(key, high_fraction->level));
		return std::make_pair(high_fraction, high_bucket);
	}
	std::pair<TableFractionBase*, BucketBase*> locate_low(uint64_t key) {
		auto low_fraction = L[low_fracid(key)];
		auto low_bucket = low_fraction->get(low_index(key, low_fraction->level));
		return std::make_pair(low_fraction, low_bucket);
	}
	bool _kick_high(uint64_t key, BucketBase *b, int level) {
		uint64_t mask = key & ~high_fingerprint_mask(level);
		for (int i = 0; i < b->size(); i++) {
			uint64_t nkey = mask | b->get(i);
			auto low = locate_low(nkey);
			int low_size = low.second->size();
			if (low_size < SLOT_N) {
				low.second->insert(low_fingerprint(nkey, low.first->level));
				b->replace(i, high_fingerprint(key, level));
				return true;
			}
		}
		return false;
	}
	bool _kick_low(uint64_t key, BucketBase *b, int level) {
		uint64_t mask = key & ~low_fingerprint_mask(level);
		for (int i = 0; i < b->size(); i++) {
			uint64_t nkey = mask | (b->get(i) << (FRAC_K + level));
			auto high = locate_high(nkey);
			int high_size = high.second->size();
			if (high_size < SLOT_N) {
				high.second->insert(high_fingerprint(nkey, high.first->level));
				b->replace(i, low_fingerprint(key, level));
				return true;
			}
		}
		return false;
	}
	bool _insert(uint64_t key, bool key_native) {
		//std::cerr<<"insert:"<<key<<std::endl;
		//std::cerr<<high_fracid(key)<<std::endl;
		if (key_native) {
			auto high = locate_high(key);
			int high_size = high.second->size();
			uint64_t high_fp = high_fingerprint(key, high.first->level);
			if (high_size < SLOT_N) {
				//std::cerr << "high_fp = " << high_fp << std::endl;
				high.second->insert(high_fp);
			} else {
				auto low = locate_low(key);
				int low_size = low.second->size();
				uint64_t low_fp = low_fingerprint(key, low.first->level);
				if (low_size < SLOT_N) {
					low.second->insert(low_fp);
				} else {
					if (!_kick_high(key, high.second, high.first->level)) {
						if (!_kick_low(key, low.second, low.first->level)) {
					 		return false;
						}
					}
				}
			}
		} else {
			auto low = locate_low(key);
			int low_size = low.second->size();
			uint64_t low_fp = low_fingerprint(key, low.first->level);
			if (low_size < SLOT_N) {
				low.second->insert(low_fp);
			} else {
				auto high = locate_high(key);
				int high_size = high.second->size();
				uint64_t high_fp = high_fingerprint(key, high.first->level);
				if (high_size < SLOT_N) {
					high.second->insert(high_fp);
				} else {
					if (!_kick_low(key, low.second, low.first->level)) {
						if (!_kick_high(key, high.second, high.first->level)) {
							return false;
						}
					}
				}
			}
		}
		return true;
	}

	bool _query(uint64_t key, bool key_native) {
		if (key_native) {
			auto high = locate_high(key);
			if (high.second->query(high_fingerprint(key, high.first->level)) != -1) return true;
			auto low = locate_low(key);
			if (low.second->query(low_fingerprint(key, low.first->level)) != -1) return true;
		} else {
			auto low = locate_low(key);
			if (low.second->query(low_fingerprint(key, low.first->level)) != -1) return true;
			auto high = locate_high(key);
			if (high.second->query(high_fingerprint(key, high.first->level)) != -1) return true;	
		}
		return false;
	}
	bool _remove(uint64_t key, bool key_native) {
		if (key_native) {
			auto high = locate_high(key);
			if (high.second->remove(high_fingerprint(key, high.first->level))) return true;
			auto low = locate_low(key);
			if (low.second->remove(low_fingerprint(key, low.first->level))) return true;
		} else {
			auto low = locate_low(key);
			if (low.second->remove(low_fingerprint(key, low.first->level))) return true;
			auto high = locate_high(key);
			if (high.second->remove(high_fingerprint(key, high.first->level))) return true;	
		}
		return false;
	}
public:
	bool insert_key(uint64_t key) {
		uint64_t hash = hash_func_64bit(key);
		return _insert(hash & ((1ull << rfl) - 1), hash >> 63);
	}

	bool query_key(uint64_t key) {
		uint64_t hash = hash_func_64bit(key);
		return _query(hash & ((1ull << rfl) - 1), hash >> 63);
	}

	bool remove_key(uint64_t key) {
		uint64_t hash = hash_func_64bit(key);
		return _remove(hash & ((1ull << rfl) - 1), hash >> 63);
	}
/*
	bool insert_key(char *key) {
		uint64_t hash = hash_func1_64bit(key);
		return _insert((hash >> 32) & (BUCKET_N - 1), hash & 0x0000ffffu);
	}

	bool query_key(char *key) {
		uint64_t hash = hash_func1_64bit(key);
		return _query((hash >> 32) & (BUCKET_N - 1), hash & 0x0000ffffu);
	}

	bool remove_key(char *key) {
		uint64_t hash = hash_func1_64bit(key);
		return _remove((hash >> 32) & (BUCKET_N - 1), hash & 0x0000ffffu);
	}
*/
	double memory_consumption_in_bytes() {
		double ret = 0;
		for (int i = 0; i < (1 << FRAC_K); i++) {
			ret += H[i]->memory_consumption_in_bytes();
			ret += L[i]->memory_consumption_in_bytes();
		}
		return ret;
	}
};
