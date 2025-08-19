#include "../includes/CommandHandler.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/ServerHandler.hpp"
#include "../includes/config.hpp"
#include <iostream>
#include <sstream>

// TODO

std::map<std::string, bool (*)(Server &, Client &, const IrcCommand &)> CommandHandler::commandMap_;

void CommandHandler::initCommandMap() {
  commandMap_["PING"] = pingCmd;
}

IrcCommand CommandHandler::parseCommandLine(const std::string &cmdLine) {
  std::istringstream iss(cmdLine);
  std::string token;
  IrcCommand ircCommand;
  std::vector<std::string> tokens;
  ircCommand.prefix = config::serverName;
  ircCommand.trailing = "";

  while (std::getline(iss, token, ' ')) {
    if (token.empty())
      continue;
    if (token[0] == ':') {
      ircCommand.trailing = token.substr(1);
      continue;
    }
    tokens.push_back(token);
  }

  ircCommand.command = tokens[0];
  ircCommand.params = std::vector<std::string>(tokens.begin() + 1, tokens.end());
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
            << ":\n" << command.command << std::endl;

  if (commandMap_.find(command.command) == commandMap_.end()) {
    ServerHandler::queueMessage(server, client, ":myserver 421 " + client.getNickname() + " " + command.command + " :Unknown command\r\n");
    return;
  }

  commandMap_[command.command](server, client, command);
  return;
}