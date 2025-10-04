/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
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
#include "../../includes/CommandUtils.hpp"

bool Command::topic(Server &server, Client &client,
                      const IrcMessage &command) {
  // パラメータチェック (最低でもchannelが必要)
  if (command.params.size() < 1) {
    IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "461", 
      client.getNickname() + " TOPIC :Not enough parameters");
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
  
  // パラメータが1つの場合: トピックを表示
  if (command.params.size() == 1) {
    if (channel->isTopicSet()) {
      // 332 RPL_TOPIC: トピックを表示
      IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "332", 
        clientNickname + " " + channelName + " :" + channel->getTopic());
      CommandUtils::reply(server, client, msg);
    } else {
      // 331 RPL_NOTOPIC: トピックが設定されていない
      IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "331", 
        clientNickname + " " + channelName + " :No topic is set");
      CommandUtils::reply(server, client, msg);
    }
    return true;
  }
  
  // パラメータが2つ以上の場合: トピックを設定
  // topic制限がある場合、オペレーターかどうかをチェック
  if (channel->isTopicRestricted() && !channel->isOperator(clientNickname)) {
    IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "482", 
      clientNickname + " " + channelName + " :You're not channel operator");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  // 新しいトピックを設定
  std::string newTopic = command.params[1];
  channel->setTopic(newTopic);
  
  // チャンネルの全メンバーにトピック変更を通知
  std::vector<std::string> members = channel->getMembers();
  for (std::vector<std::string>::const_iterator it = members.begin();
       it != members.end(); ++it) {
    Client* memberClient = server.findClientByNickname(*it);
    if (memberClient) {
      IrcMessage msg = CommandUtils::createIrcMessage(
        clientNickname, "TOPIC", 
        channelName + " :" + newTopic);
      CommandUtils::reply(server, *memberClient, msg);
    }
  }
  
  return true;
}
