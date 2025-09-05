#include "../includes/CommandUtils.hpp"
#include "../includes/Client.hpp"
#include "../includes/CommandHandler.hpp"
#include "../includes/Server.hpp"

int CommandUtils::sendWelcomeMessage(Server &server, Client &client) {
  IrcMessage msg = CommandHandler::createIrcMessage(
      server.getServerName(), "001",
      client.getNickname() + " :Welcome to the IRC server," +
          client.getNickname());
  CommandHandler::reply(server, client, msg);
  return 0;
}
