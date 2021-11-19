#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include "LDCF.h"
#include "hashing.h"
#include "evaluation.h"
using namespace std;
typedef long long ll;
const int N = 23;

class LdcfEvaluation : public EvaluationBase {
	virtual void init() {
		F = new LogarithmicDynamicCuckooFilter(1<< N, 0.01);
	}
	virtual bool insert(char *key) {
		return F->insertItem(key);
	}
	virtual bool query(char *key) {
		return F->queryItem(key);
	}
	virtual bool remove(char *key) {
		return F->deleteItem(key);
	}
	virtual void debug() {
	}
	LogarithmicDynamicCuckooFilter* F;
} E;
int main(int argc, char* argv[]) {
	
	int n = (1<<N);
	E.gen_random_data(n);
	E.evaluation("LDCF");
	return 0;
}