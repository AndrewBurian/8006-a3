/* ----------------------------------------------------------------------------
SOURCE FILE

Name:		filter.c

Program:	banhammer

Developer:	Jordan Marling

Created On:	2015-03-01

Functions:
	Filter::Filter(std::string name, std::string logFile, int period, int attempts)
	Filter::~Filter()
	void Filter::run()
	void Filter::add_action(Action &action)
	std::string Filter::get_name()
	void Filter::load_filter()
	void Filter::check_log_line(std::string line)

Description:
	This file has all the functions for the Filter class. The filter class
	listens to a log file and checks each new line against regex filters.
	If a filter is triggered, all of the registered actions are then
	notified.

Revisions:

---------------------------------------------------------------------------- */

#include "filter.h"

#include "action.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <map>
#include <regex.h>
#include <sstream>
#include <stdio.h>
#include <sys/inotify.h>
#include <time.h>
#include <unistd.h>

#define INOTIFY_BUFFER_LEN 		(sizeof(struct inotify_event) + NAME_MAX + 1)
#define REGEX_MAX_MATCHES		100

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Filter constructor

Prototype:	Filter::Filter(std::string name, std::string logFile, int period, int attempts)

Developer:	Jordan Marling

Created On:	2015-03-01

Parameters:
	std::string name: The name of the filter, should be the same as the file in "filter/NAME.filter"
	std::string logFile: the log file to listen to.
	int period: the period in which the attempts are allowed
	int attempts: the amount of attempts allows in the period of time.

Return Values:
	none

Description:
	This is the constructor for the Filter class. It opens the log file and
	loads the filters into memory.

Revisions:

---------------------------------------------------------------------------- */
Filter::Filter(std::string name, std::string logFile, int _period, int _attempts) : filter_name(name), log_file_name(logFile), datetime_expression(0), period(_period), attempts(_attempts) {
	log_file.open(log_file_name.c_str());

	load_filter();
}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Filter deconstructor

Prototype:	Filter::~Filter()

Developer:	Jordan Marling

Created On:	2015-03-01

Parameters:
	none

Return Values:
	none

Description:
	Closes the log file when the object is destroyed.

Revisions:

---------------------------------------------------------------------------- */
Filter::~Filter() {
	log_file.close();
}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Run

Prototype:	void Filter::run()

Developer:	Jordan Marling

Created On:	2015-03-01

Parameters:
	none

Return Values:
	void

Description:
	This function is run in a separate thread where it can listen to
	log files and checks the new lines with the loaded filters.

Revisions:

---------------------------------------------------------------------------- */
void Filter::run() {

	int notify_descriptor;
	int file_descriptor;
	int length;
	char event_buffer[INOTIFY_BUFFER_LEN];
	std::string line;


	//init inotify
	notify_descriptor = inotify_init();
	if (notify_descriptor < 0) {
		perror("inotify_init");
	}


	//add file to inotify watch
	file_descriptor = inotify_add_watch(notify_descriptor, log_file_name.c_str(), IN_MODIFY);
	if (file_descriptor < 0) {
		perror("inotify_add_watch");
		return;
	}


	//loop infinitely for log file updates
	while (1) {

		//read all of the lines in the file
		while (std::getline(log_file, line)) {

			check_log_line(line);

		}

		//if we reach the end of the file, reset the EOF bit
		if (log_file.eof()) {
			log_file.clear();
		}

		//block until new data in file.
		length = read(notify_descriptor, event_buffer, INOTIFY_BUFFER_LEN);
		if (length < 0) {
			perror("read");
			break;
		}

	}

}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Add Action

Prototype:	void Filter::add_action(Action &action)

Developer:	Jordan Marling

Created On:	2015-03-01

Parameters:
	Action &action: The action object to add to the filter

Return Values:
	void

Description:
	This adds an action class to the filter. The action class will then
	be called when the filter is triggered.

Revisions:

---------------------------------------------------------------------------- */
void Filter::add_action(Action &action) {
	actions.push_back(&action);
}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Get Name

Prototype:	std::string Filter::get_name()

Developer:	Jordan Marling

Created On:	2015-03-01

Parameters:
	none

Return Values:
	The name of the filter

Description:
	This returns the name of the filter.

Revisions:

---------------------------------------------------------------------------- */
std::string Filter::get_name() {
	return filter_name;
}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Load Filter

Prototype:	void Filter::load_filter()

Developer:	Jordan Marling

Created On:	2015-03-02

Parameters:
	none

Return Values:
	void

Description:
	This function loads the filter file and compiles all the regex and
	keyword lists.

Revisions:

