#pragma once

#include "Client.hpp"
#include <string>
#include <map>

class Server {
public:
  enum ServerStatus { STARTING, RUNNING, ERROR };

  Server(int port, std::string password);
  ~Server();

  // run the server
  void run();

  ServerStatus getStatus() const;

private:
  int port_;
  std::string password_;
  ServerStatus serverStatus_;
  int socket_;
  std::map<int, Client> clients_;

  // initialize the socket
  void initSocket();

  // handle the accept event
  void handleAccept();

  // handle the recv event
  void handleRecv();

  // event loop
  void eventLoop();
};
