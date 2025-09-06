/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:19 by yxu               #+#    #+#             */
/*   Updated: 2025/09/06 21:37:02 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"

bool Command::ping(Server &server, Client &client, const IrcMessage &command) {
  (void)command;
  IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "PONG",
      server.getServerName() + " :" + client.getNickname());
  CommandUtils::reply(server, client, msg);
  return true;
}
