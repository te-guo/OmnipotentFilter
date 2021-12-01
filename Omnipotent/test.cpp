#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include "omnipotent.h"
#include "evaluation.h"
using namespace std;
typedef long long ll;

class OmnipotentEvaluation : public EvaluationBase {
	StaticOmnipotentFilter<uint16_t> *F;
	string get_filter_name() {return "Omnipotent";}
	void init() {
		F = new StaticOmnipotentFilter<uint16_t> (max_capacity);
	}
	bool insert(char *key) {
		return F->insert_key(key);
	}
	bool query(char *key) {
		return F->query_key(key);
	}
	bool remove(char *key) {
		return F->remove_key(key);
	}
	void debug() {
//		F.show_statistic();
	}
} E;
int main(int argc, char* argv[]) {
	E.prepare();
	if(argc >= 2)
		E.evaluation(argv[1], false);
	else
		E.evaluation();
	return 0;
}