#include "../includes/Server.hpp"
#include "../includes/ServerHandler.hpp"
#include "../includes/ServerUtils.hpp"
#include "../includes/config.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(int port, std::string password)
    : port_(port), password_(password), serverStatus_(SERV_STOPPED) {
  std::cout << "[INFO] ircd: Initializing server..." << std::endl;

  // create a socket and bind it to the port
  socketFd_ = ServerUtils::createSocket(port_);
  if (socketFd_ < 0)
    throw std::runtime_error("Failed to create socket");

  // set the socket to non-blocking
  if (ServerUtils::setNonblock(socketFd_) < 0)
    throw std::runtime_error("Failed to set socket to non-blocking");

  // create an epoll instance
  epollFd_ = epoll_create1(0);
  if (epollFd_ < 0)
    throw std::runtime_error("Failed to create epoll instance");

  // add the socket to the epoll instance
  if (ServerUtils::addEpollEvent(epollFd_, socketFd_, EPOLLIN) < 0)
    throw std::runtime_error("Failed to add socket to epoll");

  std::cout << "[INFO] ircd: Server initialized successfully" << std::endl;
}

Server::~Server() {
  // close the socket and all clients fds
  close(socketFd_);
  for (std::map<int, Client>::iterator it = clients_.begin();
       it != clients_.end(); ++it) {
    close(it->first);
  }
}

void Server::setStatus(ServerStatus status) {
  if (getStatus() == SERV_ERROR)
    return;
  serverStatus_ = status;
}

Server::ServerStatus Server::getStatus() const { return serverStatus_; }

void Server::run() {
  if (getStatus() == SERV_ERROR)
    return;

  setStatus(SERV_RUNNING);
  std::cout << "[INFO] ircd: IRC server running on port " << port_
            << " with password " << password_ << std::endl;

  eventLoop();
}

void Server::eventLoop() {
  epoll_event events[config::maxEvents];

  while (getStatus() == SERV_RUNNING) {
    // wait for events
    int nfds = epoll_wait(epollFd_, events, config::maxEvents, -1);

    if (nfds < 0) {
      std::cerr << "[WARN] ircd: Failed to wait for events" << std::endl;
      continue;
    }

    // handle events
    for (int i = 0; i < nfds; i++) {
      // handle accept event if the ready fd is the socket fd
      if (events[i].data.fd == socketFd_) {
        ServerHandler::handleAccept(*this);
        continue;
      }

      // if the ready fd is not the socket fd, it must be a client fd
      // get the ready client
      Client &client = clients_[events[i].data.fd];

      // handle client events
      if (events[i].events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
        ServerHandler::closeClientConnection(*this, client);
        continue;
      }
      if (events[i].events & EPOLLIN)
        ServerHandler::handleRecv(*this, client);
      if (events[i].events & EPOLLOUT)
        ServerHandler::handleSend(*this, client);
    }
  }
}

// Channel management methods
Channel* Server::findChannel(const std::string& channelName) {
  for (auto& channel : channels_) {
    if (channel.getName() == channelName) {
      return &channel;
    }
  }
  return nullptr;
}

void Server::addChannel(const std::string& channelName) {
  channels_.emplace_back(channelName);
}
