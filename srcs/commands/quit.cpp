#include "../../includes/Command.hpp"
#include "../../includes/Server.hpp"

static std::string extractQuitReason(const IrcMessage &command) {
  if (command.params.empty()) {
    return "Client Quit";
  }

  const std::string &lastParam = command.params.back();
  if (lastParam.empty()) {
    return "Client Quit";
  }

  return lastParam;
}

bool Command::quit(Server &server, Client &client,
                   const IrcMessage &command) {
  std::string reason = extractQuitReason(command);

  server.handleClientQuit(client, reason);
  return true;
}

