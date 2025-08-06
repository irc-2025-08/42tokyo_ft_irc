#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>

class Server {
public:
  Server(int port, std::string password);
  ~Server();

  void run();

private:
  int port_;
  std::string password_;
};

#endif
