/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:43 by yxu               #+#    #+#             */
/*   Updated: 2025/09/23 19:55:19 by yxu              ###   ########.fr       */
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
   * @brief maximum length of an IRC message (including \r\n)
   * See RFC 2812
  */
  static const int MAX_MSG_LENGTH = 512;
  /**
   * @brief maximum number of parameters in an IRC message
   * See RFC 2812
   */
  static const int MAX_MSG_PARAMS = 15;

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
