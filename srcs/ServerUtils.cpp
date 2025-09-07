/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:35 by yxu               #+#    #+#             */
/*   Updated: 2025/09/06 21:54:56 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerUtils.hpp"
#include "../includes/config.hpp"
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

int ServerUtils::setNonblock(const int fd) {
  // get the current flags and add O_NONBLOCK flag to the current flags
  int flags = fcntl(fd, F_GETFL, 0);
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int ServerUtils::addEpollEvent(int epollFd, int fd, int events) {
  // create an epoll event struct and add it to the epoll
  epoll_event ev;
  ev.data.fd = fd;
  ev.events = events;
  return epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev);
}

int ServerUtils::createSocket(int port) {
  // create a IPv4 TCP socket
  int socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketFd < 0)
    return -1;

  // set the socket address struct
  sockaddr_in addr;
  addr.sin_family = AF_INET; // IPv4
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  // bind the socket to the port
  if (bind(socketFd, (sockaddr *)&addr, sizeof(addr)) < 0) {
    close(socketFd);
    return -1;
  }

  // listen for incoming connections
  if (listen(socketFd, config::backlog) < 0) {
    close(socketFd);
    return -1;
  }

  return socketFd;
}
