#pragma once

#include "Server.hpp"
#include "Client.hpp"

class ServerHandler {
public:
  /**
   * @brief queue a message, it will be sent to the client in the event loop
   * @param server the server
   * @param client the client
   * @param message the message to send
   * @return 0 on success, -1 on error
   * @note if failed, should close the client connection
   */
  static int queueMessage(Server &server, Client &client, const std::string &message);

  /**
  * @brief clear the send buffer and reset epoll event
  * @param server the server
  * @param client the client
  * @return 0 on success, -1 on error
  * @note if failed, should close the client connection
  */
  static int onSendComplete(Server &server, Client &client);

  /**
   * @brief handle the accept event
   * @param server the server
   */
  static void handleAccept(Server &server);

  /**
   * @brief handle the recv event
   * @param server the server
   * @param client the client
   */
  static void handleRecv(Server &server, Client &client);

  /**
   * @brief handle the send event
   * @param server the server
   * @param client the client
   */
  static void handleSend(Server &server, Client &client);

  /**
   * @brief handle the close event
   * @param server the server
   * @param client the client
   */
  static void closeClientConnection(Server &server, Client &client);
};