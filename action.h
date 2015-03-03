#ifndef ACTION_H
#define ACTION_H

#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include <string>
#include <map>

#define PATH "./action.d/"
#define TYPE ".act"

class Action{

private:
  std::string _name;
  std::list<std::string> _actions;

public:
  Action(std::string &name);
  void act(std::map<std::string, std::string> &replacements);
  std::string get_name();
};


#endif
