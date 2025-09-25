/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:24 by yxu               #+#    #+#             */
/*   Updated: 2025/09/23 21:03:25 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CommandHandler.hpp"
#include "../includes/Command.hpp"
#include "../includes/CommandUtils.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

std::map<std::string, bool (*)(Server &, Client &, const IrcMessage &)>
    CommandHandler::commandMap_;

void CommandHandler::initCommandMap() {
  commandMap_["PING"] = Command::ping;
  commandMap_["NICK"] = Command::nick;
  commandMap_["CAP"] = Command::cap;
  commandMap_["USER"] = Command::user;
  commandMap_["JOIN"] = Command::join;
  commandMap_["PART"] = Command::part;
  commandMap_["KICK"] = Command::kick;
  commandMap_["NAMES"] = Command::names;
  commandMap_["INVITE"] = Command::invite;
  commandMap_["MODE"] = Command::mode;
  commandMap_["TOPIC"] = Command::topic;
  commandMap_["DEBUG"] = Command::debug;
  commandMap_["QUIT"] = Command::quit;
  commandMap_["MOTD"] = Command::motd;
  commandMap_["PRIVMSG"] = Command::privmsg;
  commandMap_["PASS"] = Command::pass;
}

void CommandHandler::parseAndProcessCommand(Server &server, Client &client) {
  std::string::size_type pos = client.recvBuffer_.find("\r\n");
  while (pos != std::string::npos) {
    std::string cmdLine = client.recvBuffer_.substr(0, pos);
    client.recvBuffer_.erase(0, pos + 2);

    IrcMessage command;
    try {
      command = parseCommandLine(cmdLine);

    } catch (const std::runtime_error &e) {
      std::cerr << "[INFO] ircd: " << e.what() << std::endl;
      pos = client.recvBuffer_.find("\r\n");
      continue;
    }

    if (command.command != "PING") {
      std::cout << "[DEBUG] Received message from client " << client.getFd()
                << ": \"" << cmdLine << "\"\n"
                << "        prefix: \"" << command.prefix << "\", command: \""
                << command.command << "\", params: [";
      for (size_t i = 0; i < command.params.size(); i++) {
        std::cout << "\"" << command.params[i] << "\""
                  << (i == command.params.size() - 1 ? "" : ", ");
      }
      std::cout << "]" << std::endl;
    }

    // Clients SHOULD not use a prefix when sending a message from themselves;
    // from RFC 2812
    if (!command.prefix.empty()) {
      std::cerr << "[INFO] ignored prefix in message from client "
                << client.getFd() << ": " << cmdLine << std::endl;
      command.prefix = "";
    }

    processCommand(server, client, command);

    pos = client.recvBuffer_.find("\r\n");
  }
}

IrcMessage CommandHandler::parseCommandLine(const std::string &cmdLine) {
  if (cmdLine.length() > MAX_MSG_LENGTH - 2) // -2 to exclude \r\n
    throw std::runtime_error("Parser: Message too long");

  if (cmdLine.find('\0') != std::string::npos)
    throw std::runtime_error("Parser: Message contains null character");

  IrcMessage ircCommand;
  ircCommand.prefix = "";
  ircCommand.command = "";
  ircCommand.params = std::vector<std::string>();
  ircCommand.lastParamIsTrailing = false;

  std::string tempLine = cmdLine;
  std::string trailingPart;

  // parse prefix
  if (!tempLine.empty() && tempLine[0] == ':') {
    std::string::size_type spacePos = tempLine.find(' ');
    if (spacePos != std::string::npos) {
      ircCommand.prefix = tempLine.substr(1, spacePos - 1);
      tempLine = tempLine.substr(spacePos + 1);
    } else
      throw std::runtime_error("Parser: Wrong format");
  }

  // parse trailing
  std::string::size_type trailingPos = tempLine.find(" :");
  if (trailingPos != std::string::npos) {
    trailingPart = tempLine.substr(trailingPos + 2);
    tempLine = tempLine.substr(0, trailingPos);
    ircCommand.lastParamIsTrailing = true;
  }

  // parse command and params
  std::istringstream iss(tempLine);
  std::string token;

  // first token is command
  if (iss >> token) {
    ircCommand.command = token;
    std::transform(ircCommand.command.begin(), ircCommand.command.end(),
                   ircCommand.command.begin(), ::toupper);
  }
  // rest tokens are params
  while (iss >> token) {
    ircCommand.params.push_back(token);
  }

  // if trailing, add it to params
  if (ircCommand.lastParamIsTrailing) {
    ircCommand.params.push_back(trailingPart);
  }

  else if (ircCommand.params.size() > MAX_MSG_PARAMS) {
    throw std::runtime_error("Parser: Too many parameters");
  }

  return ircCommand;
}

void CommandHandler::processCommand(Server &server, Client &client,
                                    const IrcMessage &command) {
  if (commandMap_.find(command.command) == commandMap_.end()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "421", command.command + " :Unknown command");
    CommandUtils::reply(server, client, msg);
    return;
  }

  commandMap_[command.command](server, client, command);
  return;
}
