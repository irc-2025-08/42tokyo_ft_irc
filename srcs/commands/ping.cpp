#include "../../includes/CommandHandler.hpp"
#include "../../includes/ServerHandler.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"

// TODO
bool CommandHandler::pingCmd(Server &server, Client &client,
                                 const IrcCommand &command) {
  (void)command;
  ServerHandler::queueMessage(server, client, ":myserver PONG myserver :" + client.getNickname() + "\r\n");
  return true;
}