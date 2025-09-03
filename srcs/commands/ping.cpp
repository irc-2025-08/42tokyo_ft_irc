/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:19 by yxu               #+#    #+#             */
/*   Updated: 2025/09/04 00:47:50 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Client.hpp"
#include "../../includes/CommandHandler.hpp"
#include "../../includes/Server.hpp"

bool CommandHandler::pingCmd(Server &server, Client &client,
                             const IrcMessage &command) {
  (void)command;
  IrcMessage msg =
      createIrcMessage(server.getServerName(), "PONG",
                       server.getServerName() + " :" + client.getNickname());
  reply(server, client, msg);
  return true;
}
