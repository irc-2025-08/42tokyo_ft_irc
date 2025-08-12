#include "../includes/utils.hpp"
#include <fcntl.h>

int utils::setNonblock(const int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
