/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/XX XX:XX:XX by yxu               #+#    #+#             */
/*   Updated: 2025/01/XX XX:XX:XX by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/CommandHandler.hpp"
#include "../../includes/ServerHandler.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/CommandUtils.hpp"

bool Command::join(Server &server, Client &client,
                     const IrcMessage &command) {
  // パラメータチェック
  if (command.params.empty()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "461", 
      client.getNickname() + " JOIN :Not enough parameters");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  std::string channelName = command.params[0];
  std::string clientNickname = client.getNickname();
  
  // チャンネル名の検証
  if (channelName.empty() || channelName == "0") {
    return true;
  }
  
  // チャンネルが存在するかチェック
  Channel* channel = server.findChannel(channelName);
  bool isChannelCreator = false;
  
  if (channel == NULL) {
    // チャンネルが存在しない場合、新しく作成
    server.addChannel(channelName);
    channel = server.findChannel(channelName);
    isChannelCreator = true;
  }
  
  // クライアントが既にチャンネルのメンバーかチェック
  if (channel->hasMember(clientNickname)) {
    // 既にメンバーの場合は何もしない
    return true;
  }
  
  // チャンネル作成者でない場合のみ制限をチェック
  if (!isChannelCreator) {
    // invitation onlyチャンネルのチェック
    if (channel->isInvitationOnly()) {
      IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "473", 
        clientNickname + " " + channelName + " :Cannot join channel (+i)");
      CommandUtils::reply(server, client, msg);
      return false;
    }
    
    // ユーザー数制限のチェック
    if (channel->hasUserLimit()) {
      int currentMembers = static_cast<int>(channel->getMembers().size());
      if (currentMembers >= channel->getUserLimit()) {
        IrcMessage msg = CommandUtils::createIrcMessage(
          server.getServerName(), "471", 
          clientNickname + " " + channelName + " :Cannot join channel (+l)");
        CommandUtils::reply(server, client, msg);
        return false;
      }
    }
    
    // チャンネルキー（パスワード）のチェック
    if (channel->hasChannelKey()) {
      std::string providedKey = "";
      if (command.params.size() > 1) {
        providedKey = command.params[1];
      }
      
      if (providedKey != channel->getChannelKey()) {
        IrcMessage msg = CommandUtils::createIrcMessage(
          server.getServerName(), "475", 
          clientNickname + " " + channelName + " :Cannot join channel (+k)");
        CommandUtils::reply(server, client, msg);
        return false;
      }
    }
  }
  
  // チャンネルにクライアントを追加
  channel->addMember(clientNickname);
  
  // チャンネル作成者の場合、オペレーター権限を付与
  if (isChannelCreator) {
    channel->addOperator(clientNickname);
  }
  
  // JOIN成功の応答を送信
  IrcMessage msg = CommandUtils::createIrcMessage(
    clientNickname, "JOIN", channelName);
  CommandUtils::reply(server, client, msg);
  
  return true;
}
