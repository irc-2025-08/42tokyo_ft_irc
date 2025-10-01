#pragma once

#include <string>
#include <vector>

class Channel {
public:
  Channel(const std::string &name);
  ~Channel();
  
  // Member management methods
  void addMember(const std::string& nickname);
  void removeMember(const std::string& nickname);
  bool hasMember(const std::string& nickname) const;
  bool isEmpty() const;
  size_t getMemberCount() const;
  const std::string& getName() const;
  
  // Operator management methods
  void addOperator(const std::string& nickname);
  void removeOperator(const std::string& nickname);
  bool isOperator(const std::string& nickname) const;
  size_t getOperatorCount() const;
  std::vector<std::string> getOperators() const;
  
  // Invitation management methods
  bool isInvitationOnly() const;
  void setInvitationOnly(bool inviteOnly);
  std::vector<std::string> getMembers() const;
  
  // Topic management methods
  bool isTopicRestricted() const;
  void setTopicRestricted(bool restricted);
  bool isTopicSet() const;
  const std::string& getTopic() const;
  void setTopic(const std::string& topic);
  
  // User limit management methods
  int getUserLimit() const;
  void setUserLimit(int limit);
  bool hasUserLimit() const;
  
  // Channel key (password) management methods
  bool hasChannelKey() const;
  void setChannelKey(const std::string& key);
  void removeChannelKey();
  const std::string& getChannelKey() const;

private:
  std::string channel_name;
  std::vector<std::string> channel_member_list;
  std::vector<std::string> channel_operator_list;
  bool is_invitation_only;
  bool is_topic_restricted;
  bool is_topic_set;
  bool has_channel_key;
  int limit_of_members;
  std::string channel_password;
  std::string channel_topic;
};
