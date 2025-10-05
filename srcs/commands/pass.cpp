#include "../../includes/Command.hpp"
#include "../../includes/CommandHandler.hpp"
#include "../../includes/ServerHandler.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/IrcMessage.hpp"
#include "../../includes/CommandUtils.hpp"

bool Command::pass(Server &server, Client &client,
                                 const IrcMessage &command) {
  if (client.isRegistered()) {
    ServerHandler::queueMessage(server, client, ":myserver 462 " + client.getNickname() + " :You may not reregister\r\n");
    return false;
  }

  if (command.params.empty()) {
    ServerHandler::queueMessage(server, client, ":myserver 461 " + client.getNickname() + " PASS :Not enough parameters\r\n");
    return false;
  }

  std::string password = command.params[0];
  if (password != server.getPassword()) {
    ServerHandler::queueMessage(server, client, ":myserver 464 " + client.getNickname() + " :Password incorrect\r\n");
    return false;
  }

  client.setPasswordProvided(true);
  if (!client.isRegistered() &&
      CommandUtils::isClientRegistrationComplete(client)) {
    client.setRegistered(true);
    CommandUtils::sendWelcomeMessage(server, client);
  }
  return true;
}
