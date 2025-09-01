#pragma once

#include <string>
#include <vector>

struct IrcMessage {
  std::string prefix;
  std::string command;
  std::vector<std::string> params;
  bool lastParamIsTrailing;
};
