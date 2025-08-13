#pragma once

#include "Client.hpp"
#include <map>
#include <string>

class ServerHandler;

class Server {
public:
  friend class ServerHandler;
  enum ServerStatus { SERV_STOPPED, SERV_RUNNING, SERV_ERROR };

  Server(int port, std::string password);
  ~Server();

  /**
   * @brief run the server
   */
  void run();
  ServerStatus getStatus() const;
  void setStatus(ServerStatus status);

private:
  int port_;
  std::string password_;
  ServerStatus serverStatus_;
  int socketFd_;
  int epollFd_;
  std::map<int, Client> clients_;

  /**
   * @brief event loop
   */
  void eventLoop();
};
