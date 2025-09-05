/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:24 by yxu               #+#    #+#             */
/*   Updated: 2025/09/05 23:28:59 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CommandHandler.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/ServerHandler.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

// TODO

std::map<std::string, bool (*)(Server &, Client &, const IrcMessage &)>
    CommandHandler::commandMap_;

void CommandHandler::initCommandMap() {
  commandMap_["PING"] = pingCmd;
  commandMap_["NICK"] = nickCmd;
  commandMap_["CAP"] = capCmd;
  commandMap_["USER"] = userCmd;
}

IrcMessage CommandHandler::parseCommandLine(const std::string &cmdLine) {
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
    } else {
      // wrong format
      ircCommand.command = "";
      return ircCommand;
    }
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

  return ircCommand;
}

void CommandHandler::parseAndProcessCommand(Server &server, Client &client) {
  std::string::size_type pos = client.recvBuffer_.find("\r\n");
  while (pos != std::string::npos) {
    std::string cmdLine = client.recvBuffer_.substr(0, pos);
    client.recvBuffer_.erase(0, pos + 2);

    IrcMessage command = parseCommandLine(cmdLine);
    if (!isIrcMessageValid(command)) {
      std::cerr << "[WARN] Received illegal message from client " << client.getFd() << ": " << cmdLine << std::endl;
      continue;
    }

    processCommand(server, client, command);

    pos = client.recvBuffer_.find("\r\n");
  }
}

void CommandHandler::processCommand(Server &server, Client &client,
                                    const IrcMessage &command) {
  std::cout << "debug: Received command from client " << client.getFd()
            << ":\nprefix: " << command.prefix
            << "\ncommand: " << command.command << "\n";
  for (size_t i = 0; i < command.params.size(); i++) {
    std::cout << "param" << i << ": " << command.params[i] << "\n";
  }
  std::cout << std::endl;

  if (commandMap_.find(command.command) == commandMap_.end()) {
    IrcMessage msg = createIrcMessage(server.getServerName(), "421",
                                      command.command + " :Unknown command");
    reply(server, client, msg);
    return;
  }

  commandMap_[command.command](server, client, command);
  return;
}

int CommandHandler::reply(Server &server, Client &client,
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
    reply += ":" + message.params.back() + "\r\n";
  } else {
    reply += message.params.back() + "\r\n";
  }

  return ServerHandler::queueMessage(server, client, reply);
}

IrcMessage CommandHandler::createIrcMessage(const std::string &prefix,
                                            const std::string &command,
                                            const std::string &paramsStr) {
  std::string cmdline = "";
  if (!prefix.empty()) {
    cmdline += ":" + prefix + " ";
  }
  cmdline += command + " " + paramsStr + "\r\n";
  return parseCommandLine(cmdline);
}

IrcMessage CommandHandler::createIrcMessage(const std::string &command,
                                            const std::string &paramsStr) {
  return createIrcMessage("", command, paramsStr);
}

// to be implemented
bool CommandHandler::isIrcMessageValid(const IrcMessage &message) {
  (void)message;
  return true;
}
