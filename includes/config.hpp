/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:45 by yxu               #+#    #+#             */
/*   Updated: 2025/09/02 00:18:45 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

namespace config {
  // listen() queue length: maximum pending connections
  const int backlog = 100;

  // I/O buffer size for send() and recv()
  const int bufferSize = 1024;

  // Maximum events per epoll_wait call
  const int maxEvents = 100;

  // I/O buffer size for recv() and send()
  const int ioBufferSize = 1024;

  const std::string serverName = "myserver";
}
