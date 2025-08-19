#pragma once

#include <netinet/in.h>
#include <string>

class Server;

// TODO
class Client {
public:
  Client();
  Client(int fd, sockaddr_in addr);
  ~Client();
  Client(const Client &other);
  Client &operator=(const Client &other);

  std::string recvBuffer_;
  std::string sendBuffer_;

  int getFd() const;
  std::string getNickname() const;
  void setNickname(const std::string &nickname);

private:
  int fd_;
  sockaddr_in addr_;
  std::string nickname_;
};
