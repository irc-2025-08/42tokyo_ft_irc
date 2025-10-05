#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"

bool Command::privmsg(Server &server, Client &client, const IrcMessage &command) {
  if (command.params.size() < 2) {
    IrcMessage errorMsg = CommandUtils::createIrcMessage(
        ":" + server.getServerName(),
        "461",
        client.getNickname() + " PRIVMSG :Not enough parameters");
    CommandUtils::reply(server, client, errorMsg);
    return false;
  }

  std::string target = command.params[0];
  std::string message = command.params[1];

  if (target.empty()) {
    IrcMessage errorMsg = CommandUtils::createIrcMessage(
        ":" + server.getServerName(),
        "411",
        client.getNickname() + " :No recipient given (PRIVMSG)");
    CommandUtils::reply(server, client, errorMsg);
    return false;
  }

  if (message.empty()) {
    IrcMessage errorMsg = CommandUtils::createIrcMessage(
        ":" + server.getServerName(),
        "412",
        client.getNickname() + " :No text to send");
    CommandUtils::reply(server, client, errorMsg);
    return false;
  }

  if (target[0] == '#') {
    Channel *channel = server.findChannel(target);
    if (channel == NULL) {
      IrcMessage errorMsg = CommandUtils::createIrcMessage(
          ":" + server.getServerName(),
          "403",
          client.getNickname() + " " + target + " :No such channel");
      CommandUtils::reply(server, client, errorMsg);
      return false;
    }

    if (!channel->hasMember(client.getNickname())) {
      IrcMessage errorMsg = CommandUtils::createIrcMessage(
          ":" + server.getServerName(),
          "442",
          client.getNickname() + " " + target + " :You're not on that channel");
      CommandUtils::reply(server, client, errorMsg);
      return false;
    }

    std::vector<std::string> members = channel->getMembers();
    IrcMessage privmsgMessage = CommandUtils::createIrcMessage(
        client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName(),
        "PRIVMSG",
        target + " :" + message);

    for (size_t i = 0; i < members.size(); ++i) {
      Client *memberClient = server.findClientByNickname(members[i]);
      if (memberClient != NULL && memberClient->getNickname() != client.getNickname()) {
        CommandUtils::reply(server, *memberClient, privmsgMessage);
      }
    }
  } else {
    Client *targetClient = server.findClientByNickname(target);
    if (targetClient == NULL) {
      IrcMessage errorMsg = CommandUtils::createIrcMessage(
          ":" + server.getServerName(),
          "401",
          client.getNickname() + " " + target + " :No such nick/channel");
      CommandUtils::reply(server, client, errorMsg);
      return false;
    }

    IrcMessage privmsgMessage = CommandUtils::createIrcMessage(
        client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName(),
        "PRIVMSG",
        target + " :" + message);
    CommandUtils::reply(server, *targetClient, privmsgMessage);
  }

  return true;
}