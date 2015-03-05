#include "action.h"

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

  // find the acts header
  while(std::getline(actionsFile, action)){
    if(action == "[act]"){
      break;
    }
  }

  // read acts
  while(std::getline(actionsFile, action)){
    // skip comments and blanks
    if(action.front() == '#' || action.front() == '\n'){
      continue;
    }
    // if we're into unacts
    if(action == "[unact]"){
      break;
    }
    _actions.push_back(action);
  }

  // alread read unact
  do{
    // skip comments and blanks
    if(action.front() == '#' || action.front() == '\n'){
      continue;
    }
    _unacts.push_back(action);
  }while(std::getline(actionsFile, action));

  // close the file
  actionsFile.close();

}

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

    // start the unacts
    _threads.push_back(new std::thread(&Action::unact, this, replacements));

  }
}

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
