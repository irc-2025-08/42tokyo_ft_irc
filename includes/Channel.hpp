#pragma once

#include <string>
#include <vector>

class Channel {
public:
  Channel(const std::string &name);
  ~Channel();
  
  // Member management methods
  void addMember(const std::string& nickname);
  bool hasMember(const std::string& nickname) const;
  const std::string& getName() const;
  
  // Operator management methods
  void addOperator(const std::string& nickname);

private:
  std::string channel_name;
  std::vector<std::string> channel_member_list;
  std::vector<std::string> channel_operator_list;
  bool is_invitation_only;
  int limit_of_members;
  std::string channel_password;
  std::string channel_topic;
};
