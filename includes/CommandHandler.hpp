/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:43 by yxu               #+#    #+#             */
/*   Updated: 2025/09/05 23:24:22 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "IrcMessage.hpp"
#include <map>
#include <string>

class Server;
class Client;

class CommandHandler {
public:
  /**
   * @brief parse the command and process it
   * @param server the server
   * @param client the client
   */
  static void parseAndProcessCommand(Server &server, Client &client);

  /**
   * @brief initialize the command map, should be called before running the
   * server
   */
  static void initCommandMap();

  /**
   * @brief reply to the client
   */
  static int reply(Server &server, Client &client, const IrcMessage &message);

  static IrcMessage createIrcMessage(const std::string &prefix, const std::string &command, const std::string &paramsStr);
  static IrcMessage createIrcMessage(const std::string &command, const std::string &paramsStr);

private:
  static std::map<std::string, bool (*)(Server &, Client &, const IrcMessage &)>
      commandMap_;

  static IrcMessage parseCommandLine(const std::string &cmdLine);

  static bool isIrcMessageValid(const IrcMessage &message);

  static void processCommand(Server &server, Client &client,
                             const IrcMessage &command);

  static bool pingCmd(Server &server, Client &client,
                      const IrcMessage &command);
  static bool nickCmd(Server &server, Client &client,
                      const IrcMessage &command);
  static bool capCmd(Server &server, Client &client,
                      const IrcMessage &command);
  static bool userCmd(Server &server, Client &client,
                      const IrcMessage &command);
};
