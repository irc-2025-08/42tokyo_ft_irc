#pragma once

#include "Client.hpp"
#include <map>
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
  int socketFd_;
  int epollFd_;
  std::map<int, Client> clients_;

  // initialize the socket
  void initSocket();

  // handle the accept event
  void handleAccept();

  // handle the recv event
  void handleRecv(Client &client);

  // handle the send event
  void handleSend(Client &client);

  // handle the close event
  void handleClose(Client &client);

  // event loop
  void eventLoop();

  // add an event to the epoll
  int addEpollEvent(int fd, int events);
};
