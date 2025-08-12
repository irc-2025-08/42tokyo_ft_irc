#include "../includes/Server.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
    return 1;
  }

  int port = std::atoi(argv[1]);
  std::string password = argv[2];

  if (port < 1024 || port > 65535) {
    std::cerr << "Error: port must be between 1024 and 65535" << std::endl;
    return 1;
  }

  // run the server
  Server server(port, password);
  server.run();

  // check if the server stopped with an error
  if (server.getStatus() == Server::ERROR) {
    std::cerr << "Server stopped with error" << std::endl;
    return 1;
  } else {
    std::cout << "Server stopped" << std::endl;
  }

  return 0;
}
