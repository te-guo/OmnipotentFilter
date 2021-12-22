#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include "LDCF.h"
#include "hashing.h"
#include "evaluation.h"
using namespace std;
typedef long long ll;

class LdcfEvaluation : public EvaluationBase {
public:
	string get_filter_name() {
		return "LDCF";
	}
	void init() {
		F = new LogarithmicDynamicCuckooFilter(max_capacity, 16);
	}
	bool insert(uint64_t key) {
		return F->insertItem(key);
	}
	bool query(uint64_t key) {
		return F->queryItem(key);
	}
	bool remove(uint64_t key) {
		return F->deleteItem(key);
	}
	size_t actual_size(){
		return F->actual_size_in_bytes();
	}
	void debug() {
	}
	LogarithmicDynamicCuckooFilter* F;
} E;
int main(int argc, char* argv[]) {
	E.evaluation(argc, argv);
	return 0;
}