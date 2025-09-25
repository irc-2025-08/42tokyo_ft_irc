/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/06 21:50:24 by yxu               #+#    #+#             */
/*   Updated: 2025/09/23 20:49:34 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CommandUtils.hpp"
#include "../includes/CommandHandler.hpp"
#include "../includes/ServerHandler.hpp"
#include <iostream>

int CommandUtils::sendWelcomeMessage(Server &server, Client &client) {
  IrcMessage msg =
      createIrcMessage(server.getServerName(), "001",
                       client.getNickname() + " :Welcome to the IRC server, " +
                           client.getNickname());
  reply(server, client, msg);
  return 0;
}

int CommandUtils::reply(Server &server, Client &client,
                        const IrcMessage &message) {
  std::string reply = "";
  if (!message.prefix.empty()) {
    reply += ":" + message.prefix + " ";
  }

  reply += message.command + " ";

  for (size_t i = 0; i < message.params.size() - 1; i++) {
    reply += message.params[i] + " ";
  }

  if (message.params.size() > 0 && message.lastParamIsTrailing) {
    reply += ":" + message.params.back();
  } else {
    reply += message.params.back();
  }

  if (reply.find("PONG") == std::string::npos) {
    std::cout << "[DEBUG] Sending message to client " << client.getFd() << ": "
              << "\"" << reply << "\"\n";
  }

  reply += "\r\n";

  return ServerHandler::queueMessage(server, client, reply);
}

IrcMessage CommandUtils::createIrcMessage(const std::string &prefix,
                                          const std::string &command,
                                          const std::string &paramsStr) {
  std::string cmdline = "";
  if (!prefix.empty()) {
    cmdline += ":" + prefix + " ";
  }
  cmdline += command + " " + paramsStr;
  return CommandHandler::parseCommandLine(cmdline);
}

IrcMessage CommandUtils::createIrcMessage(const std::string &command,
                                          const std::string &paramsStr) {
  return createIrcMessage("", command, paramsStr);
}
