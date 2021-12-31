#pragma once
#include "immintrin.h"
#include "hashing.h"
#include <cstdio>
#include <iostream>
#include <cassert>
#include <cstring>

#define SLOT_N 8
#define FINGERPRINT_T uint16_t
#define SMALL_BUCKET_PRIORITY 0
#define get_large_pos(pos, f, N) ((pos<<1)|(f&1)) ^ ((f>>1)&((N<<1)-1))
	

//#define USE_STATISTIC

struct OmnipotentConfig {
	int fp_len;    //fingerprint length (logically)
};

class Bucket {
	FINGERPRINT_T a[SLOT_N];
	uint8_t num;   // number of slots
public:
	Bucket() {
		memset(a, 0, sizeof(a));
		num = 0;
	}
	uint8_t size() const {return num;}
	bool is_full() const {return num==SLOT_N;}
	FINGERPRINT_T& operator [] (const int &t) {return a[t];}
	
	bool insert(FINGERPRINT_T f) {
		if (is_full()) return false;
		a[num++] = f;
		return true;
	}
	int query(FINGERPRINT_T f) {
		const __m128i item = _mm_set1_epi16(f);
        __m128i fp = _mm_set_epi16(a[7], a[6],a[5],a[4],a[3],a[2],a[1],a[0]);
        int matched = 0;

        __m128i a_comp = _mm_cmpeq_epi16(item, fp);
        matched = _mm_movemask_epi8(a_comp);

        if (matched != 0)
        {
           return 1;
        }
        return -1;
	}
	bool remove(FINGERPRINT_T f) {
		for (int i=0; i<num; i++)
			if (a[i] == f) {
				a[i] = a[--num];
				a[num] = 0;
				return true;
			}
		return false;
	}
	void replace(int i, FINGERPRINT_T f) {
		a[i] = f;
	}
};

typedef uint32_t ui;

class StaticOmnipotentFilter {
private:
	OmnipotentConfig config;
	Bucket *b1, *b2;   //b1 -- small    b2 -- large
	int BUCKET_N, L;
	int used_slots_num;
public:
	//statistic info
#ifdef USE_STATISTIC
	int b1_cnt[SLOT_N+1];
	int b2_cnt[SLOT_N+1];
	int kick_cnt;
	void show_statistic() {
		int b1_used = 0, b2_used = 0;
		for (int i=0; i<=SLOT_N; i++) b1_used += b1_cnt[i] * i;
		for (int i=0; i<=SLOT_N; i++) b2_used += b2_cnt[i] * i;
		int tot = BUCKET_N*SLOT_N*3;
		printf("(statistic) Load Factor: %.5lf  Kick ratio: %.3lf  Big/small: %.3lf\n", 1.0*(b1_used+b2_used)/tot, 1.0*kick_cnt/(b1_used+b2_used), 1.0*b2_used/b1_used);
		for (int i=0; i<=SLOT_N; i++) printf("%d ", b1_cnt[i]); puts("");
		for (int i=0; i<=SLOT_N; i++) printf("%d ", b2_cnt[i]); puts("");
	}
#endif

	StaticOmnipotentFilter(int max_insert_num) {
		used_slots_num = 0; 
		BUCKET_N = max_insert_num / 3 / SLOT_N;
		assert((BUCKET_N & (BUCKET_N-1)) == 0);
		L = 0;
		while ((1<<L) < BUCKET_N) L++;
		b1 = new Bucket [BUCKET_N];
		b2 = new Bucket [BUCKET_N<<1];
		config.fp_len = sizeof(FINGERPRINT_T)*8;
		#ifdef USE_STATISTIC
			b1_cnt[0] = BUCKET_N;
			b2_cnt[0] = BUCKET_N<<1;
		#endif	
	}
private:

