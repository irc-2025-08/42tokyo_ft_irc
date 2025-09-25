/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/XX XX:XX:XX by yxu               #+#    #+#             */
/*   Updated: 2025/01/XX XX:XX:XX by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"

bool Command::part(Server &server, Client &client,
                     const IrcMessage &command) {
  // パラメータチェック
  if (command.params.empty()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "461",
        client.getNickname() + " PART :Not enough parameters");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  std::string channelName = command.params[0];
  std::string clientNickname = client.getNickname();
  
  // チャンネル名の検証
  if (channelName.empty()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "476",
        clientNickname + " " + channelName + " :Bad Channel Mask");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
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
  
  // クライアントがチャンネルのメンバーかチェック
  if (!channel->hasMember(clientNickname)) {
    // メンバーでない場合
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "442",
        clientNickname + " " + channelName + " :You're not on that channel");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  // チャンネルからクライアントを削除
  channel->removeMember(clientNickname);
  
  // PART成功の応答を送信
  IrcMessage msg = CommandUtils::createIrcMessage(
      clientNickname, "PART", channelName);
  CommandUtils::reply(server, client, msg);
  
  return true;
}
