/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yxu <yxu@student.42tokyo.jp>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 00:18:26 by yxu               #+#    #+#             */
/*   Updated: 2025/09/02 00:18:28 by yxu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/CommandHandler.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>

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
  try {
    Server server(port, password);
    server.run();
    std::cout << "[INFO] ircd: Server stopped" << std::endl;
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "[ERROR] ircd: " << e.what() << ". " << strerror(errno)
              << std::endl;
    return 1;
  }
}
