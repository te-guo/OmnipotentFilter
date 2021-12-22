#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include "vacuum.h"
#include "evaluation.h"
using namespace std;
typedef long long ll;
class VacuumEvaluation : public EvaluationBase {
	string get_filter_name() {
		return "VacuumFilter";
	}
	void init() {
		F.init(max_capacity, 4, 400);
	}
	bool insert(uint64_t key) {
		return F.insert(hash_func1_64bit(key));
	}
	bool query(uint64_t key) {
		return F.lookup(hash_func1_64bit(key));
	}
	bool remove(uint64_t key) {
		return F.del(hash_func1_64bit(key));
	}
	size_t actual_size(){
		return F.memory_consumption;
	}
	void debug() {
		F.debug_test();
	}
	VacuumFilter<uint16_t, 16> F;
} E;
int main(int argc, char* argv[]) {
	E.evaluation(argc, argv);
	return 0;
}