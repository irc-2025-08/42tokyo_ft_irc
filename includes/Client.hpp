/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:40 by yxu               #+#    #+#             */
/*   Updated: 2025/09/02 00:18:41 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <netinet/in.h>
#include <string>

class Server;

// TODO
class Client {
public:
  friend class CommandHandler;
  friend class ServerHandler;
  Client();
  Client(int fd, sockaddr_in addr);
  ~Client();
  Client(const Client &other);
  Client &operator=(const Client &other);

  int getFd() const;
  std::string getNickname() const;
  void setNickname(const std::string &nickname);

private:
  int fd_;
  sockaddr_in addr_;
  std::string nickname_;
  std::string recvBuffer_;
  std::string sendBuffer_;
};
