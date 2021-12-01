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
	bool insert(char *key) {
		return F->insert(key);
	}
	bool query(char *key) {
		return F->likely_contains(key);
	}
	bool remove(char *key) {
		return F->delete_item(key);
	}
	void debug() {
	}
	CompressedCuckoo::Morton3_12 *F;
} E;
int main(int argc, char* argv[]) {
	E.prepare();
	if(argc >= 2)
		E.evaluation(argv[1], false);
	else
		E.evaluation();
	return 0;
}