#pragma once

#include <string>
#include <vector>

// TODO
class Channel {
public:
  Channel(const std::string &name);
  ~Channel();

private:
  std::string channel_name;
  std::vector<std::string> channel_member_list;
  bool is_invitation_only;
  int limit_of_members;
  std::string channel_password;
  std::string channel_topic;
};
