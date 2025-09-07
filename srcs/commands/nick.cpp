/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:13 by yxu               #+#    #+#             */
/*   Updated: 2025/09/06 21:37:11 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"

// todo: check if the nickname is valid and not already used
bool Command::nick(Server &server, Client &client, const IrcMessage &command) {
  if (command.params.size() == 0) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "431",
        client.getNickname() + " :Nickname is required");
    CommandUtils::reply(server, client, msg);
    return true;
  }

  client.setNickname(command.params[0]);
  IrcMessage msg = CommandUtils::createIrcMessage(server.getServerName(), "353",
                                                  client.getNickname() + " = " +
                                                      command.params[0]);
  CommandUtils::reply(server, client, msg);
  return true;
}
