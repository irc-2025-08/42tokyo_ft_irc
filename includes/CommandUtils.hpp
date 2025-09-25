/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandUtils.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/06 21:50:14 by yxu               #+#    #+#             */
/*   Updated: 2025/09/23 20:49:22 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Server;
class Client;
class IrcMessage;

class CommandUtils {
public:
  /**
   * @brief reply an irc message to the client
   */
  static int reply(Server &server, Client &client, const IrcMessage &message);

  /**
   * @brief create an irc message from string
   */
  static IrcMessage createIrcMessage(const std::string &prefix,
                                     const std::string &command,
                                     const std::string &paramsStr);
  static IrcMessage createIrcMessage(const std::string &command,
                                     const std::string &paramsStr);

  /**
   * @brief send welcome message(001~004) to the client
   */
  static int sendWelcomeMessage(Server &server, Client &client);
};
