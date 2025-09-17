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

bool Command::topicCmd(Server &server, Client &client,
                      const IrcMessage &command) {
  // パラメータチェック (最低でもchannelが必要)
  if (command.params.size() < 1) {
    ServerHandler::queueMessage(server, client,
      ":myserver 461 " + client.getNickname() + " TOPIC :Not enough parameters\r\n");
    return false;
  }
  
  std::string channelName = command.params[0];
  std::string clientNickname = client.getNickname();
  
  // チャンネルが存在するかチェック
  Channel* channel = server.findChannel(channelName);
  if (channel == NULL) {
    ServerHandler::queueMessage(server, client,
      ":myserver 403 " + clientNickname + " " + channelName + " :No such channel\r\n");
    return false;
  }
  
  // クライアントがチャンネルのメンバーかチェック
  if (!channel->hasMember(clientNickname)) {
    ServerHandler::queueMessage(server, client,
      ":myserver 442 " + clientNickname + " " + channelName + " :You're not on that channel\r\n");
    return false;
  }
  
  // パラメータが1つの場合: トピックを表示
  if (command.params.size() == 1) {
    if (channel->isTopicSet()) {
      // 332 RPL_TOPIC: トピックを表示
      ServerHandler::queueMessage(server, client,
        ":myserver 332 " + clientNickname + " " + channelName + " :" + channel->getTopic() + "\r\n");
    } else {
      // 331 RPL_NOTOPIC: トピックが設定されていない
      ServerHandler::queueMessage(server, client,
        ":myserver 331 " + clientNickname + " " + channelName + " :No topic is set\r\n");
    }
    return true;
  }
  
  // パラメータが2つ以上の場合: トピックを設定
  // topic制限がある場合、オペレーターかどうかをチェック
  if (channel->isTopicRestricted() && !channel->isOperator(clientNickname)) {
    ServerHandler::queueMessage(server, client,
      ":myserver 482 " + clientNickname + " " + channelName + " :You're not channel operator\r\n");
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
      ServerHandler::queueMessage(server, *memberClient,
        ":" + clientNickname + " TOPIC " + channelName + " :" + newTopic + "\r\n");
    }
  }
  
  return true;
}
