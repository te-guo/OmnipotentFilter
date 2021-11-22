#pragma once

#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include<cassert>
#include<utility>
#include<sstream>
#include<map>
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
	char *key;
	int ans;
	int type;   // 0 -- insert   1 -- query
	bool checkpoint;
	Operation(int _type, char *_key, int _ans, bool _checkpoint) : key(_key), ans(_ans), type(_type), checkpoint(_checkpoint) {}
};
struct Status {
	int id;
	int fp;
	double t;
};
typedef vector<Operation> Data;

class DataGenerator {
	char* gen_string_by_int(long long x) {
		char *s = new char [19];
		for (int i=0; i<18; i++) {
			s[i] = '0'+x%10;
			x /= 10;
		}
		s[18] = 0;
		return s;
	}
public:
	void gen_random_data(Data &data, int n, bool no_query=false) {
		cerr<<"Generating random data n = " << n ;
		if (no_query) cerr << " no query";
		cerr << " ... ";
		data.clear();
		int m = n*1.1;
		long long *a = new long long[m+1];
		long long mod = 1000000000000000003ll, x = mod-1;
		for (int i=0; i<=m; i++) {
			a[i] = x;
			x = 1ll*x*3%mod;
		}
		int seg = 20;
		for(int i=0; i<seg; i++){
			int j=(long long)n*i/seg, k=(long long)n*(i+1)/seg;
			for(int l=j; l<k; ++l)
				data.push_back(Operation(0, gen_string_by_int(a[l]), 0, l==k-1));
			if(!no_query) {
				for(int l=j;l<k;++l) {
					if(rand()&1)
						data.push_back(Operation(1, gen_string_by_int(a[rand()%k]), true, l==k-1));
					else
						data.push_back(Operation(1, gen_string_by_int(a[k+rand()%(m-k+1)]), false, l==k-1));
				}
			}
		}
		cerr << "generated " << data.size() << " operations." << endl;
	}
};

class EvaluationBase {
	string log_dir, log_path;
	void open_log() {
		/*
		std::filesystem::path path = log_dir;
		if (!std::filesystem::exists(path)) {
			cerr << "Creating directories " << path << endl;
			std::filesystem::create_directories(path);
		}*/
		cerr<< "Saving log to " << log_path << endl;
		assert(freopen(log_path.c_str(), "a", stdout));
	}
	void _precompute_data() {
		insert_tot = query_tot = 0;
		int query_ans_cnt[2] = {};
		for (auto o : data) {
			if (o.type==0) {
				insert_tot++;
			} else {
				assert(o.type==1);
				query_tot++;
				assert(o.ans==0||o.ans==1);
				query_ans_cnt[o.ans]++;
			}
		}
		puts("======================================================");
		printf("Evaluating with data: #insert = %d, #query = %d", insert_tot, query_tot);
		if (query_tot>0) {
			printf(", #yes/#query = %.3lf", 1.0*query_ans_cnt[0]/query_tot);
		}
		puts("");
		puts("Procedure:");
	}
	void _evaluation(string eval_name) {
		assert(insert_tot > 100);

		int false_positive = 0, point_false_positive = 0;
		int false_negative = 0;
		int fail_num = 0;

		Timer T;
		for (int i=0; i<data.size(); i++) {
			if (data[i].type==0) {
				bool ok = insert(data[i].key);
				if (!ok)
					fail_num++, data[i].checkpoint = true;
			} else {
				bool ok = query(data[i].key) == data[i].ans;
				if (!ok)
					if(data[i].ans==false) point_false_positive++;
					else false_negative++;
			}
			if (data[i].checkpoint) {
				results.push_back((Status){i, point_false_positive, T.get()});
				false_positive += point_false_positive;
				point_false_positive = 0;
				debug();
				if (fail_num>=1) break;
			}
		}
		if (query_tot>0 && false_negative>0) {
			cout << "[!!!WARNING!!!] False negative = " << false_negative << endl;
			cerr << "[!!!WARNING!!!] False negative = " << false_negative << endl;  
		}
	}
	void _print_results() {
		int it = 0;
		int insert_num = 0, query_num = 0;
		int last_i = 0, last_query_num = 0, last_insert_num = 0; 
		double last_time = 0;
		
		map<string, double> tot_t;
		map<string, int> tot_num;
		
		for (int i=0; i<data.size(); i++) {
			if (data[i].type==0) insert_num++; else query_num++;
			if (it<results.size() && i==results[it].id) {
				string type = query_num == last_query_num ? "Insert" : (insert_num == last_insert_num ? "Query" : "Mixed");
				printf("%s ", type.c_str());

				int cur_num = i-last_i;
				double cur_t = results[it].t-last_time;
				double cur_tp = cur_num / cur_t;
				double lf = 1.0*insert_num/insert_tot;
				tot_t[type] += cur_t;
				tot_num[type] += cur_num;
				double avg_tp = tot_num[type] / tot_t[type];

				if(query_num == last_query_num)
					printf("Load_factor = %.4lf,  Throughput = %.2lf,  AVG_throughput = %.2lf\n",
							lf, cur_tp, avg_tp);
				else
					printf("Load_factor = %.4lf,  Throughput = %.2lf,  AVG_throughput = %.2lf,  FPR = %.8lf\n",
							lf, cur_tp, avg_tp, (double)results[it].fp/(query_num - last_query_num));
				
				last_i = i;
				last_time = results[it].t;
				last_insert_num = insert_num;
				last_query_num = query_num;
				it++;
			}
			if (it==results.size()) break;
		}	
	}
public:
	int max_insert_num;
	Data data;
	DataGenerator data_generator;
	vector<Status> results;
	int insert_tot = 0, query_tot = 0;

	virtual string get_filter_name() {
		assert(false);
	}
	virtual void init() {
		assert(false);
	}
	virtual bool insert(char *key) {
		assert(false);
	}
	virtual bool query(char *key) {
		assert(false);
	}
	virtual void debug() {
	}
	// get data and evaluation config (e.g. requirements) prepared 
	void prepare(string opt = "load_config", string config_path = "../eval_config.txt") {
		if (opt == "load_config") {
			ifstream in(config_path);
			assert(in.is_open());
			int n;
			in>>n; 
			max_insert_num = 1 << n;
			if (get_filter_name()=="Omnipotent") {
				max_insert_num += max_insert_num / 2;
			}
			data_generator.gen_random_data(data, max_insert_num);
			in.close();
		} else {
			assert(false);
		}
	}

	void evaluation(string eval_name = "", string path="../log") {
		time_t time_now = time(0);
		string time_str = (ctime(&time_now));
		for (int i=0; i<time_str.size(); i++) {
			if (time_str[i]==' ')
				time_str[i] = '+';
			if (time_str[i]==':')
				time_str[i] = '_';
		}
		while (time_str.back()<32) time_str.pop_back();
		log_dir = path + "/";		
//		log_path = log_dir + eval_name + time_str + ".txt";
		log_path = log_dir + eval_name + get_filter_name() + " " + time_str + ".txt";
		open_log();
		
		cout << get_filter_name() << endl;
		cout << "Evaluating [" << get_filter_name() << "] in evaluation [" << eval_name << "]" << endl;
		cerr << "Evaluating [" << get_filter_name() << "] in evaluation [" << eval_name << "]" << endl;
		
		init();
		_precompute_data();
		_evaluation(eval_name);
		_print_results();
	}

	~EvaluationBase() {
		for (auto o : data) {
			delete o.key;
		}
	}
	
};
