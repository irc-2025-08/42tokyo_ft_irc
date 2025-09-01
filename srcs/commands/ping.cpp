/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:19 by yxu               #+#    #+#             */
/*   Updated: 2025/09/02 00:18:20 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/CommandHandler.hpp"
#include "../../includes/ServerHandler.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"

// TODO
bool CommandHandler::pingCmd(Server &server, Client &client,
                                 const IrcMessage &command) {
  (void)command;
  ServerHandler::queueMessage(server, client, ":myserver PONG myserver :" + client.getNickname() + "\r\n");
  return true;
}
