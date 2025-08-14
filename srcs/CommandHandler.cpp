#include "../includes/CommandHandler.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include <iostream>

void CommandHandler::parseAndProcessCommand(Server &server, Client &client) {
  std::cout << "[INFO] ircd: Received message from client " << client.getFd()
            << ": " << client.recvBuffer_ << std::endl;
  client.recvBuffer_.clear();
  (void)server;
  // std::string::size_type pos = client.recvBuffer_.find("\r\n");
  // if (pos != std::string::npos) {
  //   std::string command = client.recvBuffer_.substr(0, pos);
  //   client.recvBuffer_.erase(0, pos + 2);
  //   processCommand(server, client, command);
  // }
}