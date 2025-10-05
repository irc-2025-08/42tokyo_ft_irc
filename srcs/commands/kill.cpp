#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"
#include "../../includes/ServerHandler.hpp"

static void sendKillAcknowledgement(Server &server, Client &client,
                                    const std::string &targetNickname,
                                    const std::string &reason) {
  std::string noticeParams = client.getNickname() +
                             " :KILL " + targetNickname + " (" + reason + ")";
  IrcMessage noticeMsg = CommandUtils::createIrcMessage(
      server.getServerName(), "NOTICE", noticeParams);
  CommandUtils::reply(server, client, noticeMsg);
}

static std::string buildSourcePrefix(const Server &server, const Client &client) {
  std::string prefix = client.getNickname();
  if (!client.getUsername().empty()) {
    prefix += "!" + client.getUsername();
  }
  prefix += "@" + server.getServerName();
  return prefix;
}

static void notifyTargetOfKill(Server &server, Client &executor,
                               Client &target, const std::string &reason) {
  std::string killParams = target.getNickname() + " :" + reason;
  IrcMessage killMsg = CommandUtils::createIrcMessage(
      buildSourcePrefix(server, executor), "KILL", killParams);
  CommandUtils::reply(server, target, killMsg);

  std::string errorReason = "Killed by " + executor.getNickname();
  if (!reason.empty()) {
    errorReason += " (" + reason + ")";
  }
  IrcMessage errorMsg = CommandUtils::createIrcMessage(
      server.getServerName(), "ERROR", ":" + errorReason);
  CommandUtils::reply(server, target, errorMsg);

  ServerHandler::handleSend(server, target);
}

bool Command::kill(Server &server, Client &client, const IrcMessage &command) {
  if (!client.isRegistered()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "451",
        client.getNickname() + " :You have not registered");
    CommandUtils::reply(server, client, msg);
    return true;
  }

  if (!client.isOperator()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "481",
        client.getNickname() +
            " :Permission Denied- You're not an IRC operator");
    CommandUtils::reply(server, client, msg);
    return true;
  }

  if (command.params.size() < 2) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "461",
        client.getNickname() + " KILL :Not enough parameters");
    CommandUtils::reply(server, client, msg);
    return true;
  }

  const std::string &targetNickname = command.params[0];
  Client *targetClient = server.findClientByNickname(targetNickname);

  if (targetClient == NULL) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "401",
        client.getNickname() + " " + targetNickname +
            " :No such nick/channel");
    CommandUtils::reply(server, client, msg);
    return true;
  }

  std::string reason = command.params.back();
  if (reason.empty()) {
    reason = "Killed";
  }

  sendKillAcknowledgement(server, client, targetNickname, reason);
  notifyTargetOfKill(server, client, *targetClient, reason);
  return true;
}

