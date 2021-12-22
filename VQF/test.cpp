#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include "vqf_filter.h"
#include "hashing.h"
#include "evaluation.h"
using namespace std;
typedef long long ll;

class VqfEvaluation : public EvaluationBase {
	string get_filter_name() {
		return "VQF";
	}
	void init() {
		F = vqf_init(max_capacity);
	}
	bool insert(uint64_t key) {
		return vqf_insert(F, hash_func1_64bit(key));
	}
	bool query(uint64_t key) {
		return vqf_is_present(F, hash_func1_64bit(key));
	}
	bool remove(uint64_t key) {
		return vqf_remove(F, hash_func1_64bit(key));
	}
	size_t actual_size(){
		return F->metadata.total_size_in_bytes;
	}
	void debug() {
	}
	vqf_filter* F;
} E;
int main(int argc, char* argv[]) {
	E.evaluation(argc, argv);
	return 0;
}