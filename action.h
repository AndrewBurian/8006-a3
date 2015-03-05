#ifndef ACTION_H
#define ACTION_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <thread>
#include <unistd.h>

#define ACTION_PATH "./action.d/"
#define ACTION_TYPE ".act"

class Action{

private:
  std::string _name;
  int _banTime;
  std::vector<std::string> _actions;
  std::vector<std::string> _unacts;
  std::vector<std::thread*> _threads;

  void unact(std::map<std::string, std::string> replacements);

public:
  Action(std::string &name, int banTime);
  void act(std::map<std::string, std::string> &replacements);
  std::string get_name();
};


#endif
