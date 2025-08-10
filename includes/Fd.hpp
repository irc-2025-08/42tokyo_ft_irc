#pragma once

#include <unistd.h>

class Fd {
public:
  explicit Fd(int fd = -1) : fd_(fd) {};
  ~Fd() { if (isValid()) close(fd_); };

  int get() const { return fd_; };
  void set(int fd) {
    if (isValid()) close(fd_);
    fd_ = fd;
   };
  bool isValid() const { return fd_ > 0; };

private:
  int fd_;

  // Disable copy constructor and assignment operator
  Fd(const Fd &other);
  Fd &operator=(const Fd &other);
};
