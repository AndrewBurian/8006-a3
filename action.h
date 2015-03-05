#ifndef ACTION_H
#define ACTION_H

#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include <string>
#include <map>

#define ACTION_PATH "./action.d/"
#define ACTION_TYPE ".act"

class Action{

private:
  std::string _name;
  int _banTime;
  std::list<std::string> _actions;

public:
  Action(std::string &name, int banTime);
  void act(std::map<std::string, std::string> &replacements);
  std::string get_name();
};


#endif
