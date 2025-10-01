/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/XX XX:XX:XX by yxu               #+#    #+#             */
/*   Updated: 2025/01/XX XX:XX:XX by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"

bool Command::kick(Server &server, Client &client,
                     const IrcMessage &command) {
  // パラメータチェック（チャンネル名とターゲットユーザー名が必要）
  if (command.params.size() < 2) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "461",
        client.getNickname() + " KICK :Not enough parameters");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  std::string channelName = command.params[0];
  std::string targetNickname = command.params[1];
  std::string clientNickname = client.getNickname();
  
  // チャンネル名の検証
  if (channelName.empty()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "476",
        clientNickname + " " + channelName + " :Bad Channel Mask");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  // ターゲットユーザー名の検証
  if (targetNickname.empty()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "461",
        clientNickname + " KICK :Not enough parameters");
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
  
  // 実行者がチャンネルのメンバーかチェック
  if (!channel->hasMember(clientNickname)) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "442",
        clientNickname + " " + channelName + " :You're not on that channel");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  // 実行者がオペレーターかチェック
  if (!channel->isOperator(clientNickname)) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "482",
        clientNickname + " " + channelName + " :You're not channel operator");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  // ターゲットがチャンネルのメンバーかチェック
  if (!channel->hasMember(targetNickname)) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "441",
        clientNickname + " " + targetNickname + " " + channelName + " :They aren't on that channel");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  // チャンネルからターゲットユーザーを削除
  channel->removeMember(targetNickname);
  
  // オペレーターの場合、オペレーターリストからも削除
  if (channel->isOperator(targetNickname)) {
    channel->removeOperator(targetNickname);
  }
  
  // チャンネルが空になった場合、チャンネルを削除
  if (channel->isEmpty()) {
    server.removeChannel(channelName);
  }
  
  // KICK成功の応答を送信
  IrcMessage msg = CommandUtils::createIrcMessage(
      clientNickname, "KICK", channelName + " " + targetNickname);
  CommandUtils::reply(server, client, msg);
  
  return true;
}
