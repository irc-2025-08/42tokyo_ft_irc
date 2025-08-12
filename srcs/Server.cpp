#include "../includes/Server.hpp"
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

Server::Server(int port, std::string password)
    : port_(port), password_(password), serverStatus_(STARTING) {
  socketFd_ = ServerUtils::createSocket(port_);
  if (socketFd_ < 0) {
    std::cerr << "Error: Failed to create socket" << std::endl;
    serverStatus_ = ERROR;
    return;
  }

  if (ServerUtils::setNonblock(socketFd_) < 0) {
    std::cerr << "Error: Failed to set socket to non-blocking" << std::endl;
    serverStatus_ = ERROR;
  }

  epollFd_ = epoll_create1(0);
  if (epollFd_ < 0) {
    std::cerr << "Error: Failed to create epoll instance" << std::endl;
    serverStatus_ = ERROR;
  }

  if (ServerUtils::addEpollEvent(epollFd_, socketFd_, EPOLLIN) < 0) {
    std::cerr << "Error: Failed to add socket to epoll" << std::endl;
    serverStatus_ = ERROR;
  }

  if (serverStatus_ == ERROR)
    std::cerr << "Error: Failed to initialize server" << std::endl;
  else
    serverStatus_ = RUNNING;
}

Server::~Server() {
  close(socketFd_);
  for (std::map<int, Client>::iterator it = clients_.begin();
       it != clients_.end(); ++it) {
    close(it->first);
  }
}

Server::ServerStatus Server::getStatus() const { return serverStatus_; }

void Server::run() {
  if (serverStatus_ == ERROR)
    return;

  std::cout << "Info: IRC server running on port " << port_ << " with password "
            << password_ << std::endl;

  eventLoop();
}

void Server::eventLoop() {
  epoll_event events[config::maxEvents];

  while (serverStatus_ == RUNNING) {
    int nfds = epoll_wait(epollFd_, events, config::maxEvents, -1);

    if (nfds < 0) {
      std::cerr << "Error: Failed to wait for events" << std::endl;
      continue;
    }

    for (int i = 0; i < nfds; i++) {
      if (events[i].data.fd == socketFd_) {
        ServerHandler::handleAccept(*this);
        continue;
      }

      Client &client = clients_[events[i].data.fd];

      if (events[i].events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
        ServerHandler::handleClose(*this, client);
        continue;
      }
      if (events[i].events & EPOLLIN)
        ServerHandler::handleRecv(*this, client);
      if (events[i].events & EPOLLOUT)
        ServerHandler::handleSend(*this, client);
    }
  }
}
