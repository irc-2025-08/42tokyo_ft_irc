/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yx <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:13 by yxu               #+#    #+#             */
/*   Updated: 2025/09/06 21:37:11 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"
#include "../../includes/Server.hpp"

bool Command::nick(Server &server, Client &client, const IrcMessage &command) {
  if (command.params.size() == 0) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "431",
        client.getNickname() + " :No nickname given");
    CommandUtils::reply(server, client, msg);
    return false;
  }

  std::string oldNickname = client.getNickname();
  std::string newNickname = command.params[0];

  Client *existingClient = server.findClientByNickname(newNickname);
  if (existingClient != NULL && existingClient->getFd() != client.getFd()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "433",
        client.getNickname() + " " + newNickname +
            " :Nickname is already in use");
    CommandUtils::reply(server, client, msg);
    return false;
  }
  
  // クライアントのニックネームを更新
  client.setNickname(newNickname);
  
  // 全チャンネルでニックネームを更新
  server.updateNicknameInAllChannels(oldNickname, newNickname);
  
  if (!client.isRegistered() &&
      CommandUtils::isClientRegistrationComplete(client)) {
    client.setRegistered(true);
    CommandUtils::sendWelcomeMessage(server, client);
  }

  IrcMessage msg = CommandUtils::createIrcMessage(server.getServerName(), "353",
                                                  client.getNickname() + " = " +
                                                      command.params[0]);
  CommandUtils::reply(server, client, msg);
  return true;
}
