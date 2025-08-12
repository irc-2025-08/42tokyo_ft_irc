#pragma once

namespace config {
  // listen() queue length: maximum pending connections
  const int backlog = 100;

  // I/O buffer size for send() and recv()
  const int bufferSize = 1024;

  // Maximum events per epoll_wait call
  const int maxEvents = 100;
}
