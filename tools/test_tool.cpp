#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include<map>
#include<string>
#include "evaluation.h"
using namespace std;
typedef long long ll;

int main(int argc, char* argv[]) {
	vector<string> folders = {"Omnipotent", "VacuumFilter", "VQF", "LDCF", "Morton"};
	string eval_name = argc >= 2 ? string(argv[1]) : get_time_str();
	std::map<std::string, std::string> arguments = load_config();
	string options = " -name " + eval_name;
	
	{
		string cmd = "";
		cmd += "cd ..";
		cmd += " && ";
		cmd += "mkdir log";
		system(cmd.c_str());
	}
	vector<string> make_updates;
	for (auto f : folders) {	
		string cmd = "";
		cmd += "cd .. && cd " + f;
		int ret = system((cmd + " && make -q").c_str());
		assert(ret == 256 || ret == 0);
		if (ret == 256) make_updates.push_back(f);
		assert(system((cmd + " && make").c_str()) == 0);
	}

	for (auto f : folders) for(int i = 0; i < argu_int("round"); i++){
		string cmd = "";
		cmd += "cd .. && cd " + f;
		assert(system((cmd + " && ./test" + options + " -round " + std::to_string(i)).c_str()) == 0);
	}
	{
		string cmd = "python3 plot.py";
		cmd += " 0";
		cmd += " " + eval_name;
		cmd += " " + arguments["round"];
		for (auto f : folders)
			cmd += " " + f;
		system(cmd.c_str());
	}

	for (auto f : make_updates) {
		cerr << "[" << f << "] has updated files!" << endl;
	}
	if (make_updates.size()==0) {
		cerr << "No updated files detected!" << endl;
	}
	return 0;
}