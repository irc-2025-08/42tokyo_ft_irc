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
  if (command.params.empty()) {
    return false;
  }

  std::string subcommand = command.params[0];
  
  if (subcommand == "LS") {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "CAP", client.getNickname() + " LS :");
    CommandUtils::reply(server, client, msg);
  } else if (subcommand == "REQ") {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "CAP", client.getNickname() + " NAK :");
    CommandUtils::reply(server, client, msg);
  } else if (subcommand == "END") {
  } else {
    return false;
  }
  
  return true;
}
