#include "../includes/Client.hpp"
#include <cstring>

Client::Client() : fd_(-1) {}

Client::Client(int fd, sockaddr_in addr) : fd_(fd), addr_(addr) {}

Client::~Client() {}

Client::Client(const Client &other) : fd_(other.fd_), addr_(other.addr_) {}

Client &Client::operator=(const Client &other) {
  fd_ = other.fd_;
  addr_ = other.addr_;
  return *this;
}

int Client::getFd() const { return fd_; }

int Client::sendMessage(Server &server, const std::string &message) {
  (void)server;
  (void)message;
  return 0;
}
