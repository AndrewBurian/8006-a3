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
  while(std::getline(actionsFile, action)){
    _actions.push_back(action);
  }

  // close the file
  actionsFile.close();

}

void Action::act(std::map<std::string, std::string> &replacements){

  std::cout << "Taking action " << _name << std::endl;

  // for each action to take
  for(std::string act : _actions){
    // range based for loop gets copies

    // for each replacement to make
    for(std::map<std::string, std::string>::const_iterator it = replacements.begin();
      it != replacements.end(); ++it){

      // find the position of the target string, replace it with the replacement
      act.replace(act.find("<" + it->first + ">"), it->first.length(), it->second);

    }

    // all replacements made, envoke action
    system(act.c_str());

  }
}

std::string Action::get_name(){
  return _name;
}
