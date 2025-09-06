/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:43 by yxu               #+#    #+#             */
/*   Updated: 2025/09/06 22:24:57 by yxu              ###   ########.fr       */
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
   * @brief initialize commands, should be called before running the server
   */
  static void initCommandMap();

  /**
   * @brief parse the command and process it
   */
  static void parseAndProcessCommand(Server &server, Client &client);

  /**
   * @brief parse one command line string without \r\n
   */
  static IrcMessage parseCommandLine(const std::string &cmdLine);

private:
  static std::map<std::string, bool (*)(Server &, Client &, const IrcMessage &)>
      commandMap_;

  static void processCommand(Server &server, Client &client,
                             const IrcMessage &command);
};
