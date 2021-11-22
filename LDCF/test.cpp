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
		F = new LogarithmicDynamicCuckooFilter(max_insert_num, 0.01);
	}
	bool insert(char *key) {
		return F->insertItem(key);
	}
	bool query(char *key) {
		return F->queryItem(key);
	}
	bool remove(char *key) {
		return F->deleteItem(key);
	}
	void debug() {
	}
	LogarithmicDynamicCuckooFilter* F;
} E;
int main(int argc, char* argv[]) {
	E.prepare();
	E.evaluation();
	return 0;
}