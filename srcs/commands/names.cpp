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

bool Command::namesCmd(Server &server, Client &client,
                      const IrcMessage &command) {
  // パラメータチェック
  if (command.params.empty()) {
    ServerHandler::queueMessage(server, client,
      ":myserver 461 " + client.getNickname() + " NAMES :Not enough parameters\r\n");
    return false;
  }
  
  std::string channelName = command.params[0];
  std::string clientNickname = client.getNickname();
  
  // チャンネルが存在するかチェック
  Channel* channel = server.findChannel(channelName);
  
  if (channel == NULL) {
    // チャンネルが存在しない場合
    ServerHandler::queueMessage(server, client,
      ":myserver 403 " + clientNickname + " " + channelName + " :No such channel\r\n");
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
  ServerHandler::queueMessage(server, client,
    ":myserver 353 " + clientNickname + " = " + channelName + " :" + membersList + "\r\n");
  
  // 366レスポンス（メンバーリスト終了）
  ServerHandler::queueMessage(server, client,
    ":myserver 366 " + clientNickname + " " + channelName + " :End of NAMES list\r\n");
  
  return true;
}
