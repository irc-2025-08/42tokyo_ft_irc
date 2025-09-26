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
    return true;
  } else {
    std::map<int, Client> clientsMap = server.getClientsMap();
    
    for (std::map<int, Client>::const_iterator it = clientsMap.begin(); it != clientsMap.end(); ++it) {
      const Client &targetClient = it->second;
      if (targetClient.getNickname() == target) {
        IrcMessage noticeMessage = CommandUtils::createIrcMessage(
            ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName(),
            "NOTICE",
            target + " :" + message);
        CommandUtils::reply(server, const_cast<Client&>(targetClient), noticeMessage);
        break;
      }
    }
  }

  return true;
}
