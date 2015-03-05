/* ----------------------------------------------------------------------------
SOURCE FILE

Name:		main.cpp

Program:	BanHammer

Developer:	Andrew Burian

Created On:	2015-03-04

Functions:
	int main(int argc, char** argv)

Description:
	Starting point of the BanHammer program.
  Reads conf and starts up

Revisions:
	(none)

---------------------------------------------------------------------------- */

#include <stdlib.h>
#include <vector>
#include <sstream>
#include <thread>
#include "filter.h"
#include "action.h"


/* ----------------------------------------------------------------------------
FUNCTION

Name:		Main

Prototype:	int main(int argc, char** argv)

Developer:	Andrew Burian

Created On:	2015-03-04

Parameters:
	command ling args (ignored)

Return Values:
	0

Description:
	Entry point for BanHammer.
  Reads the banhammer.conf file, creates filter and action objects as nessesary
  and links them together. Then gives each filter object a thread and awaits
  their completion.

Revisions:
	(none)

---------------------------------------------------------------------------- */
int main(int argc, char** argv){

  // read in main config file
  std::ifstream config("banhammer.conf", std::ios_base::in);

  std::vector<Filter*> filters;
  std::vector<Action*> actions;
  std::vector<std::thread*> threads;

  Filter* newFilter = 0;
  Action* newAction = 0;

  std::string line;

  std::string filterName;
  bool filterNameReady = false;

  std::vector<std::string> actionNames;
  bool actionNamesReady = false;

  std::string logFile;
  bool logFileReady = false;

  int period = 0;
  bool periodReady = false;

  int banTime = 0;
  bool banTimeReady = false;

  int attempts = 0;
  bool attemptsReady = false;

  std::string ruleName;
  bool first = true;

  // READ LOGIC
  while(std::getline(config, line)){

    // check for comment or blank line
    if(line.length() <= 1 || line.front() == '#'){
      continue;
    }

    // do all the checks

    // handle filter setting
    if(line.find("filter") != std::string::npos){
      filterName = line.substr(line.find('=') + 1);
      if(filterName.length() > 0){
        filterNameReady = true;
      }
      continue;
    }

    // handle logfile setting
    if(line.find("logfile") != std::string::npos){
      logFile = line.substr(line.find('=') + 1);
      if(logFile.length() > 0){
        logFileReady = true;
      }
      continue;
    }

    // handle action setting
    if(line.find("action") != std::string::npos){
      std::string actionName = line.substr(line.find('=') + 1);
      if(actionName.length() > 0){
        actionNames.push_back(actionName);
        actionNamesReady = true;
      }
      continue;
    }

    // handle period setting
    if(line.find("period") != std::string::npos){
      std::istringstream tmp(line.substr(line.find('=') + 1));
      if(tmp >> period){
        periodReady = true;
      }
      continue;
    }

    // handle attempts setting
    if(line.find("attempts") != std::string::npos){
      std::istringstream tmp(line.substr(line.find('=') + 1));
      if(tmp >> attempts){
        attemptsReady = true;
      }
      continue;
    }

    // handle bantime setting
    if(line.find("bantime") != std::string::npos){
      std::istringstream tmp(line.substr(line.find('=') + 1));
      if(tmp >> banTime){
        banTimeReady = true;
      }
      continue;
    }

    // handle end of file or next rule
    if(line.front() == '[' && line.back() == ']'){

      // if this is the first rule encountered, just save the name
      if(first){
        ruleName = line.substr(1, line.size() - 2);
        first = false;
        continue;
      }

      // otherwise, the previous rule declaration is done.
      // build the current rule

      // check to see all(); required params are set
      if(!(filterNameReady && logFileReady && actionNamesReady
        && periodReady && attemptsReady && banTimeReady)){

        // the rule had missing elements
        std::cerr << "Failed to initialize rule: " << ruleName << std::endl;

        // set all readys to false
        filterNameReady = logFileReady = actionNamesReady = periodReady = attemptsReady = banTimeReady = false;

        // empty the actions vector
        actionNames.erase(actionNames.begin(), actionNames.end());
        continue;
      }

      // Rule ready to go
      // initialize

      // first, check to see if the filter object exists already
      newFilter = 0;
      bool isNew = false;
      for(size_t i = 0; i < filters.size(); ++i){
        if((filters[i])->get_name() == filterName){
          newFilter = filters[i];
          break;
        }
      }

      // filter object does not yet exist
      if(newFilter == 0){
        newFilter = new Filter(filterName, logFile, period, attempts);
        filters.push_back(newFilter);
        // flag this as needing a thread
        isNew = true;
      }


      // then add the actions to the new filter
      for(size_t i = 0; i < actionNames.size(); ++i){

        // then, check to see if the action object exists already
        newAction = 0;
        for(size_t j = 0; i < actions.size(); ++i){
          if(actions[j]->get_name() == actionNames[i]){
            newAction = actions[j];
            break;
          }
        }

        // object does not yet exist
        if(newAction == 0){
          newAction = new Action(actionNames[i], banTime);
          actions.push_back(newAction);
        }

        // register it with the filter
        newFilter->add_action(*newAction);

      }

      // if it needs a thread, provide one
      if(isNew){
        std::thread* thread = new std::thread(&Filter::run, newFilter);
        threads.push_back(thread);
      }

      // filter created and actions added
      std::cout << "Initialize rule: " << ruleName << std::endl;

      // set all readys to false
      filterNameReady = logFileReady = actionNamesReady = periodReady = attemptsReady = banTimeReady = false;

      // empty the actions vector
      actionNames.erase(actionNames.begin(), actionNames.end());
    }

    // end of conf file reading

  }

  // wait on threads
  for(std::thread* thr : threads){
    thr->join();
  }

  return 0;
}
