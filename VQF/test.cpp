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
		F = vqf_init(max_insert_num);
	}
	bool insert(char *key) {
		return vqf_insert(F, hash_func1_32bit(key));
	}
	bool query(char *key) {
		return vqf_is_present(F, hash_func1_32bit(key));
	}
	bool remove(char *key) {
		return vqf_remove(F, hash_func1_32bit(key));
	}
	void debug() {
	}
	vqf_filter* F;
} E;
int main(int argc, char* argv[]) {
	E.prepare();
	E.evaluation("VQF");
	return 0;
}