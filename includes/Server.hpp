#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include <map>
#include <string>
#include <vector>

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
  
  // Channel management methods
  Channel* findChannel(const std::string& channelName);
  void addChannel(const std::string& channelName);

private:
  int port_;
  std::string password_;
  ServerStatus serverStatus_;
  int socketFd_;
  int epollFd_;
  std::map<int, Client> clients_;
  std::vector<Channel> channels_;

  /**
   * @brief event loop
   */
  void eventLoop();
};
