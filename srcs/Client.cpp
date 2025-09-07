#include "../includes/Client.hpp"
#include <cstring>

Client::Client() : fd_(-1), nickname_(""), registered_(false), passwordProvided_(false) {}

Client::Client(int fd, sockaddr_in addr) : fd_(fd), addr_(addr), nickname_(""), registered_(false), passwordProvided_(false) {}

Client::~Client() {}

Client::Client(const Client &other) : fd_(other.fd_), addr_(other.addr_), nickname_(other.nickname_), registered_(other.registered_), passwordProvided_(other.passwordProvided_) {}

Client &Client::operator=(const Client &other) {
  fd_ = other.fd_;
  addr_ = other.addr_;
  nickname_ = other.nickname_;
  registered_ = other.registered_;
  passwordProvided_ = other.passwordProvided_;
  return *this;
}

int Client::getFd() const { return fd_; }

std::string Client::getNickname() const { return nickname_; }

void Client::setNickname(const std::string &nickname) { nickname_ = nickname; }

bool Client::isRegistered() const { return registered_; }

bool Client::isPasswordProvided() const { return passwordProvided_; }

void Client::setPasswordProvided(bool provided) { passwordProvided_ = provided; }