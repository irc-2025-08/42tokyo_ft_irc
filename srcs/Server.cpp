#include "../includes/Server.hpp"
#include "../includes/config.hpp"
#include "../includes/utils.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(int port, std::string password)
    : port_(port), password_(password), serverStatus_(STARTING) {
  initSocket();

  if (utils::setNonblock(socketFd_) < 0) {
    std::cerr << "Error: Failed to set socket to non-blocking" << std::endl;
    serverStatus_ = ERROR;
  }

  epollFd_ = epoll_create1(0);
  if (epollFd_ < 0) {
    std::cerr << "Error: Failed to create epoll instance" << std::endl;
    serverStatus_ = ERROR;
  }

  if (addEpollEvent(socketFd_, EPOLLIN) < 0) {
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

void Server::initSocket() {
  socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socketFd_ < 0) {
    std::cerr << "Error: Failed to create socket" << std::endl;
    serverStatus_ = ERROR;
    return;
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET; // IPv4
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port_);

  if (bind(socketFd_, (sockaddr *)&addr, sizeof(addr)) < 0) {
    std::cerr << "Error: Failed to bind socket" << std::endl;
    serverStatus_ = ERROR;
    return;
  }

  if (listen(socketFd_, config::backlog) < 0) {
    std::cerr << "Error: Failed to listen on socket" << std::endl;
    serverStatus_ = ERROR;
    return;
  }
}

void Server::run() {
  if (serverStatus_ == ERROR)
    return;

  std::cout << "Info: IRC server running on port " << port_ << " with password "
            << password_ << std::endl;

  eventLoop();
}

void Server::handleAccept() {
  while (true) {
    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    errno = 0;
    int client_fd =
        accept(socketFd_, (sockaddr *)&client_addr, &client_addr_len);
    if (errno == EAGAIN)
      return;
    if (client_fd < 0) {
      std::cerr << "Warning: Failed to accept client" << std::endl;
      continue;
    }

    if (utils::setNonblock(client_fd) < 0) {
      std::cerr << "Warning: Failed to set client socket to non-blocking"
                << std::endl;
      close(client_fd);
      continue;
    }

    if (addEpollEvent(client_fd, EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP) <
        0) {
      std::cerr << "Error: Failed to add client to epoll" << std::endl;
      close(client_fd);
      continue;
    }

    clients_.insert(std::make_pair(client_fd, Client(client_fd, client_addr)));
    std::cout << "Info: New client connected" << std::endl;
  }
}

void Server::handleRecv(Client &client) {
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

void Server::handleSend(Client &client) {
  std::cout << "Info: Sending message to client " << client.getFd()
            << std::endl;
}

void Server::handleClose(Client &client) {
  std::cout << "Info: Client " << client.getFd() << " disconnected"
            << std::endl;
  clients_.erase(client.getFd());
  close(client.getFd());
}

int Server::addEpollEvent(int fd, int events) {
  epoll_event ev;
  ev.data.fd = fd;
  ev.events = events;
  return epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev);
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
        handleAccept();
        continue;
      }

      Client &client = clients_[events[i].data.fd];
      if (events[i].events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
        handleClose(client);
        continue;
      }
      if (events[i].events & EPOLLIN)
        handleRecv(client);
      if (events[i].events & EPOLLOUT)
        handleSend(client);
    }
  }
}
