#include "../includes/utils.hpp"
#include <fcntl.h>

int utils::set_nonblock(const int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
