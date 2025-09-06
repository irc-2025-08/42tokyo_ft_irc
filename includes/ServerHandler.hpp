/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:50 by yxu               #+#    #+#             */
/*   Updated: 2025/09/06 21:55:11 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "Server.hpp"

class ServerHandler {
public:
  /**
   * @brief queue a message, it will be sent to the client in the event loop
   * @param message the message to send
   * @return 0 on success, -1 on error
   * @note if failed, should close the client connection
   */
  static int queueMessage(Server &server, Client &client,
                          const std::string &message);

  /**
   * @brief clear the send buffer and reset epoll event
   * @return 0 on success, -1 on error
   * @note if failed, should close the client connection
   */
  static int onSendComplete(Server &server, Client &client);

  /**
   * @brief handle the accept event
   */
  static void handleAccept(Server &server);

  /**
   * @brief handle the recv event
   */
  static void handleRecv(Server &server, Client &client);

  /**
   * @brief handle the send event
   */
  static void handleSend(Server &server, Client &client);

  /**
   * @brief handle the close event
   */
  static void closeClientConnection(Server &server, Client &client);
};
