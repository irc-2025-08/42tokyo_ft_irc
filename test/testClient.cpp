#include "../includes/Fd.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
    return 1;
  }

  Fd fd(socket(AF_INET, SOCK_STREAM, 0));
  if (!fd.isValid()) {
    strerror(errno);
    return 1;
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(atoi(argv[1]));

  if (connect(fd.get(), (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    strerror(errno);
    return 1;
  }
  std::cout << "Connected to server" << std::endl;

  return 0;
}
