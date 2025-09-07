/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:53 by yxu               #+#    #+#             */
/*   Updated: 2025/09/06 21:48:24 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class ServerUtils {
public:
  /**
   * @brief set a fd to non-blocking
   * @return 0 on success, -1 on error
   */
  static int setNonblock(const int fd);

  /**
   * @brief add an event to the epoll
   * @return 0 on success, -1 on error
   */
  static int addEpollEvent(int epollFd, int fd, int events);

  /**
   * @brief initialize a socket and bind it to a port
   * @return the socket fd on success, -1 on error
   */
  static int createSocket(int port);
};
