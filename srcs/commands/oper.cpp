/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   oper.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 00:00:00 by yxu               #+#    #+#             */
/*   Updated: 2025/09/26 00:00:00 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"

bool Command::oper(Server &server, Client &client, const IrcMessage &command) {
  if (!client.isRegistered()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "451",
        client.getNickname() + " :You have not registered");
    CommandUtils::reply(server, client, msg);
    return true;
  }

  if (command.params.size() < 2) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "461",
        client.getNickname() + " OPER :Not enough parameters");
    CommandUtils::reply(server, client, msg);
    return true;
  }

  const std::string &name = command.params[0];
  const std::string &password = command.params[1];
  
  // Simple operator authentication (for demonstration)
  // In a real implementation, this would check against a configuration file
  if (name == "admin" && password == "adminpass") {
    client.setOperator(true);
    
    // Send RPL_YOUREOPER (381)
    IrcMessage operMsg = CommandUtils::createIrcMessage(
        server.getServerName(), "381",
        client.getNickname() + " :You are now an IRC operator");
    CommandUtils::reply(server, client, operMsg);
    
    // Send MODE message to indicate new user modes (+o)
    IrcMessage modeMsg = CommandUtils::createIrcMessage(
        server.getServerName(), "MODE",
        client.getNickname() + " +o");
    CommandUtils::reply(server, client, modeMsg);
  } else {
    // Send ERR_PASSWDMISMATCH (464) or ERR_NOOPERHOST (491)
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "464",
        client.getNickname() + " :Password incorrect");
    CommandUtils::reply(server, client, msg);
  }
  
  return true;
}