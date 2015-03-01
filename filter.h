#ifndef FILTER_H
#define FILTER_H

#include <string>

class Action;

class Filter {
public:
	
	Filter(std::string &name, std::string &logFilePath);
	void run();
	void add_action(Action &action);
	
};

#endif
