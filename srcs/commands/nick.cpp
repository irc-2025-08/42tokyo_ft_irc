/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:13 by yxu               #+#    #+#             */
/*   Updated: 2025/09/02 00:18:14 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/CommandHandler.hpp"
#include "../../includes/ServerHandler.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"

// TODO

bool CommandHandler::nickCmd(Server &server, Client &client,
                             const IrcMessage &command) {
  (void)command;
  if (command.params.size() == 0) {
    ServerHandler::queueMessage(server, client,
                                ":" + server.getServerName() + " 431 " + client.getNickname() + " :Nickname is required\r\n");
    return true;
  }
  client.setNickname(command.params[0]);
  ServerHandler::queueMessage(server, client,
                              ":" + server.getServerName() + " 353 " + client.getNickname() + " = " + command.params[0] + "\r\n");
  return true;
}
