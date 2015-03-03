#include "filter.h"

#include <fstream>
#include <iostream>
#include <limits.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <unistd.h>

#define INOTIFY_BUFFER_LEN (sizeof(struct inotify_event) + NAME_MAX + 1)

Filter::Filter(std::string name, std::string logFile, int period, int attempts) : filter_name(name), log_file_name(logFile) {
	log_file.open(log_file_name.c_str());
}

Filter::~Filter() {
	log_file.close();
}

void Filter::run() {
	
	int notify_descriptor;
	int file_descriptor;
	int length;
	
	char event_buffer[INOTIFY_BUFFER_LEN];
	//inotify_event *event;
	
	std::string line;
	
	
	notify_descriptor = inotify_init();
	if (notify_descriptor < 0) {
		perror("inotify_init");
	}
	
	file_descriptor = inotify_add_watch(notify_descriptor, log_file_name.c_str(), IN_MODIFY);
	if (file_descriptor < 0) {
		perror("inotify_add_watch");
		return;
	}
	
	
	
	while (1) {
		
		while (std::getline(log_file, line)) {
			std::cout << "Line: " << line << std::endl;
		}
		
		if (log_file.eof()) {
			log_file.clear();
		}
		
		length = read(notify_descriptor, event_buffer, INOTIFY_BUFFER_LEN);
		if (length < 0) {
			perror("read");
		}
		
		
		
	}
	
}

void Filter::add_action(Action &action) {
	actions.push_back(&action);
}

std::string Filter::get_name() {
	return filter_name;
}

void Filter::load_filter() {
	
	std::string filter_file_name("filters/" + filter_name + ".filter");
	
	std::ifstream filter_file;
	
	filter_file.open(filter_file_name);
	
	
	
	
	
	
	filter_file.close();
	
}
