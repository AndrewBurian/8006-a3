#ifndef ACTION_H
#define ACTION_H

#include <string>

class Action{

private:
  std::string _name;

public:
  Action(std::string &name);
  void act(std::string &hostname);
  std::string get_name();
};


#endif
