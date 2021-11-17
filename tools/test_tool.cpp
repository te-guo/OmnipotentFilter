#include<iostream>
#include<cstdio>
#include<chrono>
#include<vector>
#include "evaluation.h"
using namespace std;
typedef long long ll;
int main(int argc, char* argv[]) {
	vector<string> folders = {"Omnipotent", "VacuumFilter", "VQF", "LDCF"};
	string options = "";
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
		cmd += "make clean";
		cmd += " && ";
		cmd += "make";
		cmd += " && ";
		cmd += "./test" + options;
		cmd += " && ";
		cmd += "cd ..";
		system(cmd.c_str());
	}
	return 0;
}