---------------------------------------------------------------------------- */
void Filter::load_filter() {

	std::string filter_file_name(FILTER_PATH + filter_name + FILTER_TYPE);
	std::string line;
	std::string section;
	std::ifstream filter_file;

	regex_t tmp_regex;
	filter_data *data;

	//open the file
	filter_file.open(filter_file_name);

	//read each line, which should be regex.
	while (std::getline(filter_file, line)) {

		//don't do anything with empty lines or commented lines
		if (line.size() == 0 || line[0] == '#') {
			continue;
		}

		//check to see if there is a section change
		if (line[0] == '[' && line.back() == ']') {

			//get the text inside the [ and ]
			section = line.substr(1, line.size() - 2);

			//set to lowercase
			std::transform(section.begin(), section.end(), section.begin(), ::tolower);

			continue;
		}

		if (section == "params") {

			//find the equals (=) sign for the parameter
			int delimiter_pos = line.find("=", 0);

			std::string key = line.substr(0, delimiter_pos);
			std::string value = line.substr(delimiter_pos + 1);

			//set to lowercase
			std::transform(key.begin(), key.end(), key.begin(), ::tolower);

			//add parameter to map
			filter_parameters[key] = value;

			//check for special parameters
			if (key == "datetime_regex") {
				datetime_expression = (regex_t*)malloc(sizeof(regex_t));
				if (regcomp(datetime_expression, value.c_str(), REG_EXTENDED) != 0) {
					std::cout << "Error in datetime_regex: " << value << std::endl;
				}
			}
		}
		else if (section == "regex") {

			//compile into temporary regex struct
			if (regcomp(&tmp_regex, line.c_str(), REG_EXTENDED) != 0) {
				std::cout << "Error in regex: " << line << std::endl;
			}
			else {

				//add one to the end, and store it.
				filters.emplace_back();
				data = &filters.back();

				//store regex expression
				data->expression = tmp_regex;

				//read next line for regex group names. separated by comma (,)
				if (std::getline(filter_file, line) != 0) {

					std::stringstream ss(line);
					std::string token;

					//store keywords
					while (std::getline(ss, token, ',')) {
						data->keywords.push_back(token);
					}
				}

			}

		}




	}


	filter_file.close();

}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Check Log Line

Prototype:	void Filter::check_log_line(std::string line)

Developer:	Jordan Marling

Created On:	2015-03-02

Parameters:
	std::string line: the string to check against all regex filters

Return Values:
	void

Description:
	This function checks the string line against all filters loaded in
	this class. If there is a match, it sends it to the all registered
	action objects.

Revisions:

---------------------------------------------------------------------------- */
void Filter::check_log_line(std::string line) {

	int nomatch;
	regmatch_t matches[REGEX_MAX_MATCHES];
	int i;
	filter_data *data;
	std::map<std::string, std::string> keywords;
	struct tm time;
	time_t epoch = 0;

	//check to see if the date time is added to the parameters
	if (filter_parameters.find("datetime_regex") != filter_parameters.end() && filter_parameters.find("datetime_format") != filter_parameters.end()) {

		nomatch = regexec(datetime_expression, line.c_str(), REGEX_MAX_MATCHES, matches, 0);
		if (nomatch == 0) {

			//because there is only 1 group, the index is 1.
			//index 0 is the whole string

			//get the datetime from regex
			std::string datetime = line.substr(matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);

			//turn the regex into a struct tm
			strptime(datetime.c_str(), filter_parameters["datetime_format"].c_str(), &time);

			//get the seconds since epoch to do match on
			epoch = mktime(&time);

		}

	}



	//check line against all filters
	for(auto it = filters.begin(); it != filters.end(); ++it) {

		data = &(*it);
		keywords.clear();

		//check if there are matches with the filter
		nomatch = regexec(&data->expression, line.c_str(), REGEX_MAX_MATCHES, matches, 0);
		if (nomatch != 0) {
			continue;
		}

		//go through all matches and fill out keyword map
		for(i = 1; i < REGEX_MAX_MATCHES; i++) {

			//check if there is another match
			if (matches[i].rm_so == -1) {
				break;
			}

			keywords[data->keywords[i - 1]] = line.substr(matches[i].rm_so, matches[i].rm_eo - matches[i].rm_so);

		}

		//set event
		if (epoch != 0 && keywords.find("hostname") != keywords.end()) {

			std::vector<long>& hostname_attempts = attempts_by_hostname[keywords["hostname"]];

			//put the new log line into vector
			hostname_attempts.push_back(epoch);

			//sanitize the vector by removing old log lines
			for(i = hostname_attempts.size() - 1; i >= 0; --i) {

				//check if the time is older than the period we are checking
				if (hostname_attempts[i] - epoch > period) {
					hostname_attempts.erase(hostname_attempts.begin() + i);
				}

			}

			//check if there are too many attempts
			if ((int)hostname_attempts.size() >= attempts) {

				//notify all actions
				for(auto action_it = actions.begin(); action_it != actions.end(); ++action_it) {
					(*action_it)->act(keywords);
				}

			}

		}


	}
}
