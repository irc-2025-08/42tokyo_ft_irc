/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:40 by yxu               #+#    #+#             */
/*   Updated: 2025/09/06 21:22:28 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <netinet/in.h>
#include <string>

class Server;

class Client {
public:
  friend class CommandHandler;
  friend class ServerHandler;
  Client();
  Client(int fd, sockaddr_in addr);
  ~Client();

  int getFd() const;
  std::string getNickname() const;
  void setNickname(const std::string &nickname);
  std::string getUsername() const;
  void setUsername(const std::string &username);
  bool isRegistered() const;
  bool isPasswordProvided() const;
  void setPasswordProvided(bool provided);
  Client(const Client &other);
  Client &operator=(const Client &other);

private:
  int fd_;
  sockaddr_in addr_;
  std::string nickname_;
  std::string username_;
  bool registered_;
  bool passwordProvided_;
  std::string recvBuffer_;
  std::string sendBuffer_;
};
