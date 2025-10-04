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
    return false;
  } else {
    std::map<int, Client> clientsMap = server.getClientsMap();
    bool targetFound = false;
    
    for (std::map<int, Client>::const_iterator it = clientsMap.begin();
         it != clientsMap.end(); ++it) {
      const Client &targetClient = it->second;
      if (targetClient.getNickname() == target) {
        IrcMessage privmsgMessage = CommandUtils::createIrcMessage(
            ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName(),
            "PRIVMSG",
            target + " :" + message);
        CommandUtils::reply(server, const_cast<Client&>(targetClient), privmsgMessage);
        targetFound = true;
        break;
      }
    }
    
    if (!targetFound) {
      IrcMessage errorMsg = CommandUtils::createIrcMessage(
          ":" + server.getServerName(),
          "401",
          client.getNickname() + " " + target + " :No such nick/channel");
      CommandUtils::reply(server, client, errorMsg);
      return false;
    }
  }

  return true;
}