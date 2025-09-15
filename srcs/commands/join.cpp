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

bool Command::joinCmd(Server &server, Client &client,
                     const IrcMessage &command) {
  // パラメータチェック
  if (command.params.empty()) {
    ServerHandler::queueMessage(server, client,
      ":myserver 461 " + client.getNickname() + " JOIN :Not enough parameters\r\n");
    return false;
  }
  
  std::string channelName = command.params[0];
  std::string clientNickname = client.getNickname();
  
  // チャンネル名の検証
  if (channelName.empty()) {
    ServerHandler::queueMessage(server, client,
      ":myserver 476 " + clientNickname + " " + channelName + " :Bad Channel Mask\r\n");
    return false;
  }
  
  // チャンネルが存在するかチェック
  Channel* channel = server.findChannel(channelName);
  
  if (channel == NULL) {
    // チャンネルが存在しない場合、新しく作成
    server.addChannel(channelName);
    channel = server.findChannel(channelName);
  }
  
  // クライアントが既にチャンネルのメンバーかチェック
  if (channel->hasMember(clientNickname)) {
    // 既にメンバーの場合は何もしない
    return true;
  }
  
  // チャンネルにクライアントを追加
  channel->addMember(clientNickname);
  
  // JOIN成功の応答を送信
  ServerHandler::queueMessage(server, client,
    ":" + clientNickname + " JOIN " + channelName + "\r\n");
  
  return true;
}
