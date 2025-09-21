#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"

bool Command::motd(Server &server, Client &client, const IrcMessage &command) {
  (void)command;

  IrcMessage startMsg = CommandUtils::createIrcMessage(
      server.getServerName(), "375",
      client.getNickname() + " :- " + server.getServerName() + " Message of the day - ");
  CommandUtils::reply(server, client, startMsg);

  IrcMessage motdMsg = CommandUtils::createIrcMessage(
      server.getServerName(), "372",
      client.getNickname() + " :- Welcome to 42tokyo ft_irc server!");
  CommandUtils::reply(server, client, motdMsg);

  IrcMessage motdMsg2 = CommandUtils::createIrcMessage(
      server.getServerName(), "372",
      client.getNickname() + " :- This is a simple IRC server implementation.");
  CommandUtils::reply(server, client, motdMsg2);

  IrcMessage endMsg = CommandUtils::createIrcMessage(
      server.getServerName(), "376",
      client.getNickname() + " :End of /MOTD command.");
  CommandUtils::reply(server, client, endMsg);

  return true;
}