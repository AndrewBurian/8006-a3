#ifndef ACTION_H
#define ACTION_H

#include <string>

class Action{

private:

public:
  Action(std::string &name);
  void act(std::string &hostname);
};


#endif
