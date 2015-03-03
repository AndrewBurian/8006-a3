#ifndef FILTER_H
#define FILTER_H

#include <fstream>
#include <regex>
#include <stdio.h>
#include <string>
#include <vector>

class Action;

class Filter {
private:
	
	std::ifstream log_file;
	std::string filter_name;
	std::string log_file_name;
	std::vector<Action*> actions;
	std::vector<std::regex> filters;
	
	void load_filter();
	
public:
	
	Filter(std::string name, std::string logFile, int period, int attempts);
	~Filter();
	void run();
	void add_action(Action &action);
	std::string get_name();
	
};

#endif
