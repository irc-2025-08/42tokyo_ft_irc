#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"

bool Command::quit(Server &server, Client &client, const IrcMessage &command) {
  std::string quitMessage = "Goodbye";
  
  if (!command.params.empty()) {
    quitMessage = command.params[0];
  }

  IrcMessage msg = CommandUtils::createIrcMessage(
      ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName(),
      "QUIT", ":" + quitMessage);
  CommandUtils::reply(server, client, msg);

  client.setConnected(false);
  return true;
}
