#include "../includes/CommandHandler.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/ServerHandler.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

// TODO

std::map<std::string, bool (*)(Server &, Client &, const IrcCommand &)>
    CommandHandler::commandMap_;

void CommandHandler::initCommandMap() { 
  commandMap_["PING"] = pingCmd;
  commandMap_["JOIN"] = joinCmd;
}

IrcCommand CommandHandler::parseCommandLine(const std::string &cmdLine) {
  IrcCommand ircCommand;
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

    IrcCommand command = parseCommandLine(cmdLine);
    processCommand(server, client, command);

    pos = client.recvBuffer_.find("\r\n");
  }
}

void CommandHandler::processCommand(Server &server, Client &client,
                                    const IrcCommand &command) {
  std::cout << "debug: Received command from client " << client.getFd()
            << ":\nprefix: " << command.prefix
            << "\ncommand: " << command.command << "\n";
  for (size_t i = 0; i < command.params.size(); i++) {
    std::cout << "param" << i << ": " << command.params[i] << "\n";
  }
  std::cout << std::endl;

  if (commandMap_.find(command.command) == commandMap_.end()) {
    ServerHandler::queueMessage(server, client,
                                ":myserver 421 " + client.getNickname() + " " +
                                    command.command + " :Unknown command\r\n");
    return;
  }

  commandMap_[command.command](server, client, command);
  return;
}