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
	bool checkpoint;
	Operation(int _type, char *_key, int _ans, bool _checkpoint) : key(_key), ans(_ans), type(_type), checkpoint(_checkpoint) {}
};
struct Status {
	int id;
	int fp;
	double t;
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

		int false_positive = 0, point_false_positive = 0;
		int false_negative = 0;
		int fail_num = 0;
		vector<Status> results;

		set_time_stamp();
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
				results.push_back((Status){i, point_false_positive, get_time()});
				false_positive += point_false_positive;
				point_false_positive = 0;
				debug();
				if (fail_num>=1) break;
			}
		}

		int it = 0;
		insert_num = query_num = 0;
		int last_i = 0, last_query_num = 0, last_insert_num = 0; 
		double last_time = 0;
		for (int i=0; i<data.size(); i++) {
			if (data[i].type==0) insert_num++; else query_num++;
			if (it<results.size() && i==results[it].id) {
				if(query_num == last_query_num)
					printf("Insert ");
				else if(insert_num == last_insert_num)
					printf("Query  ");
				else
					printf("Mixed  ");
				if(query_num == last_query_num)
					printf("@Load factor=%.4lf : Throughput = %.2lf,  AVG throughput = %.2lf\n",
							1.0*insert_num/insert_tot, (i-last_i)/(results[it].t-last_time), i/results[it].t);
				else
					printf("@Load factor=%.4lf : Throughput = %.2lf,  AVG throughput = %.2lf,  current FPR = %.8lf\n",
							1.0*insert_num/insert_tot, (i-last_i)/(results[it].t-last_time), i/results[it].t, (double)results[it].fp/(query_num - last_query_num));
				last_i = i;
				last_time = results[it].t;
				last_insert_num = insert_num;
				last_query_num = query_num;
				it++;
			}
			if (it==results.size()) break;
		}
		if (query_num>0) {
			printf("fpr = %.8lf\n", 1.0*false_positive/query_num);
			if (false_negative>0)
				printf("!!!!!!!!!!!!!!! false negative = %d\n", false_negative);
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
	int seg = 20;
	for(int i=0; i<seg; i++){
		int j=(long long)n*i/seg, k=(long long)n*(i+1)/seg;
		for(int l=j;l<k;++l)
			ret.push_back(Operation(0, gen_string_by_int(a[l]), 0, l==k-1));
		if(!no_query) for(int l=j;l<k;++l)
			if(rand()&1)
				ret.push_back(Operation(1, gen_string_by_int(a[rand()%k]), true, l==k-1));
			else
				ret.push_back(Operation(1, gen_string_by_int(a[k+rand()%(n-k+1)]), false, l==k-1));
	}
	return ret;
}