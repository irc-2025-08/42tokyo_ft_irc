#include "../includes/Server.hpp"
#include <iostream>

Server::Server(int port, std::string password)
    : port_(port), password_(password) {}

Server::~Server() {}

void Server::run() {
  std::cout << "IRC server running on port " << port_ << " with password "
            << password_ << std::endl;
}