	bool _insert(int p1, FINGERPRINT_T f) {
		int p2;
		//std::cout<<"insert:"<<p1<<" "<<p2<<" "<<f<<std::endl;
		
		if (b1[p1].size()*3<=(used_slots_num>>L)) {
			b1[p1].insert(f);
			#ifdef USE_STATISTIC
				b1_cnt[b1[p1].size()-1]--;
				b1_cnt[b1[p1].size()]++;
			#endif
		} else {
			p2 = get_large_pos(p1, f,BUCKET_N);
			if (b1[p1].size() <= b2[p2].size() + SMALL_BUCKET_PRIORITY && !b1[p1].is_full()) {
				b1[p1].insert(f);
				#ifdef USE_STATISTIC
					b1_cnt[b1[p1].size()-1]--;
					b1_cnt[b1[p1].size()]++;
				#endif
			} else if (b1[p1].size() > b2[p2].size() + SMALL_BUCKET_PRIORITY && !b2[p2].is_full()) {
				b2[p2].insert(f);
				#ifdef USE_STATISTIC
					b2_cnt[b2[p2].size()-1]--;
					b2_cnt[b2[p2].size()]++;
				#endif
			/*
			int expected = used_slots_num / (BUCKET_N*3) + 1;
			int sz1 = -1, sz2 = -1;
			int insert_id = -1;
			if (f%3==0) {
				sz1 = b1[p1].size();
				if (sz1<expected) {
					insert_id = 1;
				}
			} else {
				sz2 = b2[p2].size();
				if (sz2<expected) {
					insert_id = 2;
				}
			}
			if (insert_id == -1) {
				if (sz1==-1) sz1 = b1[p1].size();
				if (sz2==-1) sz2 = b2[p2].size();
			} else {
				static int hit_num = 0;
				hit_num++;
				if (rand()%10000==0) std::cerr << used_slots_num << " " << hit_num << std::endl;
			}
			if (insert_id != 2 && (insert_id == 1 || sz1 <= sz2 + config.SMALL_BUCKET_PRIORITY && sz1 < SLOT_N) ) {
				b1[p1].insert(f);
				#ifdef USE_STATISTIC
					b1_cnt[b1[p1].size()-1]--;
					b1_cnt[b1[p1].size()]++;
				#endif
			}
			else if (insert_id != 1 && (insert_id == 2 || sz1 > sz2 + config.SMALL_BUCKET_PRIORITY && sz2 < SLOT_N) ) {
				b2[p2].insert(f);
				#ifdef USE_STATISTIC
					b2_cnt[b2[p2].size()-1]--;
					b2_cnt[b2[p2].size()]++;
				#endif
				*/
			} else {
				int mn = SLOT_N, mnp = -1, mni = -1;
				FINGERPRINT_T mnf;
				for (int i=0; i<SLOT_N; i++) {
					FINGERPRINT_T cf = b1[p1][i];
					int p = get_large_pos(p1, cf,BUCKET_N);
					if (b2[p].size()<mn) {
						mn = b2[p].size();
						mnp = p;
						mni = i;
						mnf = cf;
					}
				}
				if (mnp==-1) return false;
				b2[mnp].insert(mnf);
				b1[p1].replace(mni, f);
				
				#ifdef USE_STATISTIC
					kick_cnt++;
					b2_cnt[b2[mnp].size()-1]--;
					b2_cnt[b2[mnp].size()]++;
				#endif
			}
		}
		used_slots_num++;
		return true;
	}

	bool _query(int p1, FINGERPRINT_T f) {
		int p2 = get_large_pos(p1, f,BUCKET_N);
		if (b2[p2].query(f)!=-1) return true;
		if (b1[p1].query(f)!=-1) return true;
		return false;
	}
	bool _remove(int p1, FINGERPRINT_T f) {
		int p2 = get_large_pos(p1, f,BUCKET_N);
		used_slots_num--;
		if (b2[p2].remove(f)) return true;
		if (b1[p1].remove(f)) return true;
		used_slots_num++;
		return false;
	}
public:
	bool insert_key(uint64_t key) {
		uint64_t hash = hash_func_64bit(key);
		return _insert((hash >> 32) & (BUCKET_N - 1), hash & 0x0000ffffu);
	}

	bool query_key(uint64_t key) {
		uint64_t hash = hash_func1_64bit(key);
		return _query((hash >> 32) & (BUCKET_N - 1), hash & 0x0000ffffu);
	}

	bool remove_key(uint64_t key) {
		uint64_t hash = hash_func1_64bit(key);
		return _remove((hash >> 32) & (BUCKET_N - 1), hash & 0x0000ffffu);
	}

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

	double memory_consumption_in_bytes(){
		return 3 * BUCKET_N * sizeof(Bucket);
	}
};
