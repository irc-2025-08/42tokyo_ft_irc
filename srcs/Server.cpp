#include "../includes/Server.hpp"
#include "../includes/Fd.hpp"
#include "../includes/config.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

Server::Server(int port, std::string password)
    : port_(port), password_(password), serverStatus_(STARTING), socket_() {
  initSocket(socket_);
}

Server::~Server() {}

Server::ServerStatus Server::getStatus() const { return serverStatus_; }

void Server::initSocket(Fd &fd) {
  fd.set(socket(AF_INET, SOCK_STREAM, 0));
  if (!fd.isValid())
    std::cerr << "Error: Failed to create socket" << std::endl;

  sockaddr_in addr;
  addr.sin_family = AF_INET; // IPv4
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port_);

  if (bind(fd.get(), (sockaddr *)&addr, sizeof(addr)) < 0)
    std::cerr << "Error: Failed to bind socket" << std::endl;

  if (listen(fd.get(), config::backlog) < 0)
    std::cerr << "Error: Failed to listen on socket" << std::endl;
}

void Server::run() {
  if (!socket_.isValid()) {
    serverStatus_ = ERROR;
    return;
  }
  serverStatus_ = RUNNING;
  std::cout << "Info: IRC server running on port " << port_ << " with password "
            << password_ << std::endl;

  while (serverStatus_ == RUNNING) {
    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    Fd client_fd(
        accept(socket_.get(), (sockaddr *)&client_addr, &client_addr_len));
    if (!client_fd.isValid()) {
      std::cerr << "Warning: Failed to accept client" << std::endl;
      continue;
    }

    std::cout << "Info: New client connected" << std::endl;
  }
}
