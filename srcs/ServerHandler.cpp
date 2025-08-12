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

void ServerHandler::handleAccept(Server &server) {
  while (true) {
    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    errno = 0;
    int client_fd =
        accept(server.socketFd_, (sockaddr *)&client_addr, &client_addr_len);
    if (errno == EAGAIN)
      return;
    if (client_fd < 0) {
      std::cerr << "Warning: Failed to accept client" << std::endl;
      continue;
    }

    if (ServerUtils::setNonblock(client_fd) < 0) {
      std::cerr << "Warning: Failed to set client socket to non-blocking"
                << std::endl;
      close(client_fd);
      continue;
    }

    if (ServerUtils::addEpollEvent(server.epollFd_, client_fd,
                                   EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP) <
        0) {
      std::cerr << "Error: Failed to add client to epoll" << std::endl;
      close(client_fd);
      continue;
    }

    server.clients_.insert(
        std::make_pair(client_fd, Client(client_fd, client_addr)));
    std::cout << "Info: New client connected" << std::endl;
  }
}

// TODO
void ServerHandler::handleRecv(Server &server, Client &client) {
  (void)server;
  char buffer[config::bufferSize];
  memset(buffer, 0, config::bufferSize);

  while (true) {
    errno = 0;
    int bytes_read = recv(client.getFd(), buffer, config::bufferSize, 0);
    if (errno == EAGAIN || bytes_read == 0)
      break;
    if (bytes_read < 0) {
      std::cerr << "Warning: Failed to receive data from client" << std::endl;
      break;
    }

    std::cout << "Info: Received message from client " << client.getFd() << ": "
              << buffer << std::endl;
  }
}

// TODO
void ServerHandler::handleSend(Server &server, Client &client) {
  (void)server;
  std::cout << "Info: Sending message to client " << client.getFd()
            << std::endl;
}

void ServerHandler::handleClose(Server &server, Client &client) {
  std::cout << "Info: Client " << client.getFd() << " disconnected"
            << std::endl;
  server.clients_.erase(client.getFd());
  close(client.getFd());
}