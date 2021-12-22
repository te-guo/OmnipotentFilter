#include "evaluation.h"

char* DataGenerator::gen_string_by_int(long long x) {
    char *s = new char [19];
    for (int i=0; i<18; i++) {
        s[i] = '0'+x%10;
        x /= 10;
    }
    s[18] = 0;
    return s;
}

void DataGenerator::gen_random_data(Data &data, int n, bool no_query, bool no_remove) {
    cerr<<"Generating random data n = " << n ;
    if (no_query) cerr << " no query";
    cerr << " ... ";
    data.clear();
    int m = n*1.1;
    long long *a = new long long[m+1];
    long long mod = 1000000000000000003ll, x = mod-1;
    bool *alive = new bool [m+1];
    for (int i=0; i<=m; i++) {
        alive[i] = true;
        a[i] = x;
        x = 1ll*x*3%mod;
    }
    int seg = 20;
    vector<int> to_remove;
    for(int i=0; i<seg; i++){
        int j=(long long)n*i/seg, k=(long long)n*(i+1)/seg;
        for(int l=j; l<k; ++l)
            data.push_back(Operation(0, a[l], 0, l==k-1));
        if(!no_query) {
            for(int l=j;l<k;++l) {
                if(rand()&1)
                    data.push_back(Operation(1, a[rand()%k], true, l==k-1));
                else
                    data.push_back(Operation(1, a[k+rand()%(m-k+1)], false, l==k-1));
            }
        }
        if(!no_remove){
            to_remove.resize(k-j);
            for(int l=0; l < k-j; l++){
                int x = i ? rand()%k : l;
                while(!alive[x])x=rand()%k;
                alive[x] = false;
                to_remove[l] = x;
                data.push_back(Operation(2, a[x], 0, l == k-j-1));
            }
            random_shuffle(to_remove.begin(),to_remove.end());
            for(int l=0; l<to_remove.size(); ++l){
                data.push_back(Operation(0, a[to_remove[l]], 0, l==to_remove.size()-1));
                alive[to_remove[l]] = true;
            }
        }
    }
    cerr << "generated " << data.size() << " operations." << endl;
    delete [] a;
    delete [] alive;
}



void EvaluationBase::open_log() {
    /*
    std::filesystem::path path = log_dir;
    if (!std::filesystem::exists(path)) {
        cerr << "Creating directories " << path << endl;
        std::filesystem::create_directories(path);
    }*/
    cerr<< "Saving log to " << log_path << endl;
    assert(freopen(log_path.c_str(), "a", stdout));
}

void EvaluationBase::_precompute_data() {
    op_tot[0] = op_tot[1] = op_tot[2] = 0;
    int query_ans_cnt[2] = {};
    for (auto o : data) {
        op_tot[o.type]++;
        if(o.type==1) {
            assert(o.ans==0||o.ans==1);
            query_ans_cnt[o.ans]++;
        }
    }
    puts("======================================================");
    printf("Evaluating with data: #insert = %d, #query = %d, #remove = %d", op_tot[0], op_tot[1], op_tot[2]);
    if (op_tot[1]>0) {
        printf(", #yes/#query = %.3lf", 1.0*query_ans_cnt[0]/op_tot[1]);
    }
    puts("");
}

void EvaluationBase::_evaluation(string eval_name) {
    init();
    puts("Procedure:");
    assert(max_capacity > 100);

    int false_positive = 0, point_false_positive = 0;
    int false_negative = 0;
    bool fail = false;

    Timer T;
    for (int i=0; i<data.size(); i++) {
        if (data[i].type==0) {
            bool ok = insert(data[i].key);
            if (!ok)
                fail = true, data[i].checkpoint = true;
        } else if(data[i].type==1){
            bool ok = query(data[i].key) == data[i].ans;
            if (!ok)
                if(data[i].ans==false) point_false_positive++;
                else false_negative++;
        } else{
            bool ok = remove(data[i].key);
            if (!ok)
                fail = true, data[i].checkpoint = true;
        }
        if (data[i].checkpoint) {
            checkpoints.push_back((Status){i, point_false_positive, T.get(), actual_size()});
            false_positive += point_false_positive;
            point_false_positive = 0;
            debug();
            if (fail) break;
        }
    }
    if (op_tot[1]>0 && false_negative>0) {
        cout << "[!!!WARNING!!!] False negative = " << false_negative << endl;
        cerr << "[!!!WARNING!!!] False negative = " << false_negative << endl;  
    }
}

