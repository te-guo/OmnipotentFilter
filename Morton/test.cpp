#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include "morton_filter.h"
#include "evaluation.h"
using namespace std;
typedef long long ll;

class VqfEvaluation : public EvaluationBase {
	string get_filter_name() {
		return "Morton";
	}
	void init() {
		F = new CompressedCuckoo::Morton3_12(max_capacity);
	}
	bool insert(uint64_t key) {
		return F->insert(key);
	}
	bool query(uint64_t key) {
		return F->likely_contains(key);
	}
	bool remove(uint64_t key) {
		return F->delete_item(key);
	}
	size_t actual_size(){
		return 0; ///// TODO /////
	}
	void debug() {
	}
	CompressedCuckoo::Morton3_12 *F;
} E;
int main(int argc, char* argv[]) {
	E.evaluation(argc, argv);
	return 0;
}