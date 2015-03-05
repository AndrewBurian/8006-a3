/* ----------------------------------------------------------------------------
SOURCE FILE

Name:		action.cpp

Program:	BanHammer

Developer:	Andrew Burian

Created On:	2015-03-04

Functions:
	Action::Action(std::string &name, int banTime)
  void Action::act(std::map<std::string, std::string> &replacements)
  void Action::unact(std::map<std::string, std::string> replacements)

Description:
	The implementation of the Action class

Revisions:
	(none)

---------------------------------------------------------------------------- */

#include "action.h"

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Action

Prototype:	Action::Action(std::string &name, int banTime)

Developer:	Andrew Burian

Created On:	2015-03-04

Parameters:
	std::string &name
    the name of this filter, corresponds to its .act file
  int banTime
    the delay between act and unact actions

Return Values:
	Action
    New action object
  Throws:
    Exception on failure to find .act file

Description:
	Creates an action object, executes any [before] directives, and loads the
  [act] and [unact] directives for later

Revisions:
	(none)

---------------------------------------------------------------------------- */
Action::Action(std::string &name, int banTime) : _name(name), _banTime(banTime) {

  // open the file containing the actions
  std::fstream actionsFile(ACTION_PATH + _name + ACTION_TYPE, std::ios_base::in);

  // check to see if the file failed to open, and throw an exception
  if(!actionsFile){
    std::cerr << "Failed to open actions file " << ACTION_PATH << _name << ACTION_TYPE << std::endl;
    // fatal
    throw "Could not create Action";
  }

  // read in the actions and add them
  std::string action;

  // find the before header
  while(std::getline(actionsFile, action)){
    if(action == "[before]"){
      break;
    }
  }

  // read befores
  while(std::getline(actionsFile, action)){
    // skip comments and blanks
    if(action.front() == '#' || action.front() == '\n'){
      continue;
    }
    // if we're into acts
    if(action == "[act]"){
      break;
    }
    // execute befores right away
    system(action.c_str());
  }


  // alread read act
  do{
    // skip comments and blanks
    if(action.front() == '#' || action.front() == '\n'){
      continue;
    }
    // skip act header
    if(action == "[act]"){
      continue;
    }
    // if we're into unacts
    if(action == "[unact]"){
      break;
    }
    _actions.push_back(action);
  }while(std::getline(actionsFile, action));


  // alread read unact
  do{
    // skip comments and blanks
    if(action.front() == '#' || action.front() == '\n'){
      continue;
    }
    // skip unact header
    if(action == "[unact]"){
      continue;
    }
    _unacts.push_back(action);
  }while(std::getline(actionsFile, action));

  // close the file
  actionsFile.close();

}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Act

Prototype:	void Action::act(std::map<std::string, std::string> &replacements)

Developer:	Andrew Burian

Created On:	2015-03-04

Parameters:
	std::map<std::string, std::string> &replacements
    key-value pairs that are to be replaced in the act directives

Description:
	Executes all act directives after performing key-value replacements

Revisions:
	(none)

---------------------------------------------------------------------------- */
void Action::act(std::map<std::string, std::string> &replacements){

  // for each action to take
  for(std::string act : _actions){
    // range based for loop gets copies

    // for each replacement to make
    for(std::map<std::string, std::string>::const_iterator it = replacements.begin();
      it != replacements.end(); ++it){

      // find the position of the target string, replace it with the replacement
      size_t pos = act.find("<" + it->first + ">");
      if(pos != std::string::npos){
        act.replace(pos, it->first.length() + 2, it->second);
      }

    }

    // all replacements made, envoke action
    system(act.c_str());

  }
  if (_banTime >= 0) {
    // start the unacts
    _threads.push_back(new std::thread(&Action::unact, this, replacements));
  }
}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		UnAct

Prototype:	void Action::unact(std::map<std::string, std::string> replacements)

Developer:	Andrew Burian

Created On:	2015-03-04

Parameters:
	std::map<std::string, std::string> &replacements
    key-value pairs that are to be replaced in the act directives

Description:
	Delays for the action's given time, then executes all unact directives
  performing the same key-value replacements as the original act

Revisions:
	(none)

---------------------------------------------------------------------------- */
void Action::unact(std::map<std::string, std::string> replacements){

  sleep(_banTime);

  // for each action to take
  for(std::string act : _unacts){
    // range based for loop gets copies

    // for each replacement to make
    for(std::map<std::string, std::string>::const_iterator it = replacements.begin();
      it != replacements.end(); ++it){

      // find the position of the target string, replace it with the replacement
      size_t pos = act.find("<" + it->first + ">");
      if(pos != std::string::npos){
        act.replace(pos, it->first.length() + 2, it->second);
      }

    }

    // all replacements made, envoke action
    system(act.c_str());

  }

}

std::string Action::get_name(){
  return _name;
}