void EvaluationBase::_print_results() {
    int it = 0;
    int op_num[3] = {};
    int last_i = 0, last_op_num[3] = {};
    double last_time = 0;
    
    map<string, double> tot_t;
    map<string, int> tot_num;
    
    for (int i=0; i<data.size(); i++) {
        op_num[data[i].type]++;
        if (it<checkpoints.size() && i==checkpoints[it].id) {
            string type;
            if(op_num[1] == last_op_num[1] && op_num[2] == last_op_num[2]) type = "Insert";
            else if(op_num[0] == last_op_num[0] && op_num[2] == last_op_num[2]) type = "Query";
            else if(op_num[0] == last_op_num[0] && op_num[1] == last_op_num[1]) type = "Remove";
            else type = "Mixed";
            printf("%s ", type.c_str());

            int cur_num = i-last_i;
            double cur_t = checkpoints[it].t-last_time;
            double cur_tp = cur_num / cur_t;
            long key_num = op_num[0] - op_num[2];
            long last_key_num = last_op_num[0] - last_op_num[2];
            double lf = 1.0 * max(key_num, last_key_num) / max_capacity;
            tot_t[type] += cur_t;
            tot_num[type] += cur_num;
            double avg_tp = tot_num[type] / tot_t[type];
            double bpk = key_num != 0 ? (double)checkpoints[it].size * 8 / key_num : 0;

            if(type != string("Query"))
                printf("Load_factor = %.4lf,  Throughput = %.2lf,  AVG_throughput = %.2lf, BPK = %.2lf\n",
                        lf, cur_tp, avg_tp, bpk);
            else
                printf("Load_factor = %.4lf,  Throughput = %.2lf,  AVG_throughput = %.2lf, BPK = %.2lf"
                ", FPR = %.8lf\n",
                        lf, cur_tp, avg_tp, bpk, (double)checkpoints[it].fp/(op_num[1] - last_op_num[1]));
            
            last_i = i;
            last_time = checkpoints[it].t;
            copy_n(op_num, 3, last_op_num);
            it++;
        }
        if (it==checkpoints.size()) break;
    }	
}


string EvaluationBase::get_filter_name() {
    assert(false);
}
void EvaluationBase::init() {
    assert(false);
}
bool EvaluationBase::insert(uint64_t key) {
    assert(false);
}
bool EvaluationBase::query(uint64_t key) {
    assert(false);
}
bool EvaluationBase::remove(uint64_t key) {
    assert(false);
}
size_t EvaluationBase::actual_size() {
    assert(false);
}
void EvaluationBase::debug() {
}

// argv will overwrite the arguments assigned by eval_config
std::map<std::string, std::string> load_config(string config_path) {
    std::map<std::string, std::string> arguments;
    ifstream in(config_path);
    assert(in.is_open());
    std::string argu, value;
    while(in >> argu >> value)
        arguments[argu] = value;
    in.close();
    return arguments;
}
void EvaluationBase::_generate_input(){
    max_capacity = 1 << argu_int("n");
    if (get_filter_name()=="Omnipotent") {
        max_capacity += max_capacity / 2;
    }
    data_generator.gen_random_data(data, max_capacity, argu_int("no_query"), argu_int("no_remove"));
}

void EvaluationBase::evaluation(int argc, char* argv[], string path) {
    arguments = load_config();
    for(int i = 2; i < argc; i += 2)
        arguments[std::string(argv[i-1] + 1)] = argv[i];
    if(!arguments.count("name"))
        arguments["name"] = get_time_str();

    log_dir = path + "/";
    log_path = log_dir + arguments["name"]
        + "_" + get_filter_name()
        + (arguments.count("round_id") ? "#" + arguments["round_id"] : "")
        + ".txt";
    open_log();

    cout << get_filter_name() << endl;
    cout << "Evaluating [" << get_filter_name() << "] in evaluation [" << arguments["name"] << "]" << endl;
    cerr << "Evaluating [" << get_filter_name() << "] in evaluation [" << arguments["name"] << "]" << endl;

    _generate_input();
    _precompute_data();
    _evaluation(arguments["name"]);
    _print_results();
}

EvaluationBase::~EvaluationBase() {
}

string get_time_str(){
    time_t time_now = time(0);
    string time_str = (ctime(&time_now));
    if(time_str[8] == ' ')
        time_str[8] = '0';
    for (int i=0; i<time_str.size(); i++) {
        if (time_str[i]==' ')
            time_str[i] = '-';
        if (time_str[i]==':')
            time_str[i] = '-';
    }
	return string(time_str, 20, 4) + '-' + string(time_str, 4, 15);
}