#include "../includes/Server.hpp"
#include "../includes/config.hpp"
#include "../includes/utils.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(int port, std::string password)
    : port_(port), password_(password), serverStatus_(STARTING), socket_(-1) {
  initSocket();

  if (utils::set_nonblock(socket_) < 0) {
    std::cerr << "Error: Failed to set socket to non-blocking" << std::endl;
    serverStatus_ = ERROR;
  }

  if (serverStatus_ == ERROR)
    std::cerr << "Error: Failed to initialize server" << std::endl;
  else
    serverStatus_ = RUNNING;
}

Server::~Server() {
  close(socket_);
  for (std::map<int, Client>::iterator it = clients_.begin();
       it != clients_.end(); ++it) {
    close(it->first);
  }
}

Server::ServerStatus Server::getStatus() const { return serverStatus_; }

void Server::initSocket() {
  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ < 0) {
    std::cerr << "Error: Failed to create socket" << std::endl;
    serverStatus_ = ERROR;
    return;
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET; // IPv4
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port_);

  if (bind(socket_, (sockaddr *)&addr, sizeof(addr)) < 0) {
    std::cerr << "Error: Failed to bind socket" << std::endl;
    serverStatus_ = ERROR;
    return;
  }

  if (listen(socket_, config::backlog) < 0) {
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

    int client_fd = accept(socket_, (sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        return;
      std::cerr << "Warning: Failed to accept client" << std::endl;
      continue;
    }

    if (utils::set_nonblock(client_fd) < 0) {
      std::cerr << "Warning: Failed to set client socket to non-blocking" << std::endl;
      close(client_fd);
      continue;
    }

    clients_.insert(std::make_pair(client_fd, Client(client_fd, client_addr)));
    std::cout << "Info: New client connected" << std::endl;
  }
}

void Server::handleRecv() {
  for (std::map<int, Client>::iterator it = clients_.begin();
       it != clients_.end(); ++it) {
    char buffer[config::buffer_size];
    memset(buffer, 0, config::buffer_size);
    while (it->second.getStatus() == Client::CONNECTED) {
      int bytes_read = recv(it->first, buffer, config::buffer_size, 0);
      if (bytes_read < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
          break;
        std::cerr << "Warning: Failed to receive data from client" << std::endl;
        break;
      } else if (bytes_read == 0) {
        std::cout << "Info: Client " << it->first << " disconnected" << std::endl;
        it->second.setStatus(Client::DISCONNECTED);
        break;
      }
      std::cout << "Info: Received message from client " << it->first << ": " << buffer << std::endl;
    }
  }
}

void Server::eventLoop() {
  while (serverStatus_ == RUNNING) {
    handleAccept();
    handleRecv();
  }
}
