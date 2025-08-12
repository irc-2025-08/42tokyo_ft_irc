#pragma once

#include "Server.hpp"
#include "Client.hpp"

class ServerHandler {
public:
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
  static void handleClose(Server &server, Client &client);
};