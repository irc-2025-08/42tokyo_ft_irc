#pragma once

#include "Fd.hpp"
#include <string>

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
  Fd socket_;

  // initialize the socket
  void initSocket();

  void eventLoop();
};
