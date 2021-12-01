#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include "evaluation.h"
using namespace std;
typedef long long ll;

string get_time_str(){
	time_t time_now = time(0);
	string time_str = (ctime(&time_now));
	for (int i=0; i<time_str.size(); i++) {
		if (time_str[i]==' ')
			time_str[i] = '+';
		if (time_str[i]==':')
			time_str[i] = '_';
	}
	while (time_str.back()<32) time_str.pop_back();
	return time_str;
}

int main(int argc, char* argv[]) {
	vector<string> folders = {/*"Omnipotent", */"VacuumFilter", "VQF", "LDCF"};
	string eval_name = argc >= 2 ? string(argv[1]) : get_time_str();
	string options = " " + eval_name;
	
	{
		string cmd = "";
		cmd += "cd ..";
		cmd += " && ";
		cmd += "mkdir log";
		system(cmd.c_str());
	}
	for (auto f : folders) {	
		string cmd = "";
		cmd += "cd ..";
		cmd += " && ";
		cmd += "cd " + f;
		cmd += " && ";
		//cmd += "make clean";
		//cmd += " && ";
		cmd += "make";
		cmd += " && ";
		cmd += "./test" + options;
		cmd += " && ";
		cmd += "cd ..";
		system(cmd.c_str());
	}
	{
		string cmd = "";
		cmd += "python3 plot.py";
		cmd += " " + eval_name;
		for (auto f : folders)
			cmd += " ../log/" + eval_name + "\\ " + f + ".txt";
		system(cmd.c_str());
	}
	return 0;
}