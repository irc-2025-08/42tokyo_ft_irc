/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yx <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:13 by yxu               #+#    #+#             */
/*   Updated: 2025/09/06 21:37:11 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/CommandUtils.hpp"
#include <algorithm>
#include <cctype>

static bool isValidNickname(const std::string &nickname) {
  if (nickname.empty() || nickname.length() > 9) {
    return false;
  }
  
  if (!std::isalpha(nickname[0]) && nickname[0] != '[' && nickname[0] != ']' &&
      nickname[0] != '{' && nickname[0] != '}' && nickname[0] != '\\' &&
      nickname[0] != '|' && nickname[0] != '_') {
    return false;
  }
  
  for (size_t i = 1; i < nickname.length(); ++i) {
    char c = nickname[i];
    if (!std::isalnum(c) && c != '[' && c != ']' && c != '{' && c != '}' &&
        c != '\\' && c != '|' && c != '_' && c != '-') {
      return false;
    }
  }
  
  return true;
}

static bool isNicknameInUse(Server &server, const std::string &nickname, int excludeFd) {
  std::map<int, Client> clients = server.getClientsMap();
  for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
    if (it->first != excludeFd && it->second.getNickname() == nickname) {
      return true;
    }
  }
  return false;
}

static void notifyNicknameChange(Server &server, Client &client, const std::string &oldNick, const std::string &newNick) {
  IrcMessage notification;
  notification.prefix = oldNick + "!" + client.getUsername() + "@localhost";
  notification.command = "NICK";
  notification.params.push_back(newNick);
  
  std::map<int, Client> clients = server.getClientsMap();
  for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
    if (it->first != client.getFd()) {
      CommandUtils::reply(server, const_cast<Client&>(it->second), notification);
    }
  }
}

bool Command::nick(Server &server, Client &client, const IrcMessage &command) {
  if (command.params.size() == 0) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "431",
        client.getNickname() + " :No nickname given");
    CommandUtils::reply(server, client, msg);
    return true;
  }

  const std::string &newNickname = command.params[0];
  
  if (!isValidNickname(newNickname)) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "432",
        client.getNickname() + " " + newNickname + " :Erroneous nickname");
    CommandUtils::reply(server, client, msg);
    return true;
  }
  
  if (isNicknameInUse(server, newNickname, client.getFd())) {
    IrcMessage msg = CommandUtils::createIrcMessage(
        server.getServerName(), "433",
        client.getNickname() + " " + newNickname + " :Nickname is already in use");
    CommandUtils::reply(server, client, msg);
    return true;
  }
  
  std::string oldNickname = client.getNickname();
  client.setNickname(newNickname);
  
  if (!oldNickname.empty() && oldNickname != newNickname) {
    notifyNicknameChange(server, client, oldNickname, newNickname);
  }
  
  // Check if registration is now complete
  if (!client.isRegistered() && 
      client.isPasswordProvided() && 
      client.getNickname() != "*" && 
      !client.getUsername().empty()) {
    client.setRegistered(true);
    CommandUtils::sendWelcomeMessage(server, client);
  }
  
  return true;
}
