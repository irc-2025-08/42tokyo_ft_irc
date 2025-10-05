#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"

bool Command::oper(Server &server, Client &client, const IrcMessage &command) {
  if (!client.isRegistered()) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "451",
        client.getNickname() + " :You have not registered");
    CommandUtils::reply(server, client, msg);
    return true;
  }

  if (command.params.size() < 2) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "461",
        client.getNickname() + " OPER :Not enough parameters");
    CommandUtils::reply(server, client, msg);
    return true;
  }

  const std::string &name = command.params[0];
  const std::string &password = command.params[1];
  
  if (name == "admin" && password == "adminpass") {
    client.setOperator(true);
    
    IrcMessage operMsg = CommandUtils::createIrcMessage(
        server.getServerName(), "381",
        client.getNickname() + " :You are now an IRC operator");
    CommandUtils::reply(server, client, operMsg);
    
    IrcMessage modeMsg = CommandUtils::createIrcMessage(
        server.getServerName(), "MODE",
        client.getNickname() + " +o");
    CommandUtils::reply(server, client, modeMsg);
  } else {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "464",
        client.getNickname() + " :Password incorrect");
    CommandUtils::reply(server, client, msg);
  }
  
  return true;
}
