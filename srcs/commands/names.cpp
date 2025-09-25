/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   names.cpp                                          :+:      :+:    :+:   */
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
#include <iostream>

bool Command::names(Server &server, Client &client,
                      const IrcMessage &command) {
  // パラメータチェック
  if (command.params.empty()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "461", 
      client.getNickname() + " NAMES :Not enough parameters");
    CommandUtils::reply(server, client, msg);\
    return false;
  }
  
  std::string channelName = command.params[0];
  std::string clientNickname = client.getNickname();
  
  // デバッグ: channelNameの内容を確認
  std::cout << "[DEBUG] namesCmd - channelName: \"" << channelName << "\" (length: " << channelName.length() << ")" << std::endl;
  
  // チャンネルが存在するかチェック
  Channel* channel = server.findChannel(channelName);
  
  if (channel == NULL) {
    // チャンネルが存在しない場合
    IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "403", 
      clientNickname + " " + channelName + " :No such channel");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  // チャンネルメンバーのリストを取得
  std::vector<std::string> members = channel->getMembers();
  
  // メンバーリストを文字列として結合
  std::string membersList = "";
  for (size_t i = 0; i < members.size(); i++) {
    membersList += members[i];
    if (i < members.size() - 1) {
      membersList += " ";
    }
  }
  
  // 353レスポンス（チャンネルメンバーリスト）
  IrcMessage msg1 = CommandUtils::createIrcMessage(
    server.getServerName(), "353", 
    clientNickname + " = " + channelName + " :" + membersList);
  CommandUtils::reply(server, client, msg1);
  
  // 366レスポンス（メンバーリスト終了）
  IrcMessage msg2 = CommandUtils::createIrcMessage(
    server.getServerName(), "366", 
    clientNickname + " " + channelName + " :End of NAMES list");
  CommandUtils::reply(server, client, msg2);
  
  return true;
}
