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
    // CAP LS - list capabilities
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "CAP", client.getNickname() + " LS :");
    CommandUtils::reply(server, client, msg);
  } else if (subcommand == "REQ") {
    // CAP REQ - request capabilities (we don't support any, so NAK)
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "CAP", client.getNickname() + " NAK :");
    CommandUtils::reply(server, client, msg);
  } else if (subcommand == "END") {
    // CAP END - end capability negotiation
    // No response needed, just continue with registration
  } else {
    // Unknown subcommand
    return false;
  }
  
  return true;
}
