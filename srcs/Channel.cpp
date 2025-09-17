/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/XX XX:XX:XX by yxu               #+#    #+#             */
/*   Updated: 2025/01/XX XX:XX:XX by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

Channel::Channel(const std::string &name)
    : channel_name(name),
      is_invitation_only(false),
      is_topic_restricted(true),
      is_topic_set(false),
      has_channel_key(false),
      limit_of_members(0),
      channel_password(""),
      channel_topic("") {
  // メンバーリストは空で初期化される（vectorのデフォルトコンストラクタ）
  // デフォルトでは topic はオペレーターのみ変更可能
  // デフォルトではチャンネルキーは設定されていない
}

Channel::~Channel() {
  // vectorは自動的に解放されるため、特別な処理は不要
}

// Member management methods
void Channel::addMember(const std::string& nickname) {
  if (!hasMember(nickname)) {
    channel_member_list.push_back(nickname);
  }
}

void Channel::removeMember(const std::string& nickname) {
  for (std::vector<std::string>::iterator it = channel_member_list.begin();
       it != channel_member_list.end(); ++it) {
    if (*it == nickname) {
      channel_member_list.erase(it);
      return;
    }
  }
}

bool Channel::hasMember(const std::string& nickname) const {
  for (std::vector<std::string>::const_iterator it = channel_member_list.begin();
       it != channel_member_list.end(); ++it) {
    if (*it == nickname) {
      return true;
    }
  }
  return false;
}

const std::string& Channel::getName() const {
  return channel_name;
}

// Operator management methods
void Channel::addOperator(const std::string& nickname) {
  // 重複チェック
  for (std::vector<std::string>::const_iterator it = channel_operator_list.begin();
       it != channel_operator_list.end(); ++it) {
    if (*it == nickname) {
      return;  // 既にオペレーターの場合は追加しない
    }
  }
  channel_operator_list.push_back(nickname);
}

void Channel::removeOperator(const std::string& nickname) {
  for (std::vector<std::string>::iterator it = channel_operator_list.begin();
       it != channel_operator_list.end(); ++it) {
    if (*it == nickname) {
      channel_operator_list.erase(it);
      return;
    }
  }
}

bool Channel::isOperator(const std::string& nickname) const {
  for (std::vector<std::string>::const_iterator it = channel_operator_list.begin();
       it != channel_operator_list.end(); ++it) {
    if (*it == nickname) {
      return true;
    }
  }
  return false;
}

// Invitation management methods
bool Channel::isInvitationOnly() const {
  return is_invitation_only;
}

void Channel::setInvitationOnly(bool inviteOnly) {
  is_invitation_only = inviteOnly;
}

std::vector<std::string> Channel::getMembers() const {
  return channel_member_list;
}

// Topic management methods
bool Channel::isTopicRestricted() const {
  return is_topic_restricted;
}

void Channel::setTopicRestricted(bool restricted) {
  is_topic_restricted = restricted;
}

bool Channel::isTopicSet() const {
  return is_topic_set;
}

const std::string& Channel::getTopic() const {
  return channel_topic;
}

void Channel::setTopic(const std::string& topic) {
  channel_topic = topic;
  is_topic_set = !topic.empty();
}

// User limit management methods
int Channel::getUserLimit() const {
  return limit_of_members;
}

void Channel::setUserLimit(int limit) {
  limit_of_members = limit;
}

bool Channel::hasUserLimit() const {
  return limit_of_members > 0;
}

// Channel key (password) management methods
bool Channel::hasChannelKey() const {
  return has_channel_key;
}

void Channel::setChannelKey(const std::string& key) {
  channel_password = key;
  has_channel_key = true;
}

void Channel::removeChannelKey() {
  channel_password = "";
  has_channel_key = false;
}

const std::string& Channel::getChannelKey() const {
  return channel_password;
}
