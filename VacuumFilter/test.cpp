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
		F.init(max_insert_num, 4, 400);
	}
	bool insert(char *key) {
		return F.insert(key);
	}
	bool query(char *key) {
		return F.lookup(key);
	}
	bool remove(char *key) {
		return F.del(key);
	}
	void debug() {
		F.debug_test();
	}
	VacuumFilter<uint16_t, 16> F;
} E;
int main(int argc, char* argv[]) {
	E.prepare();
	if(argc >= 2)
		E.evaluation(argv[1], false);
	else
		E.evaluation();
	return 0;
}