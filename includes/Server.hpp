#pragma once

#include "Client.hpp"
#include <map>
#include <string>

class ServerHandler;

class Server {
public:
  friend class ServerHandler;

  Server(int port, std::string password);
  ~Server();

  enum ServerStatus { STARTING, RUNNING, ERROR };

  // run the server
  void run();
  
  ServerStatus getStatus() const;

private:
  int port_;
  std::string password_;
  ServerStatus serverStatus_;
  int socketFd_;
  int epollFd_;
  std::map<int, Client> clients_;

  // event loop
  void eventLoop();
};
