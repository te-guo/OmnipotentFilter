#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include "vacuum.h"
#include "evaluation.h"
using namespace std;
typedef long long ll;
const int N = 23;

class VacuumEvaluation : public EvaluationBase {
	virtual void init() {
		F.init(1 << N, 4, 400);
	}
	virtual bool insert(char *key) {
		return F.insert(key);
	}
	virtual bool query(char *key) {
		return F.lookup(key);
	}
	virtual bool remove(char *key) {
		return F.del(key);
	}
	virtual void debug() {
		F.debug_test();
	}
	VacuumFilter<uint16_t, 16> F;
} E;
int main(int argc, char* argv[]) {
	
	int n = (1<<N);
	auto data = gen_random_data(n);
	//for (int i=0; i<100; i++) cout<<data[i].key<<endl;
	E.data = data;
	E.evaluation();
	return 0;
}