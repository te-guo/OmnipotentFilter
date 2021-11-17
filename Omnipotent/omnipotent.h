#include "hashing.h"
#include <cstdio>
#include <iostream>
#include <cassert>

#ifndef OMNIPOTENT_HEADER_DEF
#define OMNIPOTENT_HEADER_DEF

struct OmnipotentConfig {
	int fp_len;    //fingerprint length (logically)
	int SMALL_BUCKET_PRIORITY = 0;
};

template<class FINGERPRINT_T, int SLOT_N>
class Bucket {
	FINGERPRINT_T a[SLOT_N];
	uint8_t num;   // number of slots
public:
	uint8_t size() const {return num;}
	bool is_full() const {return num==SLOT_N;}
	FINGERPRINT_T& operator [] (const int &t) {return a[t];}
	
	bool insert(FINGERPRINT_T f) {
		if (is_full()) return false;
		a[num++] = f;
		return true;
	}
	int query(FINGERPRINT_T f) {
		for (int i=0; i<SLOT_N; i++) {
			if (a[i] == f) {
				return i;
			}
		}
		return -1;
	}
	void replace(int i, FINGERPRINT_T f) {
		a[i] = f;
	}
};

typedef uint32_t ui;
#define USE_STATISTIC

template<int BUCKET_N, int SLOT_N, class FINGERPRINT_T>   //FINGERPRINT_T  must be  uint??_t
class StaticOmnipotentFilter {
private:
	OmnipotentConfig config;
	Bucket<FINGERPRINT_T, SLOT_N> b1[BUCKET_N], b2[BUCKET_N<<1];   //b1 -- small    b2 -- large
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

	StaticOmnipotentFilter() {
		config.fp_len = sizeof(FINGERPRINT_T)*8;
		#ifdef USE_STATISTIC
			b1_cnt[0] = BUCKET_N;
			b2_cnt[0] = BUCKET_N<<1;
		#endif	
	}
private:

	int get_large_pos(const int &pos, const FINGERPRINT_T &f) {
		return ((pos<<1)|(f&1)) ^ ((f>>1)%(BUCKET_N<<1));            //!!![MODIFIED]
	}

	bool _insert(int p1, FINGERPRINT_T f) {
		int p2 = get_large_pos(p1, f);
		//std::cout<<"insert:"<<p1<<" "<<p2<<" "<<f<<std::endl;
		if (b1[p1].size() <= b2[p2].size() + config.SMALL_BUCKET_PRIORITY && !b1[p1].is_full()) {
			b1[p1].insert(f);
			
			#ifdef USE_STATISTIC
				b1_cnt[b1[p1].size()-1]--;
				b1_cnt[b1[p1].size()]++;
			#endif
		} else if (b1[p1].size() > b2[p2].size() + config.SMALL_BUCKET_PRIORITY && !b2[p2].is_full()) {
			b2[p2].insert(f);

			#ifdef USE_STATISTIC
				b2_cnt[b2[p2].size()-1]--;
				b2_cnt[b2[p2].size()]++;
			#endif
		} else {
			int mn = SLOT_N, mnp = -1, mni = -1;
			FINGERPRINT_T mnf;
			for (int i=0; i<SLOT_N; i++) {
				FINGERPRINT_T cf = b1[p1][i];
				int p = get_large_pos(p1, cf);
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
		return true;
	}

	bool _query(int p1, FINGERPRINT_T f) {
		int p2 = get_large_pos(p1, f);
		if (b2[p2].query(f)!=-1) return true;
		if (b1[p1].query(f)!=-1) return true;
		return false;
	}
public:
	bool insert_key(char *key) {
		return _insert(hash_func1_32bit(key)%BUCKET_N, hash_func2_32bit(key));
	}

	bool query_key(char *key) {
		return _query(hash_func1_32bit(key)%BUCKET_N, hash_func2_32bit(key));
	}
};

#endif