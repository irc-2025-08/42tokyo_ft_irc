/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 22:49:59 by yxu               #+#    #+#             */
/*   Updated: 2025/09/06 21:37:40 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"

static bool isRegistrationComplete(const Client &client) {
  return client.isPasswordProvided() && 
         client.getNickname() != "*" && 
         !client.getUsername().empty();
}

bool Command::user(Server &server, Client &client, const IrcMessage &command) {
  if (client.isRegistered()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "462",
        client.getNickname() + " :You may not reregister");
    CommandUtils::reply(server, client, msg);
    return true;
  }

  if (command.params.size() < 4) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "461",
        client.getNickname() + " USER :Not enough parameters");
    CommandUtils::reply(server, client, msg);
    return true;
  }

  const std::string &username = command.params[0];
  const std::string &realname = command.params[3];
  
  client.setUsername(username);
  client.setRealname(realname);
  
  if (isRegistrationComplete(client)) {
    client.setRegistered(true);
    CommandUtils::sendWelcomeMessage(server, client);
  }
  
  return true;
}
