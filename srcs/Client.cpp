#include "../includes/Client.hpp"

Client::Client() : fd_(-1), status_(DISCONNECTED) {}

Client::Client(int fd, sockaddr_in addr)
    : fd_(fd), addr_(addr), status_(CONNECTED) {}

Client::~Client() {}

Client::Client(const Client &other)
    : fd_(other.fd_), addr_(other.addr_), status_(other.status_) {}

Client &Client::operator=(const Client &other) {
  fd_ = other.fd_;
  addr_ = other.addr_;
  status_ = other.status_;
  return *this;
}

Client::ClientStatus Client::getStatus() const { return status_; }

void Client::setStatus(ClientStatus status) { status_ = status; }

int Client::getFd() const { return fd_; }