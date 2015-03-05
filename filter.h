/* ----------------------------------------------------------------------------
HEADER FILE

Name:		filter.h

Program:	banhammer

Developer:	Jordan Marling

Created On:	2015-03-01

Description:
	This is the definition of the Filter class

Revisions:

---------------------------------------------------------------------------- */

#ifndef FILTER_H
#define FILTER_H

#include <fstream>
#include <regex.h>
#include <stdio.h>
#include <string>
#include <vector>

#define FILTER_PATH "./filter.d/"
#define FILTER_TYPE ".fil"

class Action;

class Filter {
private:

	struct filter_data {
		regex_t expression;
		std::vector<std::string> keywords;
	};

	std::ifstream log_file;
	std::string filter_name;
	std::string log_file_name;
	std::vector<Action*> actions;
	std::vector<struct filter_data> filters;

	void load_filter();
	void check_log_line(std::string line);

public:

	Filter(std::string name, std::string logFile, int period, int attempts);
	~Filter();
	void run();
	void add_action(Action &action);
	std::string get_name();

};

#endif
