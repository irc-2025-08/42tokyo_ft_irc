#pragma once

class ServerUtils {
public:
  /**
   * @brief set a fd to non-blocking
   * @param fd the fd to set to non-blocking
   * @return 0 on success, -1 on error
   */
  static int setNonblock(const int fd);

  /**
   * @brief add an event to the epoll
   * @param epollFd the epoll fd
   * @param fd the fd to add the event to
   * @param events the events to add
   * @return 0 on success, -1 on error
   */
  static int addEpollEvent(int epollFd, int fd, int events);

  /**
   * @brief initialize a socket and bind it to a port
   * @param port the port to bind the socket to
   * @return the socket fd on success, -1 on error
   */
  static int createSocket(int port);
};
