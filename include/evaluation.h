#pragma once

#include<iostream>
#include<cstdio>
#include<algorithm>
#include<chrono>
#include<vector>
#include<cassert>
#include<utility>
#include<sstream>
#include<map>
#include<set>
#include<fstream>
//#include <filesystem>

using namespace std;

struct Timer {
	chrono::high_resolution_clock::time_point _time_stamp;
	void set() {
		_time_stamp = chrono::high_resolution_clock::now();
	}
	Timer() {
		set();
	}
	double get() {  //in seconds
		auto cur = chrono::high_resolution_clock::now();
		return chrono::duration_cast<chrono::microseconds>(cur-_time_stamp).count()/1000000.0;
	}
};

struct Operation {
	uint64_t key;
	bool ans;
	int type;   // 0 -- insert   1 -- query   2 -- remove
	bool checkpoint;
	Operation(int _type, uint64_t _key, bool _ans, bool _checkpoint) : key(_key), ans(_ans), type(_type), checkpoint(_checkpoint) {}
};
struct Status {
	int id;
	int fp;
	double t;
	long size;
};
typedef vector<Operation> Data;

class DataGenerator {
	char* gen_string_by_int(long long x);
public:
	void gen_random_data(Data &data, int n, bool no_query=false, bool no_remove=false);
};

class EvaluationBase {
	string log_dir, log_path;
	std::map<std::string, std::string> arguments;
	void open_log();
	int argu_int(char* argu);
	void _load_config(string opt = "load_config", string config_path = "../default_config.txt");
	void _generate_input();
	void _precompute_data();
	void _evaluation(string eval_name);
	void _print_results();
public:
	int max_capacity;
	Data data;
	DataGenerator data_generator;
	vector<Status> checkpoints;
	int op_tot[3];

	virtual string get_filter_name();
	virtual void init();
	virtual bool insert(uint64_t key);
	virtual bool query(uint64_t key);
	virtual bool remove(uint64_t key);
	virtual size_t actual_size();
	virtual void debug();
	void evaluation(int argc, char* argv[], string path="../log");

	~EvaluationBase();

};

string get_time_str();