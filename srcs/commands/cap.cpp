/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cap.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 22:42:11 by yxu               #+#    #+#             */
/*   Updated: 2025/09/06 21:36:30 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"

bool Command::cap(Server &server, Client &client, const IrcMessage &command) {
  (void)command;
  IrcMessage msg = CommandUtils::createIrcMessage(
      server.getServerName(), "CAP", client.getNickname() + " LS :");
  CommandUtils::reply(server, client, msg);
  return true;
}
