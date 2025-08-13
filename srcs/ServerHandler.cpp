#include "../includes/ServerHandler.hpp"
#include "../includes/ServerUtils.hpp"
#include "../includes/config.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

namespace {
  // TODO
  void parseAndProcess(Server &server, Client &client) {
    std::cout << "[INFO] ircd: Received message from client " << client.getFd()
    << ": " << client.recvBuffer_ << std::endl;
    client.recvBuffer_.clear();
    (void)server;
    // std::string::size_type pos = client.recvBuffer_.find("\r\n");
    // if (pos != std::string::npos) {
    //   std::string command = client.recvBuffer_.substr(0, pos);
    //   client.recvBuffer_.erase(0, pos + 2);
    //   processCommand(server, client, command);
    // }
  }
} // namespace

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
    std::cout << "[INFO] ircd: New client connected" << std::endl;
  }
}

void ServerHandler::handleRecv(Server &server, Client &client) {
  char buffer[config::ioBufferSize];

  while (true) {
    errno = 0;
    int bytesRead =
        recv(client.getFd(), buffer, config::ioBufferSize, 0);

    if (errno == EAGAIN)
      break;
    if (bytesRead == 0) {
      handleClose(server, client);
      return;
    }
    if (bytesRead < 0) {
      std::cerr << "[WARN] ircd: Failed to receive data from client"
                << std::endl;
      break;
    }

    client.recvBuffer_.append(buffer, bytesRead);
  }

  parseAndProcess(server, client);
}

// TODO
void ServerHandler::handleSend(Server &server, Client &client) {
  (void)server;
  std::cout << "[INFO] ircd: Sending message to client " << client.getFd()
            << std::endl;
}

void ServerHandler::handleClose(Server &server, Client &client) {
  server.clients_.erase(client.getFd());
  close(client.getFd());
  std::cout << "[INFO] ircd: Client " << client.getFd() << " disconnected"
            << std::endl;
}