#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include "omnipotent.h"
#include "evaluation.h"
using namespace std;
typedef long long ll;
const int N = 19;

class OmnipotentEvaluation : public EvaluationBase {
	
StaticOmnipotentFilter<1<<N, 8, uint16_t> F;
	virtual void init() {

	}
	virtual bool insert(char *key) {
		return F.insert_key(key);
	}
	virtual bool query(char *key) {
		return F.query_key(key);
	}
	virtual void debug() {
//		F.show_statistic();
	}
} E;
int main(int argc, char* argv[]) {
	
	int n = (1<<N)*8*3;
	E.gen_random_data(n);
	E.evaluation("Omnipotent");
	return 0;
}