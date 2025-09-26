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

Client::Client() : fd_(-1), nickname_("*"), username_(""), realname_(""), registered_(false), passwordProvided_(false), connected_(false), operator_(false) {}

Client::Client(int fd, sockaddr_in addr)
    : fd_(fd), addr_(addr), nickname_("*"), username_(""), realname_(""), registered_(false), passwordProvided_(false), connected_(false), operator_(false) {}

Client::~Client() {}

Client::Client(const Client &other)
    : fd_(other.fd_), addr_(other.addr_), nickname_(other.nickname_),
      username_(other.username_), realname_(other.realname_), registered_(other.registered_), passwordProvided_(other.passwordProvided_),
      connected_(other.connected_), operator_(other.operator_), recvBuffer_(other.recvBuffer_), sendBuffer_(other.sendBuffer_) {}

Client &Client::operator=(const Client &other) {
  fd_ = other.fd_;
  addr_ = other.addr_;
  nickname_ = other.nickname_;
  username_ = other.username_;
  realname_ = other.realname_;
  registered_ = other.registered_;
  passwordProvided_ = other.passwordProvided_;
  connected_ = other.connected_;
  operator_ = other.operator_;
  recvBuffer_ = other.recvBuffer_;
  sendBuffer_ = other.sendBuffer_;
  return *this;
}

int Client::getFd() const { return fd_; }

std::string Client::getNickname() const { return nickname_; }

void Client::setNickname(const std::string &nickname) { nickname_ = nickname; }

std::string Client::getUsername() const { return username_; }

void Client::setUsername(const std::string &username) { username_ = username; }

bool Client::isConnected() const { return connected_; }

void Client::setConnected(bool connected) { connected_ = connected; }
	
bool Client::isRegistered() const { return registered_; }

bool Client::isPasswordProvided() const { return passwordProvided_; }

void Client::setPasswordProvided(bool provided) { passwordProvided_ = provided; }

std::string Client::getRealname() const { return realname_; }

void Client::setRealname(const std::string &realname) { realname_ = realname; }

void Client::setRegistered(bool registered) { registered_ = registered; }

bool Client::isOperator() const { return operator_; }

void Client::setOperator(bool operator_status) { operator_ = operator_status; }
