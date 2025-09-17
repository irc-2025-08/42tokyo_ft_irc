/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
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

bool Command::inviteCmd(Server &server, Client &client,
                       const IrcMessage &command) {
  // パラメータチェック (nickname channel)
  if (command.params.size() < 2) {
    ServerHandler::queueMessage(server, client,
      ":myserver 461 " + client.getNickname() + " INVITE :Not enough parameters\r\n");
    return false;
  }
  
  std::string targetNickname = command.params[0];
  std::string channelName = command.params[1];
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
  
  // チャンネルがinvite onlyかつクライアントがoperatorかチェック
  if (channel->isInvitationOnly() && !channel->isOperator(clientNickname)) {
    ServerHandler::queueMessage(server, client,
      ":myserver 482 " + clientNickname + " " + channelName + " :You're not channel operator\r\n");
    return false;
  }
  
  // 招待対象のクライアントが存在するかチェック
  Client* targetClient = server.findClientByNickname(targetNickname);
  if (targetClient == NULL) {
    ServerHandler::queueMessage(server, client,
      ":myserver 401 " + clientNickname + " " + targetNickname + " :No such nick/channel\r\n");
    return false;
  }
  
  // 招待対象が既にチャンネルのメンバーかチェック
  if (channel->hasMember(targetNickname)) {
    ServerHandler::queueMessage(server, client,
      ":myserver 443 " + clientNickname + " " + targetNickname + " " + channelName + " :is already on channel\r\n");
    return false;
  }
  
  // 招待対象をチャンネルに追加
  channel->addMember(targetNickname);
  
  // 招待者にINVITE成功の応答を送信
  ServerHandler::queueMessage(server, client,
    ":myserver 341 " + clientNickname + " " + targetNickname + " " + channelName + "\r\n");
  
  // 招待された人にINVITE通知を送信
  ServerHandler::queueMessage(server, *targetClient,
    ":" + clientNickname + " INVITE " + targetNickname + " " + channelName + "\r\n");
  
  // チャンネルの他のメンバーにJOIN通知を送信
  std::vector<std::string> members = channel->getMembers();
  for (std::vector<std::string>::const_iterator it = members.begin();
       it != members.end(); ++it) {
    if (*it != targetNickname) {
      Client* memberClient = server.findClientByNickname(*it);
      if (memberClient) {
        ServerHandler::queueMessage(server, *memberClient,
          ":" + targetNickname + " JOIN " + channelName + "\r\n");
      }
    }
  }
  
  return true;
}
