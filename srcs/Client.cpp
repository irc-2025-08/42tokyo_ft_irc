#include "../includes/Client.hpp"
#include <cstring>
#include <sys/epoll.h>

Client::Client() : fd_(-1), nickname_("") {}

Client::Client(int fd, sockaddr_in addr) : fd_(fd), addr_(addr), nickname_("") {}

Client::~Client() {}

Client::Client(const Client &other) : fd_(other.fd_), addr_(other.addr_) {}

Client &Client::operator=(const Client &other) {
  fd_ = other.fd_;
  addr_ = other.addr_;
  nickname_ = other.nickname_;
  return *this;
}

int Client::getFd() const { return fd_; }

std::string Client::getNickname() const { return nickname_; }

void Client::setNickname(const std::string &nickname) { nickname_ = nickname; }