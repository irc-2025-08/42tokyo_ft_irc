/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:32 by yxu               #+#    #+#             */
/*   Updated: 2025/09/02 00:18:33 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerHandler.hpp"
#include "../includes/CommandHandler.hpp"
#include "../includes/ServerUtils.hpp"
#include "../includes/config.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

int ServerHandler::queueMessage(Server &server, Client &client,
                                const std::string &message) {
  // add EPOLLOUT event
  epoll_event event;
  event.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP | EPOLLOUT;
  event.data.fd = client.getFd();

  client.sendBuffer_.append(message);
  if (epoll_ctl(server.epollFd_, EPOLL_CTL_MOD, client.getFd(), &event) < 0) {
    std::cerr << "[WARN] ircd: epoll_ctl failed with client " << client.getFd()
              << std::endl;
    return -1;
  }
  return 0;
}

int ServerHandler::onSendComplete(Server &server, Client &client) {
  // remove EPOLLOUT event
  epoll_event event;
  event.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
  event.data.fd = client.getFd();

  client.sendBuffer_.clear();
  if (epoll_ctl(server.epollFd_, EPOLL_CTL_MOD, client.getFd(), &event) < 0) {
    std::cerr << "[WARN] ircd: epoll_ctl failed with client " << client.getFd()
              << std::endl;
    return -1;
  }
  return 0;
}

void ServerHandler::handleAccept(Server &server) {
  while (true) {
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    errno = 0;
    int clientFd =
        accept(server.socketFd_, (sockaddr *)&clientAddr, &clientAddrLen);
    if (errno == EAGAIN)
      return;
    if (clientFd < 0) {
      std::cerr << "[WARN] ircd: Failed to accept client" << std::endl;
      continue;
    }

    if (ServerUtils::setNonblock(clientFd) < 0) {
      std::cerr << "[WARN] ircd: Failed to set client socket to non-blocking"
                << std::endl;
      close(clientFd);
      continue;
    }

    if (ServerUtils::addEpollEvent(server.epollFd_, clientFd,
                                   EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP) <
        0) {
      std::cerr << "[WARN] ircd: Failed to add client to epoll" << std::endl;
      close(clientFd);
      continue;
    }

    server.clients_.insert(
        std::make_pair(clientFd, Client(clientFd, clientAddr)));
    std::cout << "[INFO] ircd: New client connected (fd: " << clientFd << ")"
              << std::endl;
  }
}

void ServerHandler::handleRecv(Server &server, Client &client) {
  char buffer[config::ioBufferSize];

  while (true) {
    errno = 0;
    int bytesRead = recv(client.getFd(), buffer, config::ioBufferSize, 0);

    if (errno == EAGAIN)
      break;
    if (bytesRead == 0) {
      closeClientConnection(server, client);
      return;
    }
    if (bytesRead < 0) {
      std::cerr << "[WARN] ircd: Failed to receive data from client"
                << std::endl;
      break;
    }

    client.recvBuffer_.append(buffer, bytesRead);
  }

  CommandHandler::parseAndProcessCommand(server, client);
}

void ServerHandler::handleSend(Server &server, Client &client) {
  if (send(client.getFd(), client.sendBuffer_.c_str(),
           client.sendBuffer_.size(), 0) < 0)
    std::cerr << "[WARN] ircd: Failed to send data to client" << std::endl;

  std::cout << "debug: Sent message to client " << client.getFd() << ":\n"
            << client.sendBuffer_ << std::endl;

  if (onSendComplete(server, client) < 0)
    closeClientConnection(server, client);
}

void ServerHandler::closeClientConnection(Server &server, Client &client) {
  server.clients_.erase(client.getFd());
  close(client.getFd());
  std::cout << "[INFO] ircd: Client " << client.getFd() << " disconnected"
            << std::endl;
}
