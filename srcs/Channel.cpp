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
      limit_of_members(0),
      channel_password(""),
      channel_topic("") {
  // メンバーリストは空で初期化される（vectorのデフォルトコンストラクタ）
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

bool Channel::isOperator(const std::string& nickname) const {
  for (std::vector<std::string>::const_iterator it = channel_operator_list.begin();
       it != channel_operator_list.end(); ++it) {
    if (*it == nickname) {
      return true;
    }
  }
  return false;
}
