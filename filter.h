#ifndef FILTER_H
#define FILTER_H

class Action;

class Filter {
public:
	
	Filter(std::string& name, std::string& logFilePath);
	void run();
	void register(Action& action);
	
};

#endif
