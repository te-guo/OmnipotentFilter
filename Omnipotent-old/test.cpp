#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include "omnipotent.h"
#include "evaluation.h"
using namespace std;
typedef long long ll;

#define SELECTED_FILTER StaticOmnipotentFilter
class OmnipotentEvaluation : public EvaluationBase {
	SELECTED_FILTER *F;
	string get_filter_name() {return "Omnipotent-old";}
	void init() {
		F = new SELECTED_FILTER (max_capacity);
	}
	bool insert(uint64_t key) {
		return F->insert_key(key);
	}
	bool query(uint64_t key) {
		return F->query_key(key);
	}
	bool remove(uint64_t key) {
		return F->remove_key(key);
	}
	size_t actual_size(){
		return F->memory_consumption_in_bytes();
	}
	void debug() {
//		F.show_statistic();
	}
} E;
int main(int argc, char* argv[]) {
	E.evaluation(argc, argv);
	return 0;
}