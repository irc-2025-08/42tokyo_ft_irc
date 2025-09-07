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

bool Channel::hasMember(const std::string& nickname) const {
  for (const auto& member : channel_member_list) {
    if (member == nickname) {
      return true;
    }
  }
  return false;
}

const std::string& Channel::getName() const {
  return channel_name;
}
