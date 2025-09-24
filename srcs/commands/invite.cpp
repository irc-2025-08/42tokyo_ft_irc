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
#include "../../includes/CommandUtils.hpp"

bool Command::inviteCmd(Server &server, Client &client,
                       const IrcMessage &command) {
  // パラメータチェック (nickname channel)
  if (command.params.size() < 2) {
    IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "461", 
      client.getNickname() + " INVITE :Not enough parameters");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  std::string targetNickname = command.params[0];
  std::string channelName = command.params[1];
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
  
  // チャンネルがinvite onlyかつクライアントがoperatorかチェック
  if (channel->isInvitationOnly() && !channel->isOperator(clientNickname)) {
    IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "482", 
      clientNickname + " " + channelName + " :You're not channel operator");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  // 招待対象のクライアントが存在するかチェック
  Client* targetClient = server.findClientByNickname(targetNickname);
  if (targetClient == NULL) {
    IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "401", 
      clientNickname + " " + targetNickname + " :No such nick/channel");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  // 招待対象が既にチャンネルのメンバーかチェック
  if (channel->hasMember(targetNickname)) {
    IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "443", 
      clientNickname + " " + targetNickname + " " + channelName + " :is already on channel");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  // 招待対象をチャンネルに追加
  channel->addMember(targetNickname);
  
  // 招待者にINVITE成功の応答を送信
  IrcMessage msg1 = CommandUtils::createIrcMessage(
    server.getServerName(), "341", 
    clientNickname + " " + targetNickname + " " + channelName);
  CommandUtils::reply(server, client, msg1);
  
  // 招待された人にINVITE通知を送信
  IrcMessage msg2 = CommandUtils::createIrcMessage(
    clientNickname, "INVITE", 
    targetNickname + " " + channelName);
  CommandUtils::reply(server, *targetClient, msg2);
  
  // チャンネルの他のメンバーにJOIN通知を送信
  std::vector<std::string> members = channel->getMembers();
  for (std::vector<std::string>::const_iterator it = members.begin();
       it != members.end(); ++it) {
    if (*it != targetNickname) {
      Client* memberClient = server.findClientByNickname(*it);
      if (memberClient) {
        IrcMessage msg3 = CommandUtils::createIrcMessage(
          targetNickname, "JOIN", channelName);
        CommandUtils::reply(server, *memberClient, msg3);
      }
    }
  }
  
  return true;
}
