/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:13 by yxu               #+#    #+#             */
/*   Updated: 2025/09/04 00:49:07 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Client.hpp"
#include "../../includes/CommandHandler.hpp"
#include "../../includes/Server.hpp"

bool CommandHandler::nickCmd(Server &server, Client &client,
                             const IrcMessage &command) {
  (void)command;
  if (command.params.size() == 0) {
    IrcMessage msg =
        createIrcMessage(server.getServerName(), "431",
                         client.getNickname() + " :Nickname is required");
    reply(server, client, msg);
    return true;
  }

  client.setNickname(command.params[0]);
  IrcMessage msg =
      createIrcMessage(server.getServerName(), "353",
                       client.getNickname() + " = " + command.params[0]);
  reply(server, client, msg);
  return true;
}
