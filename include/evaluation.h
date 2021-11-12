#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include<cassert>
#include<utility>
#include<sstream>
using namespace std;
auto _time_stamp = chrono::high_resolution_clock::now();
void set_time_stamp() {
	_time_stamp = chrono::high_resolution_clock::now();
}
double get_time() {  //in seconds
	auto cur = chrono::high_resolution_clock::now();
	return chrono::duration_cast<chrono::microseconds>(cur-_time_stamp).count()/1000000.0;
}

struct Operation {
	char *key;
	int ans;
	int type;   // 0 -- insert   1 -- query
	Operation(int _type, char *_key, int _ans) : key(_key), ans(_ans), type(_type) {}
};

class EvaluationBase {
public:
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
		assert(false);
	}

	vector<Operation> data;
	void evaluation() {
		init();
		int insert_num = 0, query_num = 0, query_ans_cnt[2] = {};
		for (auto o : data) {
			if (o.type==0) {
				insert_num++;
			} else {
				assert(o.type==1);
				query_num++;
				assert(o.ans==0||o.ans==1);
				query_ans_cnt[o.ans]++;
			}
		}
		puts("======================================================");
		printf("Evaluating with data: #insert=%d, #query=%d", insert_num, query_num);
		if (query_num>0) {
			printf(", #yes/#query=%.3lf", 1.0*query_ans_cnt[0]/query_num);
		}
		puts("");

		int insert_tot = insert_num, query_tot = query_num;
		assert(insert_tot > 100);

		int false_positive = 0;
		int false_negative = 0;
		bool false_negative_before_failure = false;
		int fail_num = 0;
		vector<int> points;
		vector<pair<int,pair<double,int> > > results;
		int it = 0;
		for (int i=1; i<=99; i++) points.push_back((insert_tot+query_tot)*i/100-1);
		points.push_back(int(data.size())-1);

		set_time_stamp();
		for (int i=0; i<data.size(); i++) {
			bool is_point = it<points.size() && i==points[it];
			if (data[i].type==0) {
				bool ok = insert(data[i].key);
				if (!ok) {
					fail_num++;
					is_point = true;
				}
			} else {
				bool ok = query(data[i].key) == data[i].ans;
				if (!ok && data[i].ans==false) false_positive++;
				if (!ok && data[i].ans==true){
					false_negative++;
					if(fail_num == 0)
						false_negative_before_failure = true;
				}
			}
			if (is_point) {
				results.push_back(make_pair(i, make_pair(get_time(), fail_num)));
				if (it<points.size() && i==points[it]) it++;
				debug();
			}
			if (fail_num>=10) break;
		}

		it = insert_num = query_num = 0;
		int last_i = 0; 
		double last_time = 0;
		for (int i=0; i<data.size(); i++) {
			if (data[i].type==0) insert_num++; else query_num++;
			if (it<results.size() && i==results[it].first) {
				auto t = results[it].second;
				printf("@Load factor=%.4lf : Throughput=%.2lf, AVG throughput=%.2lf, #Insertion fails=%d\n",
						 1.0*insert_num/insert_tot, (i-last_i)/(t.first-last_time), i/t.first, t.second);
				last_i = i;
				last_time = t.first;
				it++;
			}
			if (it==results.size()) break;
		}
		if (query_num>0) {
			printf("fpr = %.8lf\n", 1.0*false_positive/query_num);
			if (false_negative>0) {
				if(false_negative_before_failure)
					printf("!!!!!!!!!!!!!!! false negative = %d\n", false_negative);
				else
					printf("false negative (only after insertion failure) = %d\n", false_negative);
			}
		}
	}

	~EvaluationBase() {
		for (auto o : data) {
			delete o.key;
		}
	}
};

char* gen_string_by_int(long long x) {
	char *s = new char [19];
	for (int i=0; i<18; i++) {
		s[i] = '0'+x%10;
		x /= 10;
	}
	s[18] = 0;
	return s;
}
vector<Operation> gen_random_data(int n, bool no_query=false) {
	vector<Operation> ret;
	long long *a = new long long[n+1];
	long long m = 1000000000000000003ll, x = m-1;
	for (int i=0; i<=n; i++) {
		a[i] = x;
		x = 1ll*x*3%m;
	}
	for (int i=0; i<n; i++) {
		ret.push_back(Operation(0, gen_string_by_int(a[i]), 0));
		if (!no_query) {
			if (rand()&1) {
				ret.push_back(Operation(1, gen_string_by_int(a[rand()%(i+1)]), true));
			} else {
				ret.push_back(Operation(1, gen_string_by_int(a[i+1+rand()%(n-i)]), false));	
			}
		}
	}
	return ret;
}