#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include "vqf_filter.h"
#include "hashing.h"
#include "evaluation.h"
using namespace std;
typedef long long ll;
const int N = 23;

class VqfEvaluation : public EvaluationBase {
	virtual void init() {
		F = vqf_init(1 << N);
	}
	virtual bool insert(char *key) {
		return vqf_insert(F, hash_func1_32bit(key));
	}
	virtual bool query(char *key) {
		return vqf_is_present(F, hash_func1_32bit(key));
	}
	virtual bool remove(char *key) {
		return vqf_remove(F, hash_func1_32bit(key));
	}
	virtual void debug() {
	}
	vqf_filter* F;
} E;
int main(int argc, char* argv[]) {
	
	int n = (1<<N);
	E.gen_random_data(n);
	E.evaluation("VQF");
	return 0;
}