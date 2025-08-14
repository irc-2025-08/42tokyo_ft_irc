#include "../includes/CommandHandler.hpp"
#include "../includes/ServerHandler.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include <iostream>

// TODO
void CommandHandler::parseAndProcessCommand(Server &server, Client &client) {
  // std::string::size_type pos = client.recvBuffer_.find("\r\n");
  // if (pos != std::string::npos) {
  //   std::string command = client.recvBuffer_.substr(0, pos);
  //   client.recvBuffer_.erase(0, pos + 2);
  //   processCommand(server, client, command);
  // }

  // test code
  std::cout << "debug: Received message from client " << client.getFd()
            << ":\n" << client.recvBuffer_ << std::endl;
  if (client.recvBuffer_.find("CAP") != std::string::npos)
    ServerHandler::queueMessage(server, client, ":myserver CAP yxu LS :\r\n");

  if (client.recvBuffer_.find("JOIN") != std::string::npos)
    ServerHandler::queueMessage(server, client, ":myserver 001 yxu :Welcome, yxu!\r\n");

  client.recvBuffer_.clear();
}