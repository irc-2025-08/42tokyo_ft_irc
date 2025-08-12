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

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    strerror(errno);
    return 1;
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(atoi(argv[1]));

  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    strerror(errno);
    return 1;
  }
  std::cout << "Connected to server" << std::endl;

  char buffer[1024];
  while (true) {
    std::cin.getline(buffer, 1024);
    send(fd, buffer, strlen(buffer), 0);
    memset(buffer, 0, 1024);
  }

  return 0;
}
