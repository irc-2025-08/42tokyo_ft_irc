#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"

bool Command::notice(Server &server, Client &client, const IrcMessage &command) {
  if (command.params.size() < 2) {
    return true;
  }

  std::string target = command.params[0];
  std::string message = command.params[1];
  if (target.empty() || message.empty()) {
    return true;
  }

  if (target[0] == '#') {
    Channel *channel = server.findChannel(target);
    if (channel == NULL) {
      return true;
    }

    if (!channel->hasMember(client.getNickname())) {
      return true;
    }

    std::vector<std::string> members = channel->getMembers();
    IrcMessage noticeMessage = CommandUtils::createIrcMessage(
        client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName(),
        "NOTICE",
        target + " :" + message);

    for (std::vector<std::string>::iterator it = members.begin(); it != members.end(); ++it) {
      if (*it == client.getNickname()) {
        continue;
      }

      Client *memberClient = server.findClientByNickname(*it);
      if (memberClient != NULL) {
        CommandUtils::reply(server, *memberClient, noticeMessage);
      }
    }
  } else {
    Client *targetClient = server.findClientByNickname(target);
    if (targetClient == NULL) {
      return true;
    }

    IrcMessage noticeMessage = CommandUtils::createIrcMessage(
        client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName(),
        "NOTICE",
        target + " :" + message);
    CommandUtils::reply(server, *targetClient, noticeMessage);
  }

  return true;
}
