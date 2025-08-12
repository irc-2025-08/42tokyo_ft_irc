#pragma once

#include <netinet/in.h>

class Client
{
public:
  enum ClientStatus { CONNECTED, DISCONNECTED };

  Client(int fd, sockaddr_in addr);
  ~Client();
  Client(const Client &other);
  Client &operator=(const Client &other);

  ClientStatus getStatus() const;
  void setStatus(ClientStatus status);

  private:
  int fd_;
  sockaddr_in addr_;
  ClientStatus status_;
};
