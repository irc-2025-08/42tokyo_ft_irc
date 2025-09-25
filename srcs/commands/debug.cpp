/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 XX:XX:XX by yxu               #+#    #+#             */
/*   Updated: 2025/09/17 XX:XX:XX by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/CommandHandler.hpp"
#include "../../includes/ServerHandler.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"
#include <sstream>
#include "../../includes/CommandUtils.hpp"

bool Command::debug(Server &server, Client &client,
                      const IrcMessage &command) {
  // パラメータチェック (channelが必要)
  if (command.params.size() < 1) {
    IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "461", 
      client.getNickname() + " DEBUG :Not enough parameters (usage: DEBUG <channel>)");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  std::string channelName = command.params[0];
  std::string clientNickname = client.getNickname();
  
  // チャンネルが存在するかチェック
  Channel* channel = server.findChannel(channelName);
  if (channel == NULL) {
    IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "403", 
      clientNickname + " " + channelName + " :No such channel");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  // クライアントがチャンネルのメンバーかチェック
  if (!channel->hasMember(clientNickname)) {
    IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "442", 
      clientNickname + " " + channelName + " :You're not on that channel");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  // デバッグ情報の開始
  ServerHandler::queueMessage(server, client,
    ":myserver 900 " + clientNickname + " :=== DEBUG INFO for " + channelName + " ===\r\n");
  
  // チャンネル名
  ServerHandler::queueMessage(server, client,
    ":myserver 900 " + clientNickname + " :channel_name: " + channel->getName() + "\r\n");
  
  // メンバーリスト
  std::vector<std::string> members = channel->getMembers();
  std::ostringstream memberStream;
  memberStream << "channel_member_list (" << members.size() << "): ";
  for (std::vector<std::string>::const_iterator it = members.begin();
       it != members.end(); ++it) {
    if (it != members.begin()) memberStream << ", ";
    memberStream << *it;
  }
  ServerHandler::queueMessage(server, client,
    ":myserver 900 " + clientNickname + " :" + memberStream.str() + "\r\n");
  
  // オペレーターリスト（getOperators()メソッドがないので、各メンバーをチェック）
  std::ostringstream operatorStream;
  operatorStream << "channel_operator_list: ";
  bool firstOp = true;
  for (std::vector<std::string>::const_iterator it = members.begin();
       it != members.end(); ++it) {
    if (channel->isOperator(*it)) {
      if (!firstOp) operatorStream << ", ";
      operatorStream << *it;
      firstOp = false;
    }
  }
  ServerHandler::queueMessage(server, client,
    ":myserver 900 " + clientNickname + " :" + operatorStream.str() + "\r\n");
  
  // Invitation Only フラグ
  ServerHandler::queueMessage(server, client,
    ":myserver 900 " + clientNickname + " :is_invitation_only: " + 
    (channel->isInvitationOnly() ? "true" : "false") + "\r\n");
  
  // Topic Restricted フラグ
  ServerHandler::queueMessage(server, client,
    ":myserver 900 " + clientNickname + " :is_topic_restricted: " + 
    (channel->isTopicRestricted() ? "true" : "false") + "\r\n");
  
  // Topic Set フラグとトピック内容
  ServerHandler::queueMessage(server, client,
    ":myserver 900 " + clientNickname + " :is_topic_set: " + 
    (channel->isTopicSet() ? "true" : "false") + "\r\n");
  
  if (channel->isTopicSet()) {
    ServerHandler::queueMessage(server, client,
      ":myserver 900 " + clientNickname + " :channel_topic: \"" + 
      channel->getTopic() + "\"\r\n");
  } else {
    ServerHandler::queueMessage(server, client,
      ":myserver 900 " + clientNickname + " :channel_topic: (empty)\r\n");
  }
  
  // Channel Key フラグ
  ServerHandler::queueMessage(server, client,
    ":myserver 900 " + clientNickname + " :has_channel_key: " + 
    (channel->hasChannelKey() ? "true" : "false") + "\r\n");
  
  if (channel->hasChannelKey()) {
    ServerHandler::queueMessage(server, client,
      ":myserver 900 " + clientNickname + " :channel_password: \"" + 
      channel->getChannelKey() + "\"\r\n");
  } else {
    ServerHandler::queueMessage(server, client,
      ":myserver 900 " + clientNickname + " :channel_password: (empty)\r\n");
  }
  
  // User Limit
  std::ostringstream limitStream;
  limitStream << channel->getUserLimit();
  ServerHandler::queueMessage(server, client,
    ":myserver 900 " + clientNickname + " :limit_of_members: " + limitStream.str() + 
    " (hasUserLimit: " + (channel->hasUserLimit() ? "true" : "false") + ")\r\n");
  
  // デバッグ情報の終了
  ServerHandler::queueMessage(server, client,
    ":myserver 900 " + clientNickname + " :=== END DEBUG INFO ===\r\n");
  
  return true;
}
