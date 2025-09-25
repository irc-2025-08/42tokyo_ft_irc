/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:21 by yxu               #+#    #+#             */
/*   Updated: 2025/09/05 23:28:04 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"
#include <cstring>

Client::Client() : fd_(-1), nickname_("*"), username_(""), registered_(false), passwordProvided_(false) {}

Client::Client(int fd, sockaddr_in addr)
    : fd_(fd), addr_(addr), nickname_("*"), username_(""), registered_(false), passwordProvided_(false) {}

Client::~Client() {}

Client::Client(const Client &other)
    : fd_(other.fd_), addr_(other.addr_), nickname_(other.nickname_),
      username_(other.username_), registered_(other.registered_), passwordProvided_(other.passwordProvided_),
      recvBuffer_(other.recvBuffer_), sendBuffer_(other.sendBuffer_) {}

Client &Client::operator=(const Client &other) {
  fd_ = other.fd_;
  addr_ = other.addr_;
  nickname_ = other.nickname_;
  username_ = other.username_;
  registered_ = other.registered_;
  passwordProvided_ = other.passwordProvided_;
  recvBuffer_ = other.recvBuffer_;
  sendBuffer_ = other.sendBuffer_;
  return *this;
}

int Client::getFd() const { return fd_; }

std::string Client::getNickname() const { return nickname_; }

void Client::setNickname(const std::string &nickname) { nickname_ = nickname; }

std::string Client::getUsername() const { return username_; }

void Client::setUsername(const std::string &username) { username_ = username; }

bool Client::isRegistered() const { return registered_; }

bool Client::isPasswordProvided() const { return passwordProvided_; }

void Client::setPasswordProvided(bool provided) { passwordProvided_ = provided; }